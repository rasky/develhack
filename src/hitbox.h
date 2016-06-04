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
    int red : 1;
    u8 h : 7;
} Hitbox;

/*
 * Returns true if the hitbox is an "attack" one (i.e.: it can deal damage to the other player).
 */
bool hitboxIsRed(const Hitbox* hitbox);

/*
 * Returns true if the hitbox is an "idle" one (i.e.: used only for collision detection).
 */
bool hitboxIsBlue(const Hitbox* hitbox);

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
bool hitboxesAreColliding(
    const Hitbox* src, fix23_8 srcX, fix23_8 srcY,
    const Hitbox* dst, fix23_8 dstX, fix23_8 dstY);

/*
 * Runs some hitbox unit tests at start.
 */
void testHitbox();
