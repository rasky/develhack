#pragma once

#include <nds.h>

#define KEY_SPECIAL_HADUKEN BIT(31)
#define KEY_SPECIAL_SHORYUKEN BIT(30)
#define KEY_SPECIAL_HADUKEN_RTL BIT(29)
#define KEY_SPECIAL_SHORYUKEN_RTL BIT(28)

uint32_t currentKeyEvent();

void debugKeys();
