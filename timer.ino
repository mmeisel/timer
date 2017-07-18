#include <PID_v1.h>
#include <TimerOne.h>

// Pins
#define PIN_ANALOG_IN A0
#define PIN_OUTPUT 3
#define PIN_DIR1 7
#define PIN_DIR2 6
#define PIN_BELL LED_BUILTIN

// Directions
#define DIR_STOP 0
#define DIR_REVERSE -1
#define DIR_FORWARD 1

// Slider properties
// Slider positions come from analogRead, so the range is 0 to 1023
#define FUDGE_FACTOR 2
#define OFF_STOP -1

// Allowable positions where the motor will create virtual detentes.
// The first position will always be the zero position (where the alarm will sound).
// Position zero is reserved for "off", so the first position should be at least 2 * FUDGE_FACTOR.
const int STOPS[] = {
    32, 96, 160, 224, 288, 352, 416, 480, 544, 608, 672, 736, 800, 864, 928, 992
};

const int NUM_STOPS = sizeof(STOPS) / sizeof(int);

int currentPosition;
int expectedPosition;

bool isRunning;
volatile long msRemaining;



void tick() {
    if (msRemaining >= 1500) {
        msRemaining -= 1000;
    }
    else {
        msRemaining = 0;
    }
}

void setDirection(int direction) {
    Serial.print("Setting direction to ");
    Serial.print(direction);
    Serial.print("\n");

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

/*
int whichStop(int position) {
    if (position <= STOPS[0] + FUDGE_FACTOR) {
        return STOPS[0];
    }

    if (position >= STOPS[NUM_STOPS - 1] - FUDGE_FACTOR) {
        return STOPS[NUM_STOPS - 1];
    }

    for (int i = 1; i < NUM_STOPS - 1; i++) {
        int stop = STOPS[i];

        if (position >= stop - FUDGE_FACTOR && position <= stop + FUDGE_FACTOR) {
            return stop;
        }
    }

    // Not at any stop.
    return NO_STOP;
}
*/

int stopBefore(int position) {
    if (position < STOPS[0] - FUDGE_FACTOR) {
        return OFF_STOP;
    }

    for (int i = 1; i < NUM_STOPS; i++) {
        if (position < STOPS[i] - FUDGE_FACTOR) {
            return i - 1;
        }
    }

    return NUM_STOPS - 1;
}

int nearestStop(int position) {
    int before = stopBefore(position);

    if (before == NUM_STOPS - 1) {
        return NUM_STOPS - 1;
    }

    int after = before + 1;
    int beforeEnd = (before == OFF_STOP ? 0 : STOPS[before]) + FUDGE_FACTOR;
    int distance = STOPS[after] - beforeEnd - 2 * FUDGE_FACTOR;

    return position < beforeEnd + distance / 2 ? before : after;
}

// Linear with maximum of 30 seconds
long positionToMs(int position) {
    int firstPosition = STOPS[0];
    int lastPosition = STOPS[NUM_STOPS - 1];

    return lround(30000.0 * max(0, position - firstPosition) / (lastPosition - firstPosition));
}

int msToPosition(long ms) {
    int firstPosition = STOPS[0];
    int lastPosition = STOPS[NUM_STOPS - 1];

    return (int) lround(firstPosition + ms * (lastPosition - firstPosition) / 30000.0);
}



void setup() {
    pinMode(PIN_OUTPUT, OUTPUT);
    pinMode(PIN_DIR1, OUTPUT);
    pinMode(PIN_DIR2, OUTPUT);
    pinMode(PIN_BELL, OUTPUT);

    digitalWrite(PIN_BELL, LOW);

    currentPosition = analogRead(PIN_ANALOG_IN);
    expectedPosition = nearestStop(currentPosition);    // Don't move the slider on startup
    msRemaining = positionToMs(expectedPosition);

    Timer1.initialize(1000000); // 1 second in microseconds
    Timer1.attachInterrupt(tick);
    isRunning = true;

    Serial.begin(9600);
}

void loop() {
    currentPosition = analogRead(PIN_ANALOG_IN);

    Serial.print("currentPosition = ");
    Serial.print(currentPosition);
    Serial.print(", msRemaining = ");
    Serial.print(msRemaining);
    Serial.print("\n");

    int nearest = nearestStop(currentPosition);
    int nearestPosition = nearest == OFF_STOP ? 0 : STOPS[nearest];
    long curMsRemaining;

    if (nearestPosition <= STOPS[0]) {
        digitalWrite(PIN_BELL, nearest == OFF_STOP ? LOW : HIGH);
        if (isRunning) {
            Timer1.stop();
            isRunning = false;
        }
    }
    else if (nearestPosition != expectedPosition) {
        // Slider was moved to a value (other than zero or off), set msRemaining based on this
        if (!isRunning) {
            digitalWrite(PIN_BELL, LOW);
            Timer1.start();
            isRunning = true;
        }

        expectedPosition = nearestPosition;
        curMsRemaining = positionToMs(nearestPosition);

        noInterrupts();
        msRemaining = curMsRemaining;
        interrupts();
    }
    else {
        // Normal operation
        noInterrupts();
        curMsRemaining = msRemaining;
        interrupts();

        // Find the new expectedPosition based on ms remaining
        int newNearest = nearestStop(msToPosition(curMsRemaining));
        int newExpectedPosition = newNearest == 0 ? 0 : STOPS[newNearest];

        // If it changed, move the slider
        if (expectedPosition == newExpectedPosition) {
            setDirection(DIR_STOP);
        }
        else {
            Serial.print("newExpectedPosition = ");
            Serial.print(newExpectedPosition);
            Serial.print("\n");

            setDirection(expectedPosition > newExpectedPosition ? DIR_REVERSE : DIR_FORWARD);
            expectedPosition = newExpectedPosition;
        }
    }

    // If the position is off by more than the fudge factor, create the virtual detentes with the
    // motor.
    if (currentPosition < expectedPosition - FUDGE_FACTOR) {
        setDirection(DIR_FORWARD);
    }
    else if (currentPosition > expectedPosition + FUDGE_FACTOR) {
        setDirection(DIR_REVERSE);
    }
    else {
        setDirection(DIR_STOP);
    }

    delay(250);
}
