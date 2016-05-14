#include "input.h"

#include <nds.h>

#include "debug.h"

#define COUNTOF(x) (sizeof(x) / sizeof(x[0]))

#define BUFFER_SIZE 60
#define MAX_KEY_FRAMES 4

static uint32_t keyBuffer[BUFFER_SIZE];
static uint8_t current = 0;

static const uint8_t HADUKEN[] = { KEY_DOWN, KEY_DOWN | KEY_RIGHT, KEY_RIGHT, KEY_A };

static void debugKeyBuffer()
{
    for (uint8_t i = 0; i < BUFFER_SIZE; i++) {
        debugf("%d: ", i);
        debugKeys(keyBuffer[i]);
    }
}

static bool detectSpecialMove(const uint8_t* pattern)
{
    uint8_t scanned = 0;
    uint8_t i = current + 1;

    int8_t hit = 0;
    int8_t curKey = COUNTOF(pattern) - 1;
    int8_t matchIdx = -1;

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
}

uint32_t currentKeyEvent()
{
    // Store current frame's keys
    uint32_t ret = keyBuffer[current] = keysCurrent();

    // Recognize special move
    if (detectSpecialMove(HADUKEN)) {
        ret ^= KEY_SPECIAL_HADUKEN;
    }

    // Wrap around
    current++;

    if (current >= BUFFER_SIZE) {
        current = 0;
    }

    return ret;
}

void debugKeys(uint32_t keys)
{
    debugf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c %c\n",
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
        keys & KEY_SPECIAL_HADUKEN ? 'H' : '.');
}
