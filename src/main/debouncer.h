#ifndef DEBOUNCER_H
#define DEBOUNCER_H

#include <stdint.h>

#define NUM_DEBOUNCE_SAMPLES 10

class Debouncer
{
  private:
    uint32_t index;
    uint32_t state[NUM_DEBOUNCE_SAMPLES];
    uint32_t debouncedState;
    uint32_t changed;
      
  public:
    Debouncer(void);
    void processButtons(uint32_t portValues);
    uint32_t buttonPressed(uint32_t buttonPins);
    uint32_t buttonReleased(uint32_t buttonPins);
    uint32_t buttonHeld(uint32_t buttonPins);
};

#endif