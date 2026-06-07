#include "debouncer.h"

Debouncer::Debouncer()
{    
    index = 0;
    debouncedState = 0b1111;
    changed = 0x00;
    
    for(int i = 0; i < NUM_DEBOUNCE_SAMPLES; i++)
    {
        state[i] = 0b1111;
    }
}

void Debouncer::processButtons(uint32_t portValues)
{
    uint8_t prevDebouncedState = debouncedState;
    
    state[index] = portValues;
    
    debouncedState = 0xff;
    for(int i = 0; i < NUM_DEBOUNCE_SAMPLES; i++)
    {
        debouncedState &= state[i];
    }
    
    index++;
    if(index >= NUM_DEBOUNCE_SAMPLES) index = 0;
    
    changed = debouncedState ^ prevDebouncedState;
}

uint32_t Debouncer::buttonPressed(uint32_t buttonPins)
{
    return (changed & debouncedState) & buttonPins;
}

uint32_t Debouncer::buttonReleased(uint32_t buttonPins)
{
    return (changed & (~debouncedState)) & buttonPins;
}

uint32_t Debouncer::buttonHeld(uint32_t buttonPins)
{
    return debouncedState & buttonPins;
}

