/*
 * Song.c
 *
 *  Created on: Jul 16, 2026
 *      Author: natet
 *  Description: Interrupt-timed monophonic song player for the piezo driver.
 */

#include "Song.h"

#include <stdbool.h>
#include <stdint.h>
#include "msp.h"
#include "Piezo.h"

#define SONG_COUNT                   3u
#define SONG_TEMPO_DEFAULT_BPM       120u
#define SONG_TEMPO_MIN_BPM           60u
#define SONG_TEMPO_MAX_BPM           200u
#define SONG_TEMPO_STEP_BPM          10u
#define SONG_NOTE_GAP_MS             40u
#define SONG_INTER_SONG_GAP_MS       3000u
#define ARRAY_LENGTH(array)          (sizeof(array) / sizeof((array)[0]))

typedef struct
{
    uint16_t frequency;
    uint8_t durationInEighthNotes;
} SongNote;

typedef struct
{
    const SongNote *notes;
    uint16_t noteCount;
} SongDefinition;

typedef enum
{
    SONG_PHASE_NOTE,
    SONG_PHASE_NOTE_GAP,
    SONG_PHASE_INTER_SONG_GAP
} SongPhase;

/* Short excerpt of the traditional Swedish melody Herr Mannelig. */
static const SongNote herrMannelig[] =
{
    {NOTE_E5, 2u}, {NOTE_F5, 2u}, {NOTE_E5, 2u}, {NOTE_D5, 2u},
    {NOTE_D5, 2u}, {NOTE_C5, 2u}, {NOTE_C5, 2u}, {NOTE_A4, 2u},
    {NOTE_C5, 2u}, {NOTE_D5, 2u}, {NOTE_C5, 2u}, {NOTE_B4, 2u},
    {NOTE_A4, 4u}, {NOTE_A4, 2u}, {NOTE_B4, 2u}, {NOTE_C5, 2u},
    {NOTE_C5, 2u}, {NOTE_C5, 2u}, {NOTE_D5, 2u}, {NOTE_D5, 2u},
    {NOTE_C5, 2u}, {NOTE_E5, 2u}, {NOTE_C5, 4u}
};

/* One verse of the traditional French round Frere Jacques. */
static const SongNote frereJacques[] =
{
    {NOTE_C4, 2u}, {NOTE_D4, 2u}, {NOTE_E4, 2u}, {NOTE_C4, 2u},
    {NOTE_C4, 2u}, {NOTE_D4, 2u}, {NOTE_E4, 2u}, {NOTE_C4, 2u},
    {NOTE_E4, 2u}, {NOTE_F4, 2u}, {NOTE_G4, 4u},
    {NOTE_E4, 2u}, {NOTE_F4, 2u}, {NOTE_G4, 4u},
    {NOTE_G4, 1u}, {NOTE_A4, 1u}, {NOTE_G4, 1u}, {NOTE_F4, 1u},
    {NOTE_E4, 2u}, {NOTE_C4, 2u},
    {NOTE_G4, 1u}, {NOTE_A4, 1u}, {NOTE_G4, 1u}, {NOTE_F4, 1u},
    {NOTE_E4, 2u}, {NOTE_C4, 2u},
    {NOTE_C4, 2u}, {NOTE_G3, 2u}, {NOTE_C4, 4u},
    {NOTE_C4, 2u}, {NOTE_G3, 2u}, {NOTE_C4, 4u}
};

/* One verse of the traditional English melody London Bridge. */
static const SongNote londonBridge[] =
{
    {NOTE_G4, 2u}, {NOTE_A4, 1u}, {NOTE_G4, 1u}, {NOTE_F4, 2u},
    {NOTE_E4, 2u}, {NOTE_F4, 2u}, {NOTE_G4, 4u},
    {NOTE_D4, 2u}, {NOTE_E4, 2u}, {NOTE_F4, 4u},
    {NOTE_E4, 2u}, {NOTE_F4, 2u}, {NOTE_G4, 4u},
    {NOTE_G4, 2u}, {NOTE_A4, 1u}, {NOTE_G4, 1u}, {NOTE_F4, 2u},
    {NOTE_E4, 2u}, {NOTE_F4, 2u}, {NOTE_G4, 4u},
    {NOTE_D4, 2u}, {NOTE_G4, 2u}, {NOTE_E4, 2u}, {NOTE_C4, 4u}
};

static const SongDefinition songs[SONG_COUNT] =
{
    {herrMannelig, (uint16_t)ARRAY_LENGTH(herrMannelig)},
    {frereJacques, (uint16_t)ARRAY_LENGTH(frereJacques)},
    {londonBridge, (uint16_t)ARRAY_LENGTH(londonBridge)}
};

static volatile bool playing = false;
static volatile bool phaseExpired = false;
static volatile uint32_t remainingMs = 0u;
static uint16_t tempoBpm = SONG_TEMPO_DEFAULT_BPM;
static uint8_t currentSong = 0u;
static uint16_t currentNote = 0u;
static SongPhase phase = SONG_PHASE_NOTE;

static uint32_t Song_GetCurrentNoteDuration(void);
static void Song_SetRemainingTime(uint32_t durationMs);
static void Song_ApplyCurrentNote(void);
static void Song_ChangeTempo(uint16_t newTempoBpm);


void Song_Init(void)
{
    playing = false;
    phaseExpired = false;
    tempoBpm = SONG_TEMPO_DEFAULT_BPM;
    currentSong = 0u;
    currentNote = 0u;
    phase = SONG_PHASE_NOTE;
    Song_SetRemainingTime(Song_GetCurrentNoteDuration());
    Piezo_Stop();
}


void Song_Tick1ms(void)
{
    if(playing && (remainingMs > 0u)){
        remainingMs--;

        if(remainingMs == 0u){
            phaseExpired = true;
        }
    }
}


void Song_Service(void)
{
    __disable_interrupt();
    if(!phaseExpired){
        __enable_interrupt();
        return;
    }
    phaseExpired = false;
    __enable_interrupt();

    if(phase == SONG_PHASE_NOTE){
        Piezo_Stop();

        if((currentNote + 1u) >= songs[currentSong].noteCount){
            currentSong++;
            if(currentSong >= SONG_COUNT){
                currentSong = 0u;
            }

            currentNote = 0u;
            phase = SONG_PHASE_INTER_SONG_GAP;
            Song_SetRemainingTime(SONG_INTER_SONG_GAP_MS);
        }
        else{
            phase = SONG_PHASE_NOTE_GAP;
            Song_SetRemainingTime(SONG_NOTE_GAP_MS);
        }
    }
    else if(phase == SONG_PHASE_NOTE_GAP){
        currentNote++;
        phase = SONG_PHASE_NOTE;
        Song_SetRemainingTime(Song_GetCurrentNoteDuration());
        Song_ApplyCurrentNote();
    }
    else{
        phase = SONG_PHASE_NOTE;
        Song_SetRemainingTime(Song_GetCurrentNoteDuration());
        Song_ApplyCurrentNote();
    }
}


void Song_TogglePlayback(void)
{
    playing = !playing;

    if(playing && (phase == SONG_PHASE_NOTE)){
        Song_ApplyCurrentNote();
    }
    else{
        Piezo_Stop();
    }
}


void Song_IncreaseTempo(void)
{
    if(tempoBpm <= (SONG_TEMPO_MAX_BPM - SONG_TEMPO_STEP_BPM)){
        Song_ChangeTempo(tempoBpm + SONG_TEMPO_STEP_BPM);
    }
    else{
        Song_ChangeTempo(SONG_TEMPO_MAX_BPM);
    }
}


void Song_DecreaseTempo(void)
{
    if(tempoBpm >= (SONG_TEMPO_MIN_BPM + SONG_TEMPO_STEP_BPM)){
        Song_ChangeTempo(tempoBpm - SONG_TEMPO_STEP_BPM);
    }
    else{
        Song_ChangeTempo(SONG_TEMPO_MIN_BPM);
    }
}


static uint32_t Song_GetCurrentNoteDuration(void)
{
    const SongNote *note = &songs[currentSong].notes[currentNote];

    /* One eighth note is 30,000 / BPM milliseconds. */
    return (30000u * note->durationInEighthNotes) / tempoBpm;
}


static void Song_SetRemainingTime(uint32_t durationMs)
{
    __disable_interrupt();
    remainingMs = durationMs;
    phaseExpired = (durationMs == 0u);
    __enable_interrupt();
}


static void Song_ApplyCurrentNote(void)
{
    uint16_t frequency = songs[currentSong].notes[currentNote].frequency;

    if(!playing || (phase != SONG_PHASE_NOTE) || (frequency == NOTE_REST)){
        Piezo_Stop();
        return;
    }

    piezoFrequency = frequency;
    Piezo_Start();
}


static void Song_ChangeTempo(uint16_t newTempoBpm)
{
    uint16_t oldTempoBpm = tempoBpm;

    if(newTempoBpm == oldTempoBpm){
        return;
    }

    tempoBpm = newTempoBpm;

    /* Scale the unfinished portion of the current note to the new tempo. */
    if(phase == SONG_PHASE_NOTE){
        __disable_interrupt();
        if((remainingMs > 0u) && !phaseExpired){
            remainingMs = (remainingMs * oldTempoBpm) / newTempoBpm;
            if(remainingMs == 0u){
                remainingMs = 1u;
            }
        }
        __enable_interrupt();
    }
}
