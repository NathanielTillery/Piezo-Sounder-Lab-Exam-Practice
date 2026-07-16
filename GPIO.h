/*
 * GPIO.h
 *
 *  Created on: Jul 16, 2026
 *      Author: natet
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <msp.h>

#define BUTTON_MASK             (BIT1 | BIT4)
#define BUTTON_DEBOUNCE_SAMPLES 10u


typedef enum
{
    BUTTON_EVENT_NONE,
    BUTTON_EVENT_DECREASE,
    BUTTON_EVENT_INCREASE,
    BUTTON_EVENT_TOGGLE
} ButtonEvent;


void GPIO_Init(void);
void GPIO_DebounceTick(void);
ButtonEvent GPIO_GetButtonEvent(void);


#endif /* GPIO_H_ */
