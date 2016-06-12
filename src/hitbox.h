#pragma once

#include <stdbool.h>

#include "fixmath.h"

/*
 * An hitbox. Coordinates are relative to the animation's frame and can be mapped to world
 * coordinates by knowing the fighter's position.
 */
typedef struct Hitbox {
    u8 x : 8;
    u8 y : 8;
    u8 w : 7;
    bool red : 1;
    u8 h : 7;
    bool white : 1;
} Hitbox;

#define WBOX(px,py,pw,ph) { .x=px, .y=py, .w=pw, .h=ph, .red=0, .white=1 }
#define BBOX(px,py,pw,ph) { .x=px, .y=py, .w=pw, .h=ph, .red=0, .white=0 }
#define RBOX(px,py,pw,ph) { .x=px, .y=py, .w=pw, .h=ph, .red=1, .white=0 }

/* Returns true if the hitbox is invalid (zero initialization is guaranteed to be invalid). */
bool hitboxInvalid(const Hitbox* hitbox);

/*
 * Returns true if the hitbox is an "attack" one (i.e.: it can deal damage to the other player).
 */
bool hitboxIsRed(const Hitbox* hitbox);

/*
 * Returns true if the hitbox is an "damage" one (i.e.: it can receive damage from the other player).
 */
bool hitboxIsBlue(const Hitbox* hitbox);

/*
 * Returns true if the hitbox is an "volume" one (i.e.: used only for collision detection).
 */
bool hitboxIsWhite(const Hitbox* hitbox);

/*
 * Returns true if the given source and destination boxes are colliding.
 *
 * Arguments:
 * - src: source hitbox;
 * - dst: destination hitbox;
 * - srcX: X coordinate of the source hitbox (in world coordinates);
 * - srcY: Y coordinate of the source hitbox (in world coordinates);
 * - dstX: X coordinate of the destination hitbox (in world coordinates);
 * - dstY: Y coordinate of the destination hitbox (in world coordinates);
 */
bool hitboxIntersects(
    const Hitbox* src, fix23_8 srcX, fix23_8 srcY,
    const Hitbox* dst, fix23_8 dstX, fix23_8 dstY);


/*
 * Given all the boxes of two players, check whether
 * any of the src red boxes intersects any of the dst
 * blue boxes.
 *
 * src_all/dst_all are two arrays of hitboxes, that can
 * contain invalid hitboxes (they are skipped).
 */
bool hitboxCheckHit(
	const Hitbox *src, int nsrc, fix23_8 srcX, fix23_8 srcY,
	const Hitbox *dst, int ndst, fix23_8 dstX, fix23_8 dstY);

/*
 * Runs some hitbox unit tests at start.
 */
void testHitbox();
