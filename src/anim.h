#pragma once

#include <nds/ndstypes.h>

void animInit(void);
void animUpdateStatus(u32 keys);
void animRedraw(void);
void animVblank(void);

/* Fighters public API */

// Number of fractional bits for scale factor in a fighter
#define SCALE_BITS 10

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
void animFighterGetState(int fx, int *status, int *movex, int *movey);
