#include "input.h"

#include <nds.h>

#include "debug.h"

#define COUNTOF(x) (sizeof(x) / sizeof(x[0]))

#define BUFFER_SIZE 60
#define MAX_KEY_FRAMES 8

static u32 keyBuffer[BUFFER_SIZE];
static u8 current = 0;

static const u8 HADUKEN_LEFT_TO_RIGHT[] = { KEY_DOWN, KEY_DOWN | KEY_RIGHT, KEY_RIGHT, KEY_A };
static const u8 HADUKEN_RIGHT_TO_LEFT[] = { KEY_DOWN, KEY_DOWN | KEY_LEFT, KEY_LEFT, KEY_A };

static const u8 SHORYUKEN_LEFT_TO_RIGHT[] = { KEY_RIGHT, KEY_DOWN, KEY_DOWN | KEY_RIGHT, KEY_A };
static const u8 SHORYUKEN_RIGHT_TO_LEFT[] = { KEY_LEFT, KEY_DOWN, KEY_DOWN | KEY_LEFT, KEY_A };

#if 0
static void debugKeyBuffer()
{
    for (u8 i = 0; i < BUFFER_SIZE; i++) {
        debugf("%d: ", i);
        debugKeys(keyBuffer[i]);
    }
}
#endif

static bool detectSpecialMove(const u8* pattern)
{
    u8 scanned = 0;
    u8 i = current + 1;

    s8 hit = 0;
    s8 curKey = COUNTOF(pattern) - 1;
    s8 matchIdx = -1;

    while (scanned <= BUFFER_SIZE) {
        i--;
        if (i == UINT8_MAX) {
            i = BUFFER_SIZE - 1;
        }

        if (curKey < 0) {
            return true;
        } else if ((matchIdx != -1) && (keyBuffer[i] == keyBuffer[matchIdx])) {
            hit++;

            if (hit >= MAX_KEY_FRAMES) {
                return false;
            }
        } else if ((keyBuffer[i] & pattern[curKey]) == pattern[curKey]) {
            hit = 0;
            curKey--;
            matchIdx = i;
        } else {
            return false;
        }

        scanned++;
    }
    return false;
}

u32 currentKeyEvent()
{
    // Store current frame's keys
    u32 ret = keyBuffer[current] = keysCurrent();

    // Recognize special move
    if (detectSpecialMove(HADUKEN_LEFT_TO_RIGHT) || detectSpecialMove(HADUKEN_RIGHT_TO_LEFT)) {
        ret ^= KEY_SPECIAL_HADUKEN;
    } else if (detectSpecialMove(SHORYUKEN_LEFT_TO_RIGHT) || detectSpecialMove(SHORYUKEN_RIGHT_TO_LEFT)) {
        ret ^= KEY_SPECIAL_SHORYUKEN;
    }

    // Wrap around
    current++;

    if (current >= BUFFER_SIZE) {
        current = 0;
    }

    return ret;
}

void debugKeys(u32 keys)
{
    debugf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c %c%c\n",
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
        keys & KEY_LID ? 'L' : '.',
        keys & KEY_SPECIAL_HADUKEN ? 'H' : '.',
        keys & KEY_SPECIAL_SHORYUKEN ? 'S' : '.');
}
