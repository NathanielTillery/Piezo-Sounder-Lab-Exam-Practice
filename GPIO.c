/*
 * Author: Nathaniel C. Tillery
 * Date: 16, July 2026
 * Description: Piezo sounder
 *
 */

#include "GPIO.h"
#include "msp.h"

#define BUTTON_MASK             (BIT1 | BIT4)
#define BUTTON_DEBOUNCE_TICKS   60000u

static volatile ButtonEvent buttonEvent = BUTTON_EVENT_NONE;


void GPIO_Init(void){

    /* Set up P1.1 and P1.4 buttons as input */
    P1->SEL0 &= ~BUTTON_MASK;                  // Set buttons to GPIO
    P1->SEL1 &= ~BUTTON_MASK;
    P1->DIR  &= ~BUTTON_MASK;                  // Set buttons as inputs

    P1->REN |= BUTTON_MASK;                    // Enable pull resistors
    P1->OUT |= BUTTON_MASK;                    // Select pull-up resistors

    P1->IES |= BUTTON_MASK;                    // Interrupt on button press
    P1->IFG &= ~BUTTON_MASK;                   // Clear pending flags
    P1->IE  |= BUTTON_MASK;                    // Enable both button interrupts

    NVIC_EnableIRQ(PORT1_IRQn);                 // Enable the Port 1 interrupt

    __enable_interrupt();                       // Enable processor interrupts

}


ButtonEvent GPIO_GetButtonEvent(void)
{
    ButtonEvent event;

    __disable_interrupt();
    event = buttonEvent;
    buttonEvent = BUTTON_EVENT_NONE;
    __enable_interrupt();

    return event;
}


void PORT1_IRQHandler(void){

    P1->IE &= ~BUTTON_MASK;                    // Ignore switch bounce
    P1->IFG &= ~BUTTON_MASK;

    /* Sample both buttons after a 20 ms one-shot debounce interval. */
    SysTick->CTRL = 0;
    SysTick->LOAD = BUTTON_DEBOUNCE_TICKS - 1u;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk;
}


void SysTick_Handler(void){
    uint8_t pressedButtons;

    SysTick->CTRL = 0;                         // Stop the one-shot timer
    pressedButtons = (uint8_t)(~P1->IN) & BUTTON_MASK;

    if(pressedButtons == BUTTON_MASK){
        buttonEvent = BUTTON_EVENT_TOGGLE;
    }
    else if(pressedButtons & BIT1){
        buttonEvent = BUTTON_EVENT_DECREASE;
    }
    else if(pressedButtons & BIT4){
        buttonEvent = BUTTON_EVENT_INCREASE;
    }

    P1->IFG &= ~BUTTON_MASK;
    P1->IE |= BUTTON_MASK;
}

