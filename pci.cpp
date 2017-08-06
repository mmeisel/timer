#include <Arduino.h>
#include <pins_arduino.h>
#include "pci.h"

namespace pci {
    void enable(uint8_t pin) {
        *digitalPinToPCMSK(pin) |= bit(digitalPinToPCMSKbit(pin));  // Enable pin
        PCIFR |= bit(digitalPinToPCICRbit(pin)); // Clear any outstanding interrupt
        PCICR |= bit(digitalPinToPCICRbit(pin)); // Enable interrupt for the group
    }

    // Disable a pin change interrupt
    void disable(uint8_t pin) {
        volatile uint8_t* mask = digitalPinToPCMSK(pin);

        *mask &= ~bit(digitalPinToPCMSKbit(pin));  // Disable pin
        if (*mask == 0) {
            PCICR &= ~bit(digitalPinToPCICRbit(pin)); // Disable interrupt for the group
        }
    }
}
