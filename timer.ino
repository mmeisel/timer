// Pins
#define PIN_SLIDER_IN A0
#define PIN_ENABLE 5
#define PIN_DIR1 6
#define PIN_DIR2 7
#define PIN_BELL LED_BUILTIN

// Directions
#define DIR_STOP 0
#define DIR_OFF 255
#define DIR_REVERSE -1
#define DIR_FORWARD 1

#define STOP_OFF -1

// Slider properties
// Slider positions come from analogRead, so the range is 0 to 1023
#define FUDGE_FACTOR 10

// Stops where the motor will create virtual detentes.
// The first stop should always be the "OFF" position. The second stop should always be the zero
// position (where the alarm will sound).

struct Stop {
    int position;
    long ms;

    Stop(int _position, long _ms) : position(_position), ms(_ms) {}
};

const Stop STOPS[] = {
    Stop(0, STOP_OFF),
    Stop(32, 0),
    Stop(96,  2000),
    Stop(160, 4000),
    Stop(224, 6000),
    Stop(288, 8000),
    Stop(352, 10000),
    Stop(416, 12000),
    Stop(480, 14000),
    Stop(544, 16000),
    Stop(608, 18000),
    Stop(672, 20000),
    Stop(736, 22000),
    Stop(800, 24000),
    Stop(864, 26000),
    Stop(928, 28000),
    Stop(992, 30000)
};

const int NUM_STOPS = sizeof(STOPS) / sizeof(Stop);

int currentPosition;
int currentStop;
int currentDirection;
int nextStop;
unsigned long deadline;



void setDirection(int direction) {
    currentDirection = direction;

    switch (direction) {
        case DIR_REVERSE:
            digitalWrite(PIN_DIR1, LOW);
            digitalWrite(PIN_DIR2, HIGH);
            digitalWrite(PIN_ENABLE, HIGH);
            break;
        case DIR_STOP:
            digitalWrite(PIN_DIR1, LOW);
            digitalWrite(PIN_DIR2, LOW);
            digitalWrite(PIN_ENABLE, HIGH);
            break;
        case DIR_OFF:
            digitalWrite(PIN_DIR1, LOW);
            digitalWrite(PIN_DIR2, LOW);
            digitalWrite(PIN_ENABLE, LOW);
            break;
        case DIR_FORWARD:
            digitalWrite(PIN_DIR1, HIGH);
            digitalWrite(PIN_DIR2, LOW);
            digitalWrite(PIN_ENABLE, HIGH);
            break;
    }
}

int stopBefore(int position) {
    int lower = 0;
    int upper = NUM_STOPS;

    while (lower < upper - 1) {
        int middle = (lower + upper) / 2;

        if (position < STOPS[middle].position) {
            upper = middle;
        }
        else {
            lower = middle;
        }
    }

    return lower;
}

void setup() {
    pinMode(PIN_ENABLE, OUTPUT);
    pinMode(PIN_DIR1, OUTPUT);
    pinMode(PIN_DIR2, OUTPUT);
    pinMode(PIN_BELL, OUTPUT);

    currentPosition = analogRead(PIN_SLIDER_IN);
    currentStop = stopBefore(currentPosition);    // Don't move the slider on startup
    nextStop = currentStop;
    deadline = millis() + STOPS[currentStop].ms;

    digitalWrite(PIN_BELL, nextStop == 1 ? HIGH : LOW);

    Serial.begin(9600);
}

void loop() {
    unsigned long now = millis();
    long remaining = (long) (deadline - now);
    bool stopChanged = false;

    currentPosition = analogRead(PIN_SLIDER_IN);

    // Check if the slider is in the expected range and is moving in a timely manner.
    // If not, a human must have moved it, or be holding it in place.
    if (currentPosition < STOPS[nextStop].position - FUDGE_FACTOR ||
        currentPosition > STOPS[currentStop].position + FUDGE_FACTOR ||
        currentStop - nextStop > 1)
    {
        int nearestStop = stopBefore(currentPosition);

        deadline = now + STOPS[nearestStop].ms;
        currentStop = nearestStop;
        nextStop = nearestStop;
        stopChanged = true;
    }
    else if (nextStop > 1 && remaining <= STOPS[nextStop - 1].ms) {
        // Normal operation (not off or ringing): move the slider down as time passes
        nextStop--;
        stopChanged = true;
    }

    if (stopChanged) {
        // Enable or disable the bell based on whether we're headed to the zero stop (stop 1)
        digitalWrite(PIN_BELL, nextStop == 1 ? HIGH : LOW);
    }

    int desiredPosition = STOPS[nextStop].position;

    if (currentPosition < desiredPosition - FUDGE_FACTOR) {
        setDirection(DIR_FORWARD);
    }
    else if (currentPosition > desiredPosition + FUDGE_FACTOR) {
        setDirection(DIR_REVERSE);
    }
    else {
        // We've reached the desired position. When we first get here, actively stop the motor.
        // After that, we can just turn it off.
        setDirection(currentDirection == DIR_STOP ? DIR_OFF : DIR_STOP);
        currentStop = nextStop;
    }

    if (Serial.available() > 0) {
        Serial.read();  // Doesn't matter what it is, just output state on any input
        Serial.print(F("remaining = "));
        Serial.print(remaining);
        Serial.print(F(", currentStop = "));
        Serial.print(currentStop);
        Serial.print(F(", nextStop = "));
        Serial.print(nextStop);
        Serial.print(F(", currentPosition = "));
        Serial.print(currentPosition);
        Serial.print(F("\n"));
    }
}
