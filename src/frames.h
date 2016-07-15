#pragma once

#include <nds/ndstypes.h>

#include "hitbox.h"

#define ANIM_DESC_MAX_FRAMES 64
#define ANIM_DESC_MAX_BOXES 3

typedef struct {
    u8 idx;
    u8 next;
    s8 movex;
    s8 movey;
    const char* filename;
    u8 animidx : 4;
    u8 animsz : 4;
    u8 speed;
    u16 flags;
    u8 damage;
    Hitbox boxes[ANIM_DESC_MAX_BOXES];
} AnimFrame;

typedef struct {
    AnimFrame frames[ANIM_DESC_MAX_FRAMES];
    struct {
        u8 idle;
        u8 forward;
        u8 backward;
        u8 jump;
        u8 punch;
        u8 kick;
        u8 hit;
        u8 special;
    } keyframes;
    u8 pivotx, pivoty;
    const char* palette;
    struct {
        u8 type;
        u8 offx, offy;
    } special;
} AnimDesc;

// Animation Flags. Mark specific animation frames
// with their capabilities.
#define FCIDLE (1 << 0) // Can go back to idle (if input says so)
#define FCFWD (1 << 1) // Can begin walking forward (if input says so)
#define FCBWD (1 << 2) // Can begin walking backward (if input says so)
#define FCWALK (FCFWD | FCBWD) // Can begin walking (if input says so)
#define FCJMP (1 << 3) // Can begin jumping (if input says so)

#define FSPECIAL (1 << 4) // Start the special effect

#define FSTATUS(f) ((f) & (7 << 8)) // Extract status
#define FST_NONE (0 << 8) // Status: none (all the others)
#define FST_IDL (1 << 8) // Status: idle
#define FST_FWD (2 << 8) // Status: moving forward
#define FST_BWD (3 << 8) // Status: moving backward
#define FST_JMP (4 << 8) // Status: jumping

extern const AnimDesc Rasky;
extern const AnimDesc Dummy;
