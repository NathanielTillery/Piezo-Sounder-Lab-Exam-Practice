/*
 * Author: Nathaniel C. Tillery
 * Date: 16, July 2026
 * Description: Piezo sounder
 *
 */

#include "GPIO.h"
#include "msp.h"

static volatile ButtonEvent pendingButtonEvent = BUTTON_EVENT_NONE;

void GPIO_Init(void){

    /* Set up P1.1 and P1.4 buttons as input */
    P1->SEL0 &= ~BUTTON_MASK;                  // Set buttons to GPIO
    P1->SEL1 &= ~BUTTON_MASK;
    P1->DIR  &= ~BUTTON_MASK;                  // Set buttons as inputs

    P1->REN |= BUTTON_MASK;                    // Enable pull resistors
    P1->OUT |= BUTTON_MASK;                    // Select pull-up resistors

    P1->IE &= ~BUTTON_MASK;                    // Buttons are sampled by SysTick
    P1->IFG &= ~BUTTON_MASK;

    __enable_interrupt();                       // Enable processor interrupts

}


/* Periodically sample both buttons and accept a state after 10 stable samples. */
void GPIO_DebounceTick(void)
{
    static uint8_t debounceCandidate = 0u;
    static uint8_t debouncedButtons = 0u;
    static uint8_t stableSampleCount = 0u;
    uint8_t rawButtons = (uint8_t)(~P1->IN) & BUTTON_MASK;
    uint8_t previousButtons;

    if(rawButtons != debounceCandidate){
        debounceCandidate = rawButtons;
        stableSampleCount = 1u;
        return;
    }

    if(stableSampleCount < BUTTON_DEBOUNCE_SAMPLES){
        stableSampleCount++;
    }

    if((stableSampleCount < BUTTON_DEBOUNCE_SAMPLES) ||
       (debounceCandidate == debouncedButtons)){
        return;
    }

    previousButtons = debouncedButtons;
    debouncedButtons = debounceCandidate;

    if(debouncedButtons == BUTTON_MASK){
        pendingButtonEvent = BUTTON_EVENT_TOGGLE;
    }
    else if(previousButtons == 0u){
        if(debouncedButtons & BIT1){
            pendingButtonEvent = BUTTON_EVENT_DECREASE;
        }
        else if(debouncedButtons & BIT4){
            pendingButtonEvent = BUTTON_EVENT_INCREASE;
        }
    }
}


ButtonEvent GPIO_GetButtonEvent(void)
{
    ButtonEvent event;

    __disable_interrupt();
    event = pendingButtonEvent;
    pendingButtonEvent = BUTTON_EVENT_NONE;
    __enable_interrupt();

    return event;
}
