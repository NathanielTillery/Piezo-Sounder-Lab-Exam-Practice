/*
 * Author: Nathaniel C. Tillery
 * Date: 16, July 2026
 * Description: Piezo sounder
 *
 */

#include "msp.h"
#include "Piezo.h"
#include "GPIO.h"
#include "Song.h"
#include "SysTick.h"


uint32_t piezoFrequency = NOTE_A4;

/**
 * main.c
 */
void main(void)
{
    ButtonEvent buttonEvent;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // Stop watchdog timer

    GPIO_Init();
    Piezo_Init();
    Song_Init();
    SysTick_Init();

    while(1){
        Song_Service();
        buttonEvent = GPIO_GetButtonEvent();

        /* Both buttons toggle between playing and paused. */
        if(buttonEvent == BUTTON_EVENT_TOGGLE){
            Song_TogglePlayback();
        }
        else if(buttonEvent == BUTTON_EVENT_DECREASE){
            Song_DecreaseTempo();
        }
        else if(buttonEvent == BUTTON_EVENT_INCREASE){
            Song_IncreaseTempo();
        }
    }
}
