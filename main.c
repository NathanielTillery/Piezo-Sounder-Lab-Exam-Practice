/*
 * Author: Nathaniel C. Tillery
 * Date: 16, July 2026
 * Description: Piezo sounder
 *
 */

#include "msp.h"
#include <stdbool.h>
#include "Piezo.h"
#include "GPIO.h"


uint32_t piezoFrequency = NOTE_A4;


/**
 * main.c
 */
void main(void)
{
    bool piezoSounding = false;
    ButtonEvent buttonEvent;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // Stop watchdog timer

    GPIO_Init();
    Piezo_Init();

    while(1){
        buttonEvent = GPIO_GetButtonEvent();

        if(buttonEvent == BUTTON_EVENT_TOGGLE){
            piezoSounding = !piezoSounding;

            if(piezoSounding){
                Piezo_Start();
            }
            else{
                Piezo_Stop();
            }
        }
        else if(buttonEvent == BUTTON_EVENT_DECREASE){
            if(piezoFrequency >= PIEZO_MIN_FREQUENCY_HZ + PIEZO_FREQUENCY_STEP_HZ){
                piezoFrequency -= PIEZO_FREQUENCY_STEP_HZ;
            }
            else{
                piezoFrequency = PIEZO_MIN_FREQUENCY_HZ;
            }

            Piezo_UpdateFrequency();
        }
        else if(buttonEvent == BUTTON_EVENT_INCREASE){
            if(piezoFrequency <= PIEZO_MAX_FREQUENCY_HZ - PIEZO_FREQUENCY_STEP_HZ){
                piezoFrequency += PIEZO_FREQUENCY_STEP_HZ;
            }
            else{
                piezoFrequency = PIEZO_MAX_FREQUENCY_HZ;
            }

            Piezo_UpdateFrequency();
        }
    }
}
