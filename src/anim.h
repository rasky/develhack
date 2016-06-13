#pragma once

#include <nds/ndstypes.h>

struct Hitbox;
typedef struct Hitbox Hitbox;

void animInit(void);
void animUpdateStatus(u32 keys);
void animRedraw(void);
void animVblank(void);

// Number of fractional bits for scale factor in a fighter
#define SCALE_BITS 10

/* Anim fighters public API */

/* Set the position of the fighter on the screen.
 *    fx: number of fighter (0 or 1)
 *    x,y: fixed point coordinate (.8)
 */
void animFighterSetPosition(int fx, u32 x, u32 y);

/* Set the scale factor of the fighter on the screen.
 *    fx: number of fighter (0 or 1)
 *    scale: fixed point (SCALE_BITS)
 *
 * scale is a multiplicative factor where 1.0 is
 * standard size (128x128).
 */
void animFighterSetScale(int fx, u32 scale);

/* Read the current fighter state (moving, jumping, etc.)
 * The value returned can be compared against the FST_* macros
 * defined in frames.h.
 */
void animFighterGetState(int fx, int* status, int* movex, int* movey);

/* Get the pointer to the hitboxes and damage info for the current frame of the
 * specified fighter. The hitboxes are stored in an array whose size
 * is ANIM_DESC_MAX_BOXES.
 */
const Hitbox* animFighterGetHitboxes(int fx, u8* damage);

void animFighterHit(int fx);
