#include "sound.h"

#include <stdio.h>

#include <nds.h>

#include <libxm7.h>

#include "debug.h"
#include "io.h"

static Bytes backgroundSnd;
static XM7_ModuleManager_Type backgroundMod;

static Bytes soundEffect;

void initSound()
{
    debugf("sound: enabling\n");
    soundEnable();
}

void unloadSound()
{
    debugf("sound: disabling and freeing resources\n");
    soundDisable();
    stopBackgroundMusic();
}

void playSoundEffect(const char* filename)
{
    debugf("sound: playing effect %s\n", filename);
    soundEffect = slurp(filename);
    soundPlaySample(soundEffect.data, SoundFormat_16Bit, soundEffect.size, 48000, 127, 64, 0, 0);
    freeBytes(&soundEffect); // FIXME: I don't know why this free() doesn't break anything...
}

void playBackgroundMusic(const char* filename)
{
    backgroundSnd = slurp(filename);

    u16 ret = XM7_LoadXM(&backgroundMod, (XM7_XMModuleHeader_Type*)backgroundSnd.data);
    if (ret != 0) {
        debugf("sound: unable to load background %s\n", filename);
        return;
    }

    DC_FlushRange(backgroundSnd.data, backgroundSnd.size);

    debugf("sound: sending over FIFO %d\n", backgroundMod);
    fifoSendValue32(FIFO_XM7, (u32)&backgroundMod);

    debugf("sound: playing background %s\n", filename);
}

void stopBackgroundMusic()
{
    fifoSendValue32(FIFO_XM7, 0);
    freeBytes(&backgroundSnd);
}
