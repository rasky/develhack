#pragma once

#include <nds.h>

#define FIFO_XM7  (FIFO_USER_07)

typedef enum GlobalSfx {
    GlobalSfxMenu,
    GlobalSfxPunch,
    GlobalSfxSize, // This must always be the last one.
} GlobalSfx;

/*
 * Initializes the sound system and unmutes the master sound.
 */
void initSoundSystem();

/*
 * Frees up all resources used by the sound system and mutes the master sound.
 */
void destroySoundSystem();

/*
 * Allocates all resources needed to start playing the given XM file.
 */
void playBackgroundMusic(const GlobalSfx sfx);

/*
 * Stops playing background music (if any) and frees resources.
 */
void stopBackgroundMusic();

/*
 * Plays a sound effect.
 */
void playSoundEffect(const GlobalSfx effect);
