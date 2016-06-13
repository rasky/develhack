#include <ctype.h>
#include <stdio.h>

#include <sys/stat.h>
#include <unistd.h>

#include <fat.h>
#include <nds.h>

#include <lauxlib.h>
#include <lua.h>

#include "anim.h"
#include "debug.h"
#include "fight.h"
#include "hitbox.h"
#include "input.h"
#include "sound.h"
#include "stage.h"

volatile int frame = 0;

void initVideo()
{
    /*
     *  Map VRAM to display a background on the main and sub screens.
     *
     *  The vramSetMainBanks function takes four arguments, one for each of the
     *  major VRAM banks. We can use it as shorthand for assigning values to
     *  each of the VRAM bank's control registers.
     *
     *  We map banks A and B to main screen background memory. This gives us
     *  256KB, which is a healthy amount for 16-bit graphics.
     *
     *  We map bank C to sub screen background memory.
     *
     *  We map bank D to LCD. This setting is generally used for when we aren't
     *  using a particular bank.
     */
    vramSetPrimaryBanks(VRAM_A_MAIN_BG,
        VRAM_B_MAIN_BG,
        VRAM_C_MAIN_BG,
        VRAM_D_MAIN_BG);

    /* Bank E is mapped for sprites */
    vramSetBankE(VRAM_E_MAIN_SPRITE);

    /*  Set the video mode on the main screen. */
    videoSetMode(MODE_5_2D);

    /*  Set the video mode on the sub screen. */
    videoSetModeSub(MODE_2_2D); // Set the graphics mode to Mode 3
}

void initBackgrounds()
{
    /*  Set up affine background 3 on main as a 16-bit color background. */
    REG_BG3CNT = BG_BMP16_256x256 | BG_BMP_BASE(0) | // The starting place in memory
        BG_PRIORITY(3); // A low priority

    /*  Set the affine transformation matrix for the main screen background 3
     *  to be the identity matrix.
     */
    REG_BG3PA = 1 << 8;
    REG_BG3PB = 0;
    REG_BG3PC = 0;
    REG_BG3PD = 1 << 8;

    /*  Place main screen background 3 at the origin (upper left of the
     *  screen).
     */
    REG_BG3X = 0;
    REG_BG3Y = 0;

    /*  Set up affine background 2 on main as a 16-bit color background. */
    REG_BG2CNT = BG_BMP16_128x128 | BG_BMP_BASE(8) | // The starting place in memory
        BG_PRIORITY(2); // A higher priority

    /*  Set the affine transformation matrix for the main screen background 3
     *  to be the identity matrix.
     */
    REG_BG2PA = 1 << 8;
    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG2PD = 1 << 8;

    /*  Place main screen background 2 in an interesting place. */
    REG_BG2X = -(SCREEN_WIDTH / 2 - 32) << 8;
    REG_BG2Y = -32 << 8;

    /*  Set up affine background 3 on the sub screen as a 16-bit color
     *  background.
     */
    REG_BG3CNT_SUB = BG_BMP16_256x256 | BG_BMP_BASE(0) | // The starting place in memory
        BG_PRIORITY(3); // A low priority

    /*  Set the affine transformation matrix for the sub screen background 3
     *  to be the identity matrix.
     */
    REG_BG3PA_SUB = 1 << 8;
    REG_BG3PB_SUB = 0;
    REG_BG3PC_SUB = 0;
    REG_BG3PD_SUB = 1 << 8;

    /* Place main screen background 3 at the origin (upper left of the screen) */
    REG_BG3X_SUB = 0;
    REG_BG3Y_SUB = 0;
}

/* Select a low priority DMA channel to perform our background copying. */
static const int DMA_CHANNEL = 3;

bool mustDumpStats()
{
    // FIXME: for now every once in a while; could be bound to a key
    return ((frame & 255) == 0);
}

void Vblank()
{
    fightVblank();
    stageVblank();
    frame++;
    if (mustDumpStats()) {
        u16 y = REG_VCOUNT;
        // vblank period: lines 192-262 (inclusive)
        u32 perc = ((u32)(y - 192) * 100) / (263 - 192);
        debugf("vblank usage: %d%%\n", perc);
    }
}

int main(void)
{
    debugf("powering on\n");
    touchPosition touchXY;

    powerOn(POWER_ALL_2D);
    setBrightness(3, -16); // set full black, both screens

    initVideo();
    lcdMainOnBottom();

    irqSet(IRQ_VBLANK, Vblank);

#ifdef WITH_UNIT_TESTS
    testHitbox();
#endif

#ifdef DEBUG_ON_SECONDARY_SCREEN
    consoleDemoInit();
#endif

    if (!fatInitDefault()) {
        debugf("Unable to initialize libfat\n");
        goto error;
    }

#if 0
    // ---- BEGIN LUA

    lua_State* lstate = luaL_newstate();
    if (lstate == NULL) {
        debugf("Unable to create Lua state\n");
        goto error;
    }

    int rc = luaL_dofile(lstate, "hello.luac");
    if (rc != LUA_OK) {
        debugf("%s\n", lua_tostring(lstate, -1));
        goto error;
    }

    lua_getglobal(lstate, "sum");
    lua_pushnumber(lstate, 1);
    lua_pushnumber(lstate, 2);
    if (lua_pcall(lstate, 2, 1, 0) != 0) {
        debugf(lua_tostring(lstate, -1));
        goto error;
    }

    int z = lua_tonumber(lstate, -1);
    lua_pop(lstate, 1);

    lua_close(lstate);
    debugf("Lua: %d\n", z);

    // ----- END LUA
#endif

    const StageDesc* stage = stageLoad("lounge");
    if (stage == NULL) {
        debugf("Unable to load stage\n");
        goto error;
    }

    fightInit(stage);

    // ----- BEGIN SOUND
    initSoundSystem();
    // playBackgroundMusic(GlobalSfxMenu);
    // ----- END SOUND

    uint32 lastKeys = 0;
    int brightness = -32;

    while (1) {
        swiWaitForVBlank();

        // initial fade-in
        if (brightness < 0) {
            brightness += 1;
            setBrightness(3, brightness / 2);
        }

        touchRead(&touchXY);

        if (touchXY.rawx || touchXY.rawy) {
            debugf("Touch: %04X, %04X / %04X, %04X\n",
                touchXY.rawx, touchXY.px,
                touchXY.rawy, touchXY.py);
        }

        uint32 keys = currentKeyEvent();
        uint32 changed = lastKeys ^ keys;

        changed &= ~KEY_TOUCH;

        if (changed) {
            debugKeys(keys);

            if ((keys & KEY_A) == KEY_A) {
                playSoundEffect(GlobalSfxPunch);
            }
        }

        fightUpdate(keys);
        stageUpdate();

        lastKeys = keys;

        if (mustDumpStats()) {
            u16 y = REG_VCOUNT;
            // frame period: all 263 lines, starting from 192
            // and wrapping around (inclusive). Part of it will be
            // used by vblank interrupt anyway.
            if (y >= 192) {
                y -= 192;
            } else {
                y += (263 - 192);
            }
            u32 perc = (u32)y * 100 / 263;
            debugf("CPU usage: %d%%\n", perc);
        }
    }

error:
    debugf("CRASHED\n");
    while (1) {
        swiWaitForVBlank();
    }

    return 0;
}
