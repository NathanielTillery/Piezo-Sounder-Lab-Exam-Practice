/*
 * SysTick.c
 *
 *  Created on: Jun 16, 2026
 *      Author: natet
 *      This C source file was created by Nathaniel C. Tillery for EGR424 Project 2.
 *      It is an Application Programming Interface file meant to act as a versatile
 *      driver for the SysTick functions in this project.
 */


#include "SysTick.h"
#include "Song.h"


void SysTick_Init(void){
    SysTick_setPeriod(SYSTICK_TICK_COUNTS); // 1 ms tick with a 3 MHz clock
    SysTick->VAL = 0u;
    SysTick_enableInterrupt();
    SysTick_enableModule();

}

void SysTick_Handler(void){
    GPIO_DebounceTick();
    Song_Tick1ms();
}


/* Following code taken from Texas Instruments driverlib systick.c */
void SysTick_enableModule(void){
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

void SysTick_disableModule(void){
    SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
}


void SysTick_enableInterrupt(void){
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

void SysTick_disableInterrupt(void){
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);
}

void SysTick_setPeriod(uint32_t period){
    //
    // Check the arguments.
    //
    //ASSERT((period > 0) && (period <= 16777216));

    // Here's how we did it in the other lab for reference.
    // 250 is the ms and 3000 is the clock... So 250 * 3000 is
    // the period for that one. (not necessarily this one)
    //SysTick->LOAD = (3000 * 250) - 1;

    //
    // Set the period of the SysTick counter.
    //
    SysTick->LOAD = period - 1;
}

uint32_t SysTick_getPeriod(void){
    //
    // Return the period of the SysTick counter.
    //
    return (SysTick->LOAD + 1);
}

uint32_t SysTick_getValue(void){
    //
    // Return the current value of the SysTick counter.
    //
    return (SysTick->VAL);
}
