#ifndef __TIMER_PCI_H__
#define __TIMER_PCI_H__

// Pin change interrupts
namespace pci {
    void enable(uint8_t pin);
    void disable(uint8_t pin);
}

#endif
