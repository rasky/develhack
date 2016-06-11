#pragma once

#include <nds/ndstypes.h>

typedef struct
{
    const char* id; // stage identifier
    u32 w; // width of the level graphics
    u32 h; // height of the level graphics
    u32 floory; // position of the floor within the level
    u32 fighterStartX[2]; // initial position of fighters
} StageDesc;

// Functions related to loading and updating the stage, which generally
// controls the various background layers and may include some sprites
// as well.

const StageDesc* stageLoad(const char *id);

/* Set the stage coordinate to be shown at the top-left of the screen.
 * x/y are fixed point (.8).
 */
void stageSetPosition(s32 x, s32 y);

/* Set the zoom factor (fixed point using SCALE_BITS fractional bits)
 */
void stageSetZoom(u32 zoom);

/* Update the stage. Called once per frame in main loop.
 */
void stageUpdate();

/* Update the stage at vblank time. Called once per frame
 * during vblank.
 */
void stageVblank();
