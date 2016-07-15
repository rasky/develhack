#pragma once

#include <nds/ndstypes.h>
#include "hitbox.h"

#define TYPE_HAGOKEN   (0x1)

void specialInit(void);
void specialUpdate(void);
void specialVblank(void);
bool specialCollide(Hitbox *fighters, int *idx);

void specialCreate(u8 type, s32 x, s32 y, u8 enemy);
void specialSetScale(u32 scale);
