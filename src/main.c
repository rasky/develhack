#include <ctype.h>
#include <stdio.h>

#include <sys/stat.h>
#include <unistd.h>

#include <fat.h>
#include <nds.h>

#include <lauxlib.h>
#include <lua.h>

#include "debug.h"
#include "starField.h"

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
    vramSetPrimaryBanks(VRAM_A_MAIN_BG_0x06000000,
        VRAM_B_MAIN_BG_0x06020000,
        VRAM_C_SUB_BG_0x06200000,
        VRAM_D_LCD);

    /*  Set the video mode on the main screen. */
    videoSetMode(MODE_3_2D | // Set the graphics mode to Mode 3
        DISPLAY_BG2_ACTIVE | // Enable BG2 for display
        DISPLAY_BG3_ACTIVE); //Enable BG3 for display

    /*  Set the video mode on the sub screen. */
    videoSetModeSub(MODE_3_2D | // Set the graphics mode to Mode 3
        DISPLAY_BG3_ACTIVE); // Enable BG3 for display
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

void displayStarField()
{
    dmaCopyHalfWords(DMA_CHANNEL,
        starFieldBitmap, /* This variable is generated for us by  grit. */
        (uint16*)BG_BMP_RAM(0), /* Our address for main  background 3 */
        starFieldBitmapLen); /* This length (in bytes) is generated from grit. */
}

void Vblank()
{
    displayStarField();
    frame++;
}

int main(void)
{
    debugf("Powering on\n");
    touchPosition touchXY;

    powerOn(POWER_ALL_2D);

    initVideo();
    initBackgrounds();
    lcdMainOnBottom();

    irqSet(IRQ_VBLANK, Vblank);

    if (!fatInitDefault()) {
        debugf("Unable to initialize libfat");
        goto error;
    }

    FILE* hello = fopen("hello.txt", "rb");

    if (hello == NULL) {
        debugf("Unable to read hello.txt");
        goto error;
    }

    struct stat st;
    int status = fstat(fileno(hello), &st);
    char* greeting = (char*)malloc(st.st_size + 1);
    memset(greeting, 0, st.st_size + 1);
    int i = fread(greeting, 1, st.st_size, hello) - 1;
    while (isspace(greeting[i])) {
        greeting[i--] = 0;
    }
    fclose(hello);

    // ---- BEGIN LUA

    lua_State* lstate = luaL_newstate();
    if (lstate == NULL) {
        debugf("Unable to create Lua state");
        goto error;
    }

    int rc = luaL_dofile(lstate, "luac.out");
    if (rc != LUA_OK) {
        debugf(lua_tostring(lstate, -1));
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

    uint32 last_keys = 0;

    while (1) {
        swiWaitForVBlank();
        touchRead(&touchXY);

        if (touchXY.rawx || touchXY.rawy) {
            debugf("Touch: %04X, %04X / %04X, %04X\n",
                touchXY.rawx, touchXY.px,
                touchXY.rawy, touchXY.py);
        }

        uint32 keys = keysCurrent();
        uint32 changed = last_keys ^ keys;

        changed &= ~KEY_TOUCH;

        if (changed) {
            debugf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
                keys & KEY_A ? 'A' : '.',
                keys & KEY_B ? 'B' : '.',
                keys & KEY_SELECT ? 's' : '.',
                keys & KEY_START ? 'S' : '.',
                keys & KEY_RIGHT ? 'R' : '.',
                keys & KEY_LEFT ? 'L' : '.',
                keys & KEY_UP ? 'U' : '.',
                keys & KEY_DOWN ? 'D' : '.',
                keys & KEY_R ? 'R' : '.',
                keys & KEY_L ? 'L' : '.',
                keys & KEY_X ? 'X' : '.',
                keys & KEY_Y ? 'Y' : '.',
                keys & KEY_TOUCH ? 'T' : '.',
                keys & KEY_LID ? 'L' : '.');
        }

        last_keys = keys;
    }

error:
    while (1) {
        swiWaitForVBlank();
    }

    return 0;
}
