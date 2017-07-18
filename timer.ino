// Pins
#define PIN_ANALOG_IN A0
#define PIN_OUTPUT 5
#define PIN_DIR1 6
#define PIN_DIR2 7
#define PIN_BELL LED_BUILTIN

// Directions
#define DIR_STOP 0
#define DIR_REVERSE -1
#define DIR_FORWARD 1

#define OFF -1

// Slider properties
// Slider positions come from analogRead, so the range is 0 to 1023
#define FUDGE_FACTOR 6

// Stops where the motor will create virtual detentes.
// The first stop should always be the "OFF" position. The second stop should always be the zero
// position (where the alarm will sound).

struct Stop {
    int position;
    long ms;

    Stop(int _position, long _ms) : position(_position), ms(_ms) {}
};

const Stop STOPS[] = {
    Stop(0, OFF),
    Stop(32, 0),
    Stop(96, 2000),
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
bool isMoving;
unsigned long deadline;



void setDirection(int direction) {
    switch (direction) {
        case DIR_STOP:
            digitalWrite(PIN_DIR1, LOW);
            digitalWrite(PIN_DIR2, LOW);
            digitalWrite(PIN_OUTPUT, LOW);
            break;
        case DIR_REVERSE:
            digitalWrite(PIN_DIR1, LOW);
            digitalWrite(PIN_DIR2, HIGH);
            digitalWrite(PIN_OUTPUT, HIGH);
            break;
        case DIR_FORWARD:
            digitalWrite(PIN_DIR1, HIGH);
            digitalWrite(PIN_DIR2, LOW);
            digitalWrite(PIN_OUTPUT, HIGH);
            break;
    }
}

int stopBefore(int position) {
    for (int i = 1; i < NUM_STOPS; i++) {
        if (position < STOPS[i].position - FUDGE_FACTOR) {
            return i - 1;
        }
    }

    return NUM_STOPS - 1;
}

int findNearestStop(int position) {
    int before = stopBefore(position);

    if (before == NUM_STOPS - 1) {
        return NUM_STOPS - 1;
    }

    int after = before + 1;
    int beforeEnd = STOPS[before].position + FUDGE_FACTOR;
    int distance = STOPS[after].position - beforeEnd - 2 * FUDGE_FACTOR;

    return position < beforeEnd + distance / 2 ? before : after;
}

void setup() {
    pinMode(PIN_OUTPUT, OUTPUT);
    pinMode(PIN_DIR1, OUTPUT);
    pinMode(PIN_DIR2, OUTPUT);
    pinMode(PIN_BELL, OUTPUT);

    digitalWrite(PIN_BELL, LOW);

    currentPosition = analogRead(PIN_ANALOG_IN);
    currentStop = findNearestStop(currentPosition);    // Don't move the slider on startup

    deadline = millis() + STOPS[currentStop].ms;
    Serial.begin(9600);
}

void loop() {
    currentPosition = analogRead(PIN_ANALOG_IN);

    if (!isMoving) {
        unsigned long now = millis();
        int nearestStop = findNearestStop(currentPosition);
        bool stopChanged = false;

        if (nearestStop != currentStop) {
            // Slider was moved by a human
            deadline = now + STOPS[nearestStop].ms;
            currentStop = nearestStop;
            stopChanged = true;
        }
        else if (currentStop > 1 && (long) (deadline - now) <= STOPS[currentStop - 1].ms) {
            // Time to move the slider to the next stop down
            currentStop--;
            stopChanged = true;
            isMoving = true;
        }

        if (stopChanged) {
            // Enable or disable the bell based on whether we're at the zero stop (stop 1)
            digitalWrite(PIN_BELL, currentStop == 1 ? HIGH : LOW);
        }

        if (Serial.available() > 0) {
            Serial.read();  // Doesn't matter what it is, just output state on any input
            Serial.print("timeRemaining = ");
            Serial.print((long) (deadline - now));
            Serial.print(", currentStop = ");
            Serial.print(currentStop);
            Serial.print(", currentPosition = ");
            Serial.print(currentPosition);
            Serial.print(", nearestStop = ");
            Serial.print(nearestStop);
            Serial.print("\n");
        }
    }

    // Normal operation, keep the slider in the desired position
    int expectedPosition = STOPS[currentStop].position;

    if (currentPosition < expectedPosition - FUDGE_FACTOR) {
        setDirection(DIR_FORWARD);
    }
    else if (currentPosition > expectedPosition + FUDGE_FACTOR) {
        setDirection(DIR_REVERSE);
    }
    else {
        setDirection(DIR_STOP);
        isMoving = false;
    }
}
