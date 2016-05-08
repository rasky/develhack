#include "input.h"

#include <nds.h>

#include "debug.h"

#define COUNTOF(x) (sizeof(x) / sizeof(x[0]))

#define BUFFER_SIZE 60
#define MAX_KEY_FRAMES 4

static uint32_t keyBuffer[BUFFER_SIZE];
static uint8_t current = 0;

static const uint8_t HADUKEN[] = { KEY_DOWN, KEY_DOWN | KEY_RIGHT, KEY_RIGHT, KEY_A };

static bool detectSpecialMove(const uint8_t* pattern)
{
    uint8_t scanned = 0;
    uint8_t i = current + 1;

    int8_t hit = 0;
    int8_t curKey = COUNTOF(pattern) - 1;
    int8_t preKey = -1;

    while (scanned <= BUFFER_SIZE) {
        i--;
        if (i == UINT8_MAX) {
            i = BUFFER_SIZE - 1;
        }

        if (hit >= MAX_KEY_FRAMES) {
            return false;
        } else if (curKey < 0) {
            return true;
        } else if ((keyBuffer[i] & pattern[curKey]) == pattern[curKey]) {
            keyBuffer[i] = 0;
            hit = 0;
            preKey = curKey;
            curKey--;
        } else if (preKey != UINT8_MAX && ((keyBuffer[i] ^ pattern[preKey]) == pattern[preKey])) {
            keyBuffer[i] = 0;
            hit++;
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
