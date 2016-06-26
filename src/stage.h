#pragma once

#include <nds/ndstypes.h>

typedef struct
{
    char id[32]; // stage identifier
    u32 w; // width of the level graphics
    u32 h; // height of the level graphics
    u32 floory; // position of the floor within the level
    u32 fighterStartX[2]; // initial position of fighters
    u32 fighterLimits[2]; // minimum/maximum X coordinates the figthers can be at
} StageDesc;

// Functions related to loading and updating the stage, which generally
// controls the various background layers and may include some sprites
// as well.

// Initialize global stage-related state
void stageInit();

const StageDesc* stageLoad(const char* id);

/* Set the stage viewport
 * left/right/top are fixed point (.8).
 */
void stageSetPosition(s32 left, s32 right, s32 top);

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
