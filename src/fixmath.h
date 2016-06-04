#pragma once

#include <nds.h>

typedef s32 fix23_8; // 1.23.8

/*
 * Returns the integer part of the given fixed point number.
 */
s32 fix23_to_int(fix23_8 val);
