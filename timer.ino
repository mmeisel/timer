#include <PinChangeInterrupt.h>
#include <LowPower.h>

#define DEBUG 1

// Pins
#define PIN_SLIDER_IN A0
#define PIN_ENABLE 5
#define PIN_DIR1 6
#define PIN_DIR2 7
#define PIN_SLIDE_UP 9
#define PIN_SLIDE_DOWN 10
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
    // Every 30 seconds up to 5 minutes
    Stop(15,   0),
    Stop(31,   30),
    Stop(47,   60),
    Stop(63,   90),
    Stop(79,   120),
    Stop(95,   150),
    Stop(111,  180),
    Stop(127,  210),
    Stop(143,  240),
    Stop(159,  270),
    Stop(175,  300),
    // Every minute up to 30 minutes
    Stop(191,  360),
    Stop(207,  420),
    Stop(223,  480),
    Stop(239,  540),
    Stop(255,  600),
    Stop(271,  660),
    Stop(287,  720),
    Stop(303,  780),
    Stop(319,  840),
    Stop(335,  900),
    Stop(351,  960),
    Stop(367,  1020),
    Stop(383,  1080),
    Stop(399,  1140),
    Stop(415,  1200),
    Stop(431,  1260),
    Stop(447,  1320),
    Stop(463,  1380),
    Stop(479,  1440),
    Stop(495,  1500),
    Stop(511,  1560),
    Stop(527,  1620),
    Stop(543,  1680),
    Stop(559,  1740),
    Stop(575,  1800),
    // Every 2 minutes up to 1 hour
    Stop(591,  1920),
    Stop(607,  2040),
    Stop(623,  2160),
    Stop(639,  2280),
    Stop(655,  2400),
    Stop(671,  2520),
    Stop(687,  2640),
    Stop(703,  2760),
    Stop(719,  2880),
    Stop(735,  3000),
    Stop(751,  3120),
    Stop(767,  3240),
    Stop(783,  3360),
    Stop(799,  3480),
    Stop(815,  3600),
    // Every 10 minutes up to 2 hours
    Stop(831,  3960),
    Stop(847,  4320),
    Stop(863,  4680),
    Stop(879,  5040),
    Stop(895,  5400),
    Stop(911,  5760),
    Stop(927,  6120),
    Stop(943,  6480),
    Stop(959,  6840),
    Stop(975,  7200),
    // Every 20 minutes up to 3 hours
    Stop(991,  8400),
    Stop(1007, 9600),
    Stop(1023, 10800)
};

const int NUM_STOPS = sizeof(STOPS) / sizeof(Stop);

int currentPosition;
int currentStop;
int currentDirection;
int nextStop;
unsigned long deadline;
volatile unsigned long lastInterrupt;

#ifdef DEBUG
unsigned long lastReport;
#endif



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

void handleInterrupt() {
    lastInterrupt = millis();
}

void setup() {
    // Disable interrupt for millis(), we don't need it, and it won't be accurate anyway.
    // Uncomment when using circuit with external time source.
    // TIMSK0 &= ~_BV(TOIE0); // disable timer0 overflow interrupt

    pinMode(PIN_ENABLE, OUTPUT);
    pinMode(PIN_DIR1, OUTPUT);
    pinMode(PIN_DIR2, OUTPUT);
    pinMode(PIN_BELL, OUTPUT);
    pinMode(PIN_SLIDE_DOWN, INPUT_PULLUP);
    pinMode(PIN_SLIDE_UP, INPUT_PULLUP);

    currentPosition = analogRead(PIN_SLIDER_IN);
    currentStop = stopBefore(currentPosition);    // Don't move the slider on startup
    nextStop = currentStop;
    deadline = millis() + STOPS[currentStop].ms;

    digitalWrite(PIN_BELL, nextStop == 1 ? HIGH : LOW);

#ifdef DEBUG
    Serial.begin(9600);
    lastReport = 0;
#endif
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
        // Then we can sleep the processor until it's time for the next tick. We will get an
        // interrupt if the slider is moved in the meantime.
        if (currentDirection == DIR_FORWARD || currentDirection == DIR_REVERSE) {
            currentStop = nextStop;
            setDirection(DIR_STOP);
        }
        else {
            setDirection(DIR_OFF);
            if (digitalRead(PIN_SLIDE_UP) == LOW && digitalRead(PIN_SLIDE_DOWN) == LOW) {
                attachPCINT(digitalPinToPCINT(PIN_SLIDE_UP), handleInterrupt, RISING);
                attachPCINT(digitalPinToPCINT(PIN_SLIDE_DOWN), handleInterrupt, RISING);

                LowPower.idle(SLEEP_FOREVER, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_ON, SPI_OFF,
                              USART0_ON, TWI_OFF);

                detachPCINT(digitalPinToPCINT(PIN_SLIDE_UP));
                detachPCINT(digitalPinToPCINT(PIN_SLIDE_DOWN));
            }
        }
    }

#ifdef DEBUG
    if ((long) (now - lastReport) > 2000) {
        lastReport = now;
        Serial.read();  // Doesn't matter what it is, just output state on any input
        Serial.print(F("remaining = "));
        Serial.print(remaining);
        Serial.print(F(", currentStop = "));
        Serial.print(currentStop);
        Serial.print(F(", nextStop = "));
        Serial.print(nextStop);
        Serial.print(F(", currentPosition = "));
        Serial.print(currentPosition);
        Serial.print(F(", lastInterrupt = "));
        Serial.print(lastInterrupt);
        Serial.print(F(", slideUp = "));
        Serial.print(digitalRead(PIN_SLIDE_UP));
        Serial.print(F(", slideDown = "));
        Serial.print(digitalRead(PIN_SLIDE_DOWN));
        Serial.print(F("\n"));
    }
#endif

}
