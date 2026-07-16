/*
 * Author: Nathaniel C. Tillery
 * Date: 16, July 2026
 * Description: Piezo sounder
 *
 */
#include "Piezo.h"
#include "msp.h"

#define PIEZO_TIMER_CLOCK_HZ 750000u

static void TIMER_A_Init(void);


/* Initialize the P2.4 Timer_A0.1 PWM output. */
void Piezo_Init(void){
    P2->SEL0 |= BIT4;                       // Select TA0.1 on P2.4
    P2->SEL1 &= ~BIT4;
    P2->DIR |= BIT4;
    P2->OUT &= ~BIT4;

    TIMER_A_Init();
}

/* Apply the shared frequency using a 50 percent duty cycle. */
void Piezo_UpdateFrequency(void){
    uint32_t periodCounts;

    if(piezoFrequency < PIEZO_MIN_FREQUENCY_HZ){
        piezoFrequency = PIEZO_MIN_FREQUENCY_HZ;
    }
    else if(piezoFrequency > PIEZO_MAX_FREQUENCY_HZ){
        piezoFrequency = PIEZO_MAX_FREQUENCY_HZ;
    }

    periodCounts = PIEZO_TIMER_CLOCK_HZ / piezoFrequency;
    TIMER_A0->CCR[0] = (uint16_t)(periodCounts - 1u);
    TIMER_A0->CCR[1] = (uint16_t)(periodCounts / 2u);
    TIMER_A0->CTL |= TIMER_A_CTL_CLR;
}


/* Enable PWM at the selected frequency. */
void Piezo_Start(void){
    Piezo_UpdateFrequency();
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK |
                    TIMER_A_CTL_ID__4 |
                    TIMER_A_CTL_MC__UP |
                    TIMER_A_CTL_CLR;
}


/* Stop the timer and force the TA0.1 output low. */
void Piezo_Stop(void){
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK |
                    TIMER_A_CTL_ID__4 |
                    TIMER_A_CTL_MC__STOP |
                    TIMER_A_CTL_CLR;
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_0;
}


/* Initialize Timer_A0 without starting the PWM output. */
static void TIMER_A_Init(void){
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK |
                    TIMER_A_CTL_ID__4 |
                    TIMER_A_CTL_MC__STOP |
                    TIMER_A_CTL_CLR;
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_0;
    Piezo_UpdateFrequency();

}


