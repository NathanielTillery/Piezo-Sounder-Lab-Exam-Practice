/*
 * GPIO.h
 *
 *  Created on: Jul 16, 2026
 *      Author: natet
 */

#ifndef GPIO_H_
#define GPIO_H_

typedef enum
{
    BUTTON_EVENT_NONE,
    BUTTON_EVENT_DECREASE,
    BUTTON_EVENT_INCREASE,
    BUTTON_EVENT_TOGGLE
} ButtonEvent;


void GPIO_Init(void);
ButtonEvent GPIO_GetButtonEvent(void);


#endif /* GPIO_H_ */
