/*
 * Piezo.h
 *
 *  Created on: Jul 16, 2026
 *      Author: natet
 */

#ifndef PIEZO_H_
#define PIEZO_H_

#include <stdint.h>

#define PIEZO_MIN_FREQUENCY_HZ   100u
#define PIEZO_MAX_FREQUENCY_HZ   2000u
#define PIEZO_FREQUENCY_STEP_HZ  50u

typedef enum
{
    NOTE_REST = 0,

    NOTE_C3  = 131,
    NOTE_CS3 = 139,
    NOTE_D3  = 147,
    NOTE_DS3 = 156,
    NOTE_E3  = 165,
    NOTE_F3  = 175,
    NOTE_FS3 = 185,
    NOTE_G3  = 196,
    NOTE_GS3 = 208,
    NOTE_A3  = 220,
    NOTE_AS3 = 233,
    NOTE_B3  = 247,

    NOTE_C4  = 262,
    NOTE_CS4 = 277,
    NOTE_D4  = 294,
    NOTE_DS4 = 311,
    NOTE_E4  = 330,
    NOTE_F4  = 349,
    NOTE_FS4 = 370,
    NOTE_G4  = 392,
    NOTE_GS4 = 415,
    NOTE_A4  = 440,
    NOTE_AS4 = 466,
    NOTE_B4  = 494,

    NOTE_C5  = 523,
    NOTE_CS5 = 554,
    NOTE_D5  = 587,
    NOTE_DS5 = 622,
    NOTE_E5  = 659,
    NOTE_F5  = 698,
    NOTE_FS5 = 740,
    NOTE_G5  = 784,
    NOTE_GS5 = 831,
    NOTE_A5  = 880,
    NOTE_AS5 = 932,
    NOTE_B5  = 988
} PiezoNote;


extern uint32_t piezoFrequency;


void Piezo_Init(void);
void Piezo_UpdateFrequency(void);
void Piezo_Stop(void);  // Disables the PWM output and forces the pin inactive
void Piezo_Start(void); // Enables the piezo sound


#endif /* PIEZO_H_ */
