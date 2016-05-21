#pragma once

#include <nds/ndstypes.h>

typedef struct {
	u32 w, h;   // size of the level graphics
	u32 floory; // position of the floor within the level
	u32 fighterStartX[2]; // initial position of fighters
} LevelDesc;


extern const LevelDesc LoungeRoom;
