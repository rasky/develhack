#include "fight.h"

#include <string.h>

#include <nds.h>

#include "anim.h"
#include "debug.h"
#include "frames.h"
#include "stage.h"
#include "specials.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192

// Position of the floor on the screen
#define SCREEN_FLOOR_Y 180

// Margin is the minimum distance that we allow a fighter to stay from the border
// When a fighter is closer than this to the border, we scroll the screen and/or
// zoom out.
#define MARGIN_MIN (50 << 8)

// This is the maximum distance that we allow a character to stay from the border
// before we begin zooming in.
#define MARGIN_MAX (90 << 8)

// This is the maximum zoom level. After reaching this level, we don't zoom anymore
#define ZOOM_MAX (1 << SCALE_BITS)

/*
 * World coordinates:
 *
 *  * Y=0 is the floor on which fighters walk. Positive Y coordinates are jumps.
 *  * X is a positive number whose maximum depends on the size of the level.
 */

struct {
    struct Fighter {
        // World position of the fighter (fixed .8).
        // This is the point where the fighter's pivot will be placed.
        s32 wx, wy;

        // Life points of the player (0=dead)
        int lifepoints;

        // Accumulator to check whether was hit many points in short
        // window (and thus the player will fall to the ground)
        int fallpoints;
    } fighters[2];

    struct {
        const StageDesc* desc;
    } stage;

    struct {
        // World coordinates of the rectangle that is shown on screen.
        // (fixed .8). y1 is not calculated, as it is implicit given the
        // scale factor defined by the x0-x1 range.
        s32 x0, x1;
        s32 y0;
        u32 zoom;
    } camera;

    int collisionFlag;
} gFight;

static void fighterLoadHitboxes()
{
    debugf("Loading hitboxes\n");

    for (int i = 0; i < MAX_HITBOXES; i++) {
        const HitboxDesc* hitboxDesc = &Hitboxes[i];

        for (int j = 0; j < ANIM_DESC_MAX_FRAMES; j++) {
            AnimFrame* animFrame = (AnimFrame*)&Dummy.frames[j];

            if (strcmp(hitboxDesc->name, animFrame->filename) == 0) {
                memcpy(animFrame->boxes, hitboxDesc->hitbox[animFrame->animidx], HITBOX_TYPES_COUNT * sizeof(Hitbox));

#if 0
                debugf("%s - %d - %d %d\n", animFrame->filename, animFrame->animidx,
                    animFrame->boxes[0].x, animFrame->boxes[0].y);
#endif
            }
        }
    }
}

void fightInit(const StageDesc* desc)
{
    animInit();

    memset(&gFight, 0, sizeof(gFight));

    gFight.stage.desc = desc;
    gFight.fighters[0].wx = (u32)desc->fighterStartX[0] << 8;
    gFight.fighters[1].wx = (u32)desc->fighterStartX[1] << 8;
    gFight.fighters[0].wy = desc->floory << 8;
    gFight.fighters[1].wy = desc->floory << 8;
    gFight.fighters[0].lifepoints = 300;
    gFight.fighters[1].lifepoints = 300;
    gFight.camera.x0 = 0 << 8;
    gFight.camera.x1 = SCREEN_WIDTH << 8;
    gFight.camera.y0 = (desc->floory - SCREEN_FLOOR_Y) << 8;
    gFight.camera.zoom = 1 << SCALE_BITS;

    fighterLoadHitboxes();
}

static void fightUpdateStatus(int fx, u32 keys)
{
    struct Fighter* f = &gFight.fighters[fx];
    const StageDesc* desc = gFight.stage.desc;

    int status, movex, movey;
    animFighterGetState(fx, &status, &movex, &movey);

    bool isleft = f->wx < gFight.fighters[1 - fx].wx;

    switch (status) {
    case FST_IDL:
        // Idle: do nothing
        break;

    case FST_FWD:
    case FST_BWD:
        // Update the sprite position
        if (isleft)
            f->wx += (s32)movex * 64;
        else
            f->wx -= (s32)movex * 64;
        break;

    case FST_JMP:
        f->wy += (s32)movey * 64;
        if (keys & KEY_LEFT) {
            f->wx -= (s32)movex * 64;
        } else if (keys & KEY_RIGHT) {
            f->wx += (s32)movex * 64;
        }
        break;

    case FST_FALL:
    	if (isleft)
	        f->wx += (s32)movex * 64;
	    else
	        f->wx -= (s32)movex * 64;
    	break;
    }

    if (f->wx > desc->fighterLimits[1] << 8) {
        f->wx = desc->fighterLimits[1] << 8;
    } else if (f->wx < desc->fighterLimits[0] << 8) {
        f->wx = desc->fighterLimits[0] << 8;
    }

    if (f->fallpoints > 0)
    	f->fallpoints--;
}

// Calculate the "camera" position; this basically means calculating
// which portion (rectangle) of the stage is currently viewable on
// the screen, which depends on the position of the fighters.
static void updateCamera()
{

    u32 cameraw = gFight.camera.x1 - gFight.camera.x0;

    s32 xl = gFight.fighters[0].wx;
    s32 xr = gFight.fighters[1].wx;

    if (xl > xr) {
        s32 app = xl;
        xl = xr;
        xr = app;
    }

    s32 lmargin = xl - gFight.camera.x0;
    s32 rmargin = gFight.camera.x1 - xr;

    bool changed = false;

    if (lmargin < MARGIN_MIN) {
        if (rmargin < MARGIN_MAX) {
            // zoom out
            gFight.camera.x0 = xl - MARGIN_MIN;
        } else {
            // scroll left
            gFight.camera.x0 = xl - MARGIN_MIN;
            gFight.camera.x1 = gFight.camera.x0 + cameraw;
        }
        changed = true;
    }
    if (lmargin > MARGIN_MAX && gFight.camera.zoom < ZOOM_MAX) {
        // zoom in
        gFight.camera.x0 = xl - MARGIN_MAX;
        changed = true;
    }
    if (rmargin < MARGIN_MIN) {
        if (lmargin < MARGIN_MAX) {
            // zoom out
            gFight.camera.x1 = xr + MARGIN_MIN;
        } else {
            // scroll right
            gFight.camera.x1 = xr + MARGIN_MIN;
            gFight.camera.x0 = gFight.camera.x1 - cameraw;
        }
        changed = true;
    }
    if (rmargin > MARGIN_MAX && gFight.camera.zoom < ZOOM_MAX) {
        // zoom in
        gFight.camera.x1 = xr + MARGIN_MAX;
        changed = true;
    }

    if (changed) {
        if (gFight.camera.x0 < 0) {
            gFight.camera.x0 = 0;
        } else if (gFight.camera.x1 > gFight.stage.desc->w << 8) {
            gFight.camera.x1 = gFight.stage.desc->w << 8;
        }

        cameraw = gFight.camera.x1 - gFight.camera.x0;
        gFight.camera.zoom = div32(SCREEN_WIDTH << (8 + SCALE_BITS), cameraw);

        // FIXME: This keeps the floory line always at the same point. Not sure
        // if it's what we want, but for it's OK.
        gFight.camera.y0 = (gFight.stage.desc->floory << 8) - cameraw * SCREEN_FLOOR_Y / SCREEN_WIDTH;
    }
}

void fightFall(int fx)
{
   animFighterFall(fx);
   gFight.fighters[fx].fallpoints = 0;
}

// This function will be called when player #fx was hit
//   fx - fighter index
//   npoints - number of hitpoints inflicted to the player
void fightHit(int fx, int npoints)
{
    // Play hit animation (depending if in jump or not)
    animFighterHit(fx);

    // Remove points and calc if dead
    gFight.fighters[fx].lifepoints -= npoints;
    if (gFight.fighters[fx].lifepoints < 0) {
        gFight.fighters[fx].lifepoints = 0;
        // TODO: die
        fightFall(fx);
    } else {
		gFight.fighters[fx].fallpoints += npoints*10;
		if (gFight.fighters[fx].fallpoints >= 85) {
			// Hit many times in short timespan -> fall
	        fightFall(fx);
		}
    }

    debugf("OUCH: player %d: life %d (hit by %d, fall: %d)\n", fx, gFight.fighters[fx].lifepoints, npoints, gFight.fighters[fx].fallpoints);

    // TOOD: Play sound effect of hit
}

static void updateCollisions()
{
    u8 dmg0, dmg1;
    bool f0, f1;
    u8 pvx0, pvy0;
    u8 pvx1, pvy1;
    const Hitbox* b0 = animFighterGetHitboxes(0, &dmg0, &pvx0, &pvy0, &f0);
    const Hitbox* b1 = animFighterGetHitboxes(1, &dmg1, &pvx1, &pvy1, &f1);

    // Check if a red box collide with a blue box
    bool hit0 = hitboxCheckHit(
        b0, ANIM_DESC_MAX_BOXES, gFight.fighters[0].wx, gFight.fighters[0].wy, f0,
        b1, ANIM_DESC_MAX_BOXES, gFight.fighters[1].wx, gFight.fighters[1].wy, f1);
    bool hit1 = hitboxCheckHit(
        b1, ANIM_DESC_MAX_BOXES, gFight.fighters[1].wx, gFight.fighters[1].wy, f1,
        b0, ANIM_DESC_MAX_BOXES, gFight.fighters[0].wx, gFight.fighters[0].wy, f0);

    // Now handle hits
    // TODO: make sure we handle double-hits correctly
    if (hit0)
        fightHit(1, dmg0);
    if (hit1)
        fightHit(0, dmg1);

    Hitbox blues[2] = {
        hitboxGetBlue(b0, ANIM_DESC_MAX_BOXES, gFight.fighters[0].wx, gFight.fighters[0].wy, f0),
        hitboxGetBlue(b1, ANIM_DESC_MAX_BOXES, gFight.fighters[1].wx, gFight.fighters[1].wy, f1),
    };

    blues[0].x -= pvx0;
    blues[0].y -= pvy0;
    blues[1].x -= pvx1;
    blues[1].y -= pvy1;

    int fxhit;
    if (specialCollide(blues, &fxhit)) {
        // FIXME: parametrize special move damage
        fightHit(fxhit, 20);
        fightFall(fxhit);
    }
}

static void updateFighters()
{
    u32 cameraw = gFight.camera.x1 - gFight.camera.x0;

    s32 xf0 = div32((gFight.fighters[0].wx - gFight.camera.x0) << 8, cameraw);
    s32 xf1 = div32((gFight.fighters[1].wx - gFight.camera.x0) << 8, cameraw);
    s32 yf0 = div32((gFight.fighters[0].wy - gFight.camera.y0) << 8, cameraw);
    s32 yf1 = div32((gFight.fighters[1].wy - gFight.camera.y0) << 8, cameraw);

    animFighterSetPosition(0, (SCREEN_WIDTH * xf0), (SCREEN_WIDTH * yf0));
    animFighterSetPosition(1, (SCREEN_WIDTH * xf1), (SCREEN_WIDTH * yf1));

    // debugf("new fpos: %d %d\n", (SCREEN_WIDTH*xf0)>>8, (SCREEN_WIDTH*xf1)>>8);

    animFighterSetScale(0, gFight.camera.zoom);
    animFighterSetScale(1, gFight.camera.zoom);

    animRedraw();
}

static void updateStage()
{
    stageSetPosition(gFight.camera.x0, gFight.camera.x1, gFight.camera.y0);
    stageSetZoom(gFight.camera.zoom);
}

static void updateSpecial()
{
    specialSetScale(gFight.camera.zoom);
    specialUpdate();
}

void fightUpdate(u32 keys)
{
    animUpdateStatus(keys);
    fightUpdateStatus(0, keys);
    fightUpdateStatus(1, 0);
    updateCollisions();

    // Compute the new camera based on the position of the fighters
    // then update the sprites and the stage position/zoom to reflect
    // the new camera.
    updateCamera();
    updateFighters();
    updateStage();
    updateSpecial();
}

void fightVblank()
{
    animVblank();
}
