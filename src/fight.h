#pragma once

#include "levels.h"

void fightInit(const LevelDesc *desc);
void fightUpdate(u32 keys);
void fightVblank();
