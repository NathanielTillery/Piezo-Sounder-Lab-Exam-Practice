/*
 * Song.h
 *
 *  Created on: Jul 16, 2026
 *      Author: natet
 */

#ifndef SONG_H_
#define SONG_H_

/* Initialize the song player in its paused state. */
void Song_Init(void);

/* Advance the song player's timing by one millisecond. */
void Song_Tick1ms(void);

/* Apply note and song transitions outside the SysTick interrupt. */
void Song_Service(void);

/* Playback and tempo controls used by main. */
void Song_TogglePlayback(void);
void Song_IncreaseTempo(void);
void Song_DecreaseTempo(void);

#endif /* SONG_H_ */
