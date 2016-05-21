#include <nds.h>
#include <string.h>
#include "levels.h"
#include "anim.h"
#include "debug.h"
#include "frames.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192

// Margin is the minimum distance that we allow a fighter to stay from the border
// When a fighter is closer than this to the border, we scroll the screen and/or
// zoom out.
#define MARGIN_MIN (50 << 8)

// This is the maximum distance that we allow a character to stay from the border
// before we begin zooming in.
#define MARGIN_MAX (90 << 8)

// This is the maximum zoom level. After reaching this level, we don't zoom anymore
#define ZOOM_MAX   (1<<SCALE_BITS)

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
	} fighters[2];

	struct {
		const LevelDesc *desc;
	} level;

	struct {
		// World coordinates of the rectangle that is shown on screen.
		// (fixed .8). y1 is not calculated, as it is implicit given the
		// scale factor defined by the x0-x1 range.
		s32 x0, x1;
		s32 y0;
		u32 zoom;
	} camera;
} gFight;


void fightInit(const LevelDesc *desc) {
	animInit();

	memset(&gFight, 0, sizeof(gFight));

	gFight.level.desc = desc;
	gFight.fighters[0].wx = (u32)desc->fighterStartX[0] << 8;
	gFight.fighters[1].wx = (u32)desc->fighterStartX[1] << 8;
	gFight.fighters[0].wy = desc->floory << 8;
	gFight.fighters[1].wy = desc->floory << 8;
	gFight.camera.x0 = 0<<8;
	gFight.camera.x1 = SCREEN_WIDTH<<8;
	gFight.camera.y0 = 0<<8;
	gFight.camera.zoom = 1<<SCALE_BITS;
}


static void fightUpdateStatus(int fx, u32 keys) {
	struct Fighter *f = &gFight.fighters[fx];

	int status, movex, movey;
	animFighterGetState(fx, &status, &movex, &movey);

	bool isleft = f->wx < gFight.fighters[1-fx].wx;

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
		// if (f->wx > gFight.level.desc->w<<8)
		// 	f->wx = gFight.level.desc->h<<8;
		// if (f->wx < 0)
		// 	f->wx = 0;
		break;

	case FST_JMP:
		f->wy += (s32)movey * 64;
		if (keys & KEY_LEFT) {
			f->wx -= (s32)movex * 64;
		} else if (keys & KEY_RIGHT) {
			f->wx += (s32)movex * 64;
		}
		break;
	}
}


// Calculate the "camera" position; this basically means calculating
// which portion (rectangle) of the level is currently viewable on
// the screen, which depends on the position of the fighters.
static void updateCamera() {

	u32 cameraw = gFight.camera.x1 - gFight.camera.x0;

	s32 xl = gFight.fighters[0].wx;
	s32 xr = gFight.fighters[1].wx;

	if (xl > xr) {
		s32 app = xl; xl = xr; xr = app;
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
		cameraw = gFight.camera.x1 - gFight.camera.x0;
		gFight.camera.zoom = div32(SCREEN_WIDTH<<(8+SCALE_BITS), cameraw);
	}
}

static void updateFighters() {
	u32 cameraw = gFight.camera.x1 - gFight.camera.x0;

	s32 xf0 = div32((gFight.fighters[0].wx - gFight.camera.x0)<<8, cameraw);
	s32 xf1 = div32((gFight.fighters[1].wx - gFight.camera.x0)<<8, cameraw);
	s32 yf0 = div32((gFight.fighters[0].wy - gFight.camera.y0)<<8, cameraw);
	s32 yf1 = div32((gFight.fighters[1].wy - gFight.camera.y0)<<8, cameraw);

	animFighterSetPosition(0, (SCREEN_WIDTH*xf0), (SCREEN_WIDTH*yf0));
	animFighterSetPosition(1, (SCREEN_WIDTH*xf1), (SCREEN_WIDTH*yf1));

	// debugf("new fpos: %d %d\n", (SCREEN_WIDTH*xf0)>>8, (SCREEN_WIDTH*xf1)>>8);

	animFighterSetScale(0, gFight.camera.zoom);
	animFighterSetScale(1, gFight.camera.zoom);

	animRedraw();
}

void fightUpdate(u32 keys) {
    animUpdateStatus(keys);
	fightUpdateStatus(0, keys);
	updateCamera();
	updateFighters();
}

void fightVblank() {
	animVblank();
}
