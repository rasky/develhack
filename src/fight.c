#include <nds.h>
#include <string.h>
#include "levels.h"
#include "anim.h"
#include "debug.h"
#include "frames.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192

#define FLOAT_TO_FIXED8(x) ((x) * (1<<8))
#define MARGIN_LEFT_MIN  FLOAT_TO_FIXED8(0.1)
#define MARGIN_LEFT_MAX  FLOAT_TO_FIXED8(0.3)
#define MARGIN_RIGHT_MIN FLOAT_TO_FIXED8((1-0.1))
#define MARGIN_RIGHT_MAX FLOAT_TO_FIXED8((1-0.3))

#define MAX_DIST  FLOAT_TO_FIXED8(0.5)

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
		// (fixed .8)
		s32 x0, y0;
		s32 x1, y1;
		u32 zoom;
	} camera;
} gFight;


void fightInit(const LevelDesc *desc) {
	animInit();

	memset(&gFight, 0, sizeof(gFight));

	gFight.level.desc = desc;
	gFight.fighters[0].wx = (u32)desc->fighterStartX[0] << 8;
	gFight.fighters[1].wx = (u32)desc->fighterStartX[1] << 8;
	gFight.camera.x0 = 0<<8;
	gFight.camera.x1 = SCREEN_WIDTH<<8;
	gFight.camera.y0 = 0<<8;
	gFight.camera.y1 = SCREEN_HEIGHT<<8;
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
	}
}


// Calculate the "camera" position; this basically means calculating
// which portion (rectangle) of the level is currently viewable on
// the screen, which depends on the position of the fighters.
static void updateCamera() {

	u32 cameraw = gFight.camera.x1 - gFight.camera.x0;
	int leftfx = 0;

	// Compute current relative position on screen ([0,1] range)
	s32 xf0 = div32((gFight.fighters[0].wx - gFight.camera.x0)<<8, cameraw);
	s32 xf1 = div32((gFight.fighters[1].wx - gFight.camera.x0)<<8, cameraw);
	debugf("init: f0.wx=%d, f0.wy=%d, cx0=%d, cx1=%d, camera.w=%d\n",
		gFight.fighters[0].wx>>8, gFight.fighters[1].wx>>8,
		gFight.camera.x0>>8, gFight.camera.x1>>8, cameraw>>8);

	if (xf0 > xf1) {
		u32 app = xf0; xf0 = xf1; xf1 = app;
		leftfx = 1;
	}

	s32 dist = xf1-xf0;
	debugf("dist: %ld (MAX=%d, xf0=%d, xf1=%d)\n", dist,(u32)MAX_DIST,xf0,xf1);
	if (dist >= MAX_DIST) {
		u32 newcamw = div32(xf1*cameraw - xf0*cameraw, MAX_DIST);
		u32 delta = (newcamw-cameraw)/2;
		gFight.camera.x0 -= delta/2;
		// gFight.camera.x1 += delta/2;
		gFight.camera.zoom = div32(SCREEN_WIDTH<<(8+SCALE_BITS), newcamw);
		debugf("new camera: w=%d, (%d,%d), zoom: %d\n",
			newcamw, gFight.camera.x0>>8, gFight.camera.x1>>8, gFight.camera.zoom);

		cameraw = gFight.camera.x1 - gFight.camera.x0;
	}


	xf0 = div32((gFight.fighters[0].wx - gFight.camera.x0)<<8, cameraw);
	xf1 = div32((gFight.fighters[1].wx - gFight.camera.x0)<<8, cameraw);
	animFighterSetPosition(leftfx, (SCREEN_WIDTH*xf0), 130<<8);
	animFighterSetPosition(1-leftfx, (SCREEN_WIDTH*xf1), 130<<8);

	debugf("new fpos: %d %d\n", (SCREEN_WIDTH*xf0)>>8, (SCREEN_WIDTH*xf1)>>8);

	animFighterSetScale(0, gFight.camera.zoom);
	animFighterSetScale(1, gFight.camera.zoom);
}

static void updateFighters() {
	u32 cameraw = gFight.camera.x1 - gFight.camera.x0;

	s32 xf0 = div32((gFight.fighters[0].wx - gFight.camera.x0)<<8, cameraw);
	s32 xf1 = div32((gFight.fighters[1].wx - gFight.camera.x0)<<8, cameraw);
	animFighterSetPosition(0, (SCREEN_WIDTH*xf0), 130<<8);
	animFighterSetPosition(1, (SCREEN_WIDTH*xf1), 130<<8);

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
