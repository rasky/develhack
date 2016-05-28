#include "sound.h"

#include <stdio.h>

#include <nds.h>

#include <libxm7.h>

#include "debug.h"
#include "io.h"

/*
 * Sound System Setup/Teardown
 */

static Bytes GLOBAL_SFX[GlobalSfxSize];

static void loadGlobalSfx()
{
    GLOBAL_SFX[GlobalSfxMenu] = slurp("hyo-fate.xm");
    GLOBAL_SFX[GlobalSfxPunch] = slurp("punch.ima");
}

void initSoundSystem()
{
    debugf("sound: enabling\n");
    loadGlobalSfx();
    soundEnable();
}

static void freeGlobalSfx()
{
    for (u8 i = 0; i < GlobalSfxSize; i++) {
        freeBytes(&GLOBAL_SFX[i]);
    }
}

void destroySoundSystem()
{
    debugf("sound: disabling and freeing resources\n");
    soundDisable();
    stopBackgroundMusic();
    freeGlobalSfx();
}

/*
 * Effect API
 */

void playSoundEffect(const GlobalSfx sfx)
{
    const Bytes* effect = &GLOBAL_SFX[sfx];

    soundPlaySample(effect->data, SoundFormat_ADPCM, effect->size, 48000, 127, 64, 0, 0);
}

/*
 * Background Music API
 */

static XM7_ModuleManager_Type backgroundMod;

void playBackgroundMusic(const GlobalSfx sfx)
{
    const Bytes* backgroundSnd = &GLOBAL_SFX[sfx];

    u16 ret = XM7_LoadXM(&backgroundMod, (XM7_XMModuleHeader_Type*)GLOBAL_SFX[sfx].data);
    if (ret != 0) {
        debugf("sound: unable to load background %d\n", sfx);
        return;
    }

    DC_FlushRange(backgroundSnd->data, backgroundSnd->size);
    fifoSendValue32(FIFO_XM7, (u32)&backgroundMod);
}

void stopBackgroundMusic()
{
    fifoSendValue32(FIFO_XM7, 0);
}
