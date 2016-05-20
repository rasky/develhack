#include "sound.h"

#include <stdio.h>

#include <nds.h>

#include <libxm7.h>

#include "debug.h"

static void* backgroundSnd = NULL;
static XM7_ModuleManager_Type* backgroundMod = NULL;

void initSound()
{
    debugf("sound: enabling\n");
    fifoSendValue32(FIFO_SOUND, SOUND_MASTER_ENABLE);
}

void unloadSound()
{
    debugf("sound: disabling and freeing resources\n");
    fifoSendValue32(FIFO_SOUND, SOUND_MASTER_DISABLE);
    stopBackgroundMusic();
}

void playBackgroundMusic(const char* filename)
{
    // Load file to RAM
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        debugf("error: cannot load bg music: %s\n", filename);
        return;
    }

    fseek(fp, 0, SEEK_END);
    size_t fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    backgroundSnd = malloc(fsize);
    memset(backgroundSnd, 0, fsize);
    fread(backgroundSnd, 1, fsize, fp);

    fclose(fp);

    // Send module to ARM7 process
    backgroundMod = malloc(sizeof(XM7_ModuleManager_Type));
    memset(backgroundMod, 0, sizeof(XM7_ModuleManager_Type));

    u16 ret = XM7_LoadXM(backgroundMod, (XM7_XMModuleHeader_Type*)backgroundSnd);
    if (ret != 0) {
        debugf("sound: unable to load background %s\n", filename);
        return;
    }

    DC_FlushAll();

    debugf("sound: sending over FIFO %d\n", backgroundMod);
    fifoSendValue32(FIFO_XM7, (u32)backgroundMod);

    debugf("sound: playing background %s\n", filename);
}

void stopBackgroundMusic()
{
    fifoSendValue32(FIFO_XM7, 0);

    free(backgroundMod);
    free(backgroundSnd);

    backgroundMod = NULL;
    backgroundSnd = NULL;
}
