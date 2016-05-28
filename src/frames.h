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
		u8 jump;
	} keyframes;
	u8 pivotx, pivoty;
	const char *palette;
} AnimDesc;

// Animation Flags. Mark specific animation frames
// with their capabilities.
#define FCIDLE (1<<0)   // Can go back to idle (if input says so)
#define FCFWD  (1<<1)   // Can begin walking forward (if input says so)
#define FCBWD  (1<<2)   // Can begin walking backward (if input says so)
#define FCWALK (FCFWD|FCBWD)   // Can begin walking (if input says so)
#define FCJMP  (1<<3)   // Can begin jumping (if input says so)

#define FSTATUS(f) ((f)&(3<<8))  // Extract status
#define FST_IDL  (0<<8)  // Status: idle
#define FST_FWD  (1<<8)  // Status: moving forward
#define FST_BWD  (2<<8)  // Status: moving backward
#define FST_JMP  (3<<8)  // Status: jumping

extern const AnimDesc Rasky;
extern const AnimDesc Dummy;
