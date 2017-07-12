#include <PID_v1.h>
#include <TimerOne.h>

// Pins
#define PIN_ANALOG_IN A0
#define PIN_ANALOG_OUT 3
#define PIN_BELL LED_BUILTIN

// Slider properties
#define ZERO_POSITION 8.0
#define MAX_POSITION 1023.0
#define FUDGE_FACTOR 2.0

// Current slider position
double position;

// Currently desired slider position
double setPoint;

// Output of PID algorithm
double pidOutput;

// See http://playground.arduino.cc/Code/PIDLibrary
// P_ON_M (proportional on measurement) mode is better for linear slide pots
PID pid(&position, &pidOutput, &setPoint, 2, 5, 1, P_ON_M, DIRECT);

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

// Linear with maximum of one minute
long positionToMs(double position) {
    return round(60000.0 * max(0, position - ZERO_POSITION) / (MAX_POSITION - ZERO_POSITION));
}

double msToPosition(long ms) {
    return ZERO_POSITION + ms * (MAX_POSITION - ZERO_POSITION) / 60000.0;
}



void setup() {
    digitalWrite(PIN_BELL, LOW);

    position = analogRead(PIN_ANALOG_IN);
    setPoint = position;    // Don't move the slider on startup
    msRemaining = positionToMs(position);

    pid.SetMode(AUTOMATIC);

    Timer1.initialize(1000000); // 1 second in microseconds
    Timer1.attachInterrupt(tick);
    isRunning = true;

    Serial.begin(9600);
}

void loop() {
    position = analogRead(PIN_ANALOG_IN);

    Serial.print("position = ");
    Serial.print(position);
    Serial.print(", msRemaining = ");
    Serial.print(msRemaining);
    Serial.print("\n");

    if (position <= ZERO_POSITION + FUDGE_FACTOR) {
        // Ring the bell based on whether we are at zero (DING!) or below zero (off).
        bool isInOffPosition = position < ZERO_POSITION - FUDGE_FACTOR;

        digitalWrite(PIN_BELL, isInOffPosition ? LOW : HIGH);
        Timer1.stop();
        isRunning = false;

        if (!isInOffPosition) {
            Serial.print("DING!\n");
        }
    }
    else {
        bool setMsRemaining = false;
        long curMsRemaining;

        if (!isRunning) {
            Timer1.start();
            isRunning = true;
        }

        // If the position is off by more than the fudge factor, assume the slider was moved and
        // reset msRemaining based on the position.
        if (abs(position - setPoint) > FUDGE_FACTOR) {
            setMsRemaining = true;
            curMsRemaining = positionToMs(position);
        }

        noInterrupts();
        if (setMsRemaining) {
            msRemaining = curMsRemaining;
        }
        else {
            curMsRemaining = msRemaining;
        }
        interrupts();

        // Move the slider towards zero based on seconds remaining
        setPoint = msToPosition(curMsRemaining);

        Serial.print("setPoint = ");
        Serial.print(setPoint);
        Serial.print("\n");

        pid.Compute();
        analogWrite(PIN_ANALOG_OUT, pidOutput);
    }
    delay(250);
}
