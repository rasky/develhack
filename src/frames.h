#pragma once

#include <nds/ndstypes.h>

#define ANIM_DESC_MAX_FRAMES 64

typedef struct {
	u8 idx;
	u8 next;
	s8 movex;
	s8 movey;
	const char *filename;
	u8 animidx : 4;
	u8 animsz : 4;
	u8 speed;
	u16 flags;
} AnimFrame;

typedef struct {
	AnimFrame frames[ANIM_DESC_MAX_FRAMES];
	struct {
		u8 idle;
		u8 forward;
		u8 backward;
	} keyframes;
	u8 pivotx, pivoty;

} AnimDesc;

// Animation Flags. Mark specific animation frames
// with their capabilities.
#define FCIDLE (1<<0)   // Can go back to idle (if input says so)
#define FCWALK (1<<1)   // Can begin walking (if input says so)

extern const AnimDesc Rasky;
