#pragma once

#include <nds.h>

#define KEY_SPECIAL_HADUKEN BIT(31)
#define KEY_SPECIAL_SHORYUKEN BIT(30)

uint32_t currentKeyEvent();

void debugKeys();
