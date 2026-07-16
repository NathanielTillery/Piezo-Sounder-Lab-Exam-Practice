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
#include "SysTick.h"


uint32_t piezoFrequency = NOTE_A4;
uint32_t SysTickPeriod = SYSTICK_TONE_PERIOD_DEFAULT;

volatile int SysTickTimeout = 0;
volatile int SysTickCount = 0;

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
    SysTick_Init();

    while(1){
        /* SysTick decides when stuff runs */
       if(SysTickTimeout){
           SysTickCount++;

           /* Every time SysTick runs, we increment the rising tone on the piezo */
           if(piezoFrequency <= PIEZO_MAX_FREQUENCY_HZ - PIEZO_FREQUENCY_STEP_HZ){
               piezoFrequency += PIEZO_FREQUENCY_STEP_HZ;
           }
           else{
               piezoFrequency = PIEZO_MIN_FREQUENCY_HZ;
           }
           Piezo_UpdateFrequency();

           SysTickTimeout = 0; //Turn off the SysTick Flag
       }
        buttonEvent = GPIO_GetButtonEvent();

        /* If button is pressed, toggle the sound */
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
            if(SysTickPeriod >= SYSTICK_TONE_PERIOD_MIN + SYSTICK_PERIOD_STEP){
                SysTickPeriod -= SYSTICK_PERIOD_STEP;
            }
            else{
                SysTickPeriod = SYSTICK_TONE_PERIOD_MIN;
            }

            SysTick_setTonePeriod(SysTickPeriod);
        }
        else if(buttonEvent == BUTTON_EVENT_INCREASE){
            if(SysTickPeriod <= SYSTICK_TONE_PERIOD_MAX - SYSTICK_PERIOD_STEP){
                SysTickPeriod += SYSTICK_PERIOD_STEP;
            }
            else{
                SysTickPeriod = SYSTICK_TONE_PERIOD_MAX;
            }

            SysTick_setTonePeriod(SysTickPeriod);
        }

    }
}
