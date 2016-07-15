#pragma once

#include <nds/ndstypes.h>

#define TYPE_HAGOKEN   (0x1)

void specialInit(void);
void specialUpdate(void);
void specialVblank(void);

void specialCreate(u8 type, s32 x, s32 y);
void specialSetScale(u32 scale);
