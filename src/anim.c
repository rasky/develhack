#include <stdio.h>
#include <nds.h>
#include "debug.h"
#include "frames.h"
#include "input.h"
#include "anim.h"

#define SPRITE_W 64
#define SPRITE_H 64
#define SPRITE_SIZE (SPRITE_W*SPRITE_H/2)
#define FRAME_SIZE (SPRITE_SIZE*4)
#define MAX_GFX_FRAMES 64

// Misc flags for AnimFighter
#define FLAGS_IS_RIGHT (1<<0)

typedef struct {
	// Screen position of the fighter (fixed point .8).
	// This is the point where the fighter's pivot will be placed.
	u32 x,y;

	// Scale factor (fixed point, with SCALE_BITS fractional bits)
	int scale;

	// Current frame being drawn
	u8 curframe;

	// Countdown timer for current frame, before switching to next one
	u8 cftime;

	// Misc flags for this fighter (FLAGS_*)
	u8 flags;

	// Pointers to the 4 VRAM sprites that compose this fighter
	// (each fighter is 128x128, composed by 4 64x64 hardware sprites)
	u16 *vramptr[4];

	// Pointer to the animation description of this fighter (see frames.c)
	const AnimDesc *desc;

	// Full graphics for this fighter (all the frames). This is loaded by
	// animLoad(). Notice that this cannot be indexed directly by curframe
	// because frames (as described in AnimDesc / frames.c) can reuse the same
	// graphics multiple times. gfxindex must be used to convert the frame index
	// into an index for this gfx table.
	u8 gfx[MAX_GFX_FRAMES * FRAME_SIZE];

	// Conversion map between the frame index (as stored by curframe and desc),
	// and the gfx table.
	u8 gfxindex[ANIM_DESC_MAX_FRAMES];

	// Palette for this fighter.
	u16 pal[256];
} AnimFighter;

AnimFighter gFighters[2];

static void animLoad(int fidx, const AnimDesc *desc) {
	const int MAX_ANIMS = 16;

	int lastgfxindex = 0;
	struct  {
		const char *filename;
		int gfxindex;
	} loadedAnims[MAX_ANIMS];
	memset(loadedAnims, 0, sizeof(loadedAnims));

	AnimFighter *f = &gFighters[fidx];
	f->desc = desc;
	for (int i=0;i<ANIM_DESC_MAX_FRAMES;i++) {
		const AnimFrame *cur = &desc->frames[i];

		if (cur->filename == NULL) {
			break;
		}
		if (cur->idx != i) {  // consistency check
			debugf("error: invalid index in frame\n");
			return;
		}

		// Find the animation (if already loaded)
		int a = 0;
		for (;a<MAX_ANIMS;a++) {
			if (loadedAnims[a].filename == NULL ||
				loadedAnims[a].filename == cur->filename) {
				break;
			}
		}
		// ... or load the new animation
		if (loadedAnims[a].filename != cur->filename) {
			char fnbuf[128];
			strcpy(fnbuf, cur->filename);
			strcat(fnbuf, ".img.bin");

			debugf("loading %s\n", fnbuf);
			FILE *fp = fopen(fnbuf, "rb");
			if (fp == NULL) {
				debugf("error: cannot load %s\n", fnbuf);
				return;
			}
			fread(&f->gfx[lastgfxindex * FRAME_SIZE], cur->animsz, FRAME_SIZE, fp);
			fclose(fp);

			loadedAnims[a].filename = cur->filename;
			loadedAnims[a].gfxindex = lastgfxindex;
			lastgfxindex += cur->animsz;
		}

		// Compute the global gfx index for this frame, by adding
		// the index within the animation to the animation base index.
		f->gfxindex[i] = loadedAnims[a].gfxindex + cur->animidx;
	}

	// Flush the data cache for graphics, so that it can be
	// immediately blitted to VRAM.
	DC_FlushRange(f->gfx, sizeof(f->gfx));
}


void animInit(void) {
	animLoad(0, &Rasky);
	animLoad(1, &Rasky);

	oamInit(&oamMain, SpriteMapping_1D_64, false);
	oamDisable(&oamMain);

	for (int fx=0;fx<2;fx++) {
		AnimFighter *f = &gFighters[fx];
		for (int i=0;i<4;i++) {
			u16 *ptr = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
			f->vramptr[i] = ptr;
			oamSet(&oamMain, fx*4+i,
				/*pos*/ 0, 0,
				/*pri*/ 3,
				/*pal*/ 0,
				SpriteSize_64x64, SpriteColorFormat_16Color, ptr,
				/*affine*/ fx, 0,
				/*hide*/ 0,
				/*flip*/ 0, 0,
				/*mosaic*/ 0);
		}

		// Start with idle animation
		f->scale = 1<<SCALE_BITS;
		f->curframe = f->desc->keyframes.idle;
	}


	oamEnable(&oamMain);
	debugf("sprites ok!\n");

	// Load palette and copy to palette RAM
	FILE *f = fopen("rasky-idle.pal.bin", "rb");
	if (f == NULL) {
		debugf("error cannot load rasky-pal\n");
		return;
	}
	fread(gFighters[0].pal, 1, 256*2, f);
	fclose(f);

	dmaCopyHalfWords(0, gFighters[0].pal, SPRITE_PALETTE, 256*2);

	// Sane defaults for scaling (position can be 0,0, it doesn't matter,
	// but scale=0 means nothing is displayed by default).
	gFighters[0].scale = 1 * (1<<SCALE_BITS);
	gFighters[1].scale = 1 * (1<<SCALE_BITS);
}

static void animUpdateOam(int fx) {
	AnimFighter *f = &gFighters[fx];
	const AnimDesc *fdesc = f->desc;

	// Compute coordinates so that the scale factor is computed using the pivot as
	// center of scaling. This means that the sprite's pivot will be positioned at
	// the specified coordinate (f->x, f->y), and then the sprite will be scaled.
	int scaledw = (SPRITE_W * f->scale)>>SCALE_BITS;
	int scaledh = (SPRITE_H * f->scale)>>SCALE_BITS;
	int x = (f->x>>8) - ((fdesc->pivotx * f->scale)>>SCALE_BITS) - (SPRITE_W-scaledw)/2;
	int y = (f->y>>8) - ((fdesc->pivoty * f->scale)>>SCALE_BITS) - (SPRITE_H-scaledh)/2;

	// Calculate inverse of scale in .8 fractional format (which is what
	// oamRotateScale() expects).
	int invscale = div32(1<<(8+SCALE_BITS), f->scale);
	if (!(f->flags & FLAGS_IS_RIGHT)) {
		oamRotateScale(&oamMain, fx, 0, invscale, invscale);
		oamSetXY(&oamMain, fx*4+0, x, y);
		oamSetXY(&oamMain, fx*4+1, x+scaledw, y);
		oamSetXY(&oamMain, fx*4+2, x, y+scaledh);
		oamSetXY(&oamMain, fx*4+3, x+scaledw, y+scaledh);
	} else {
		oamRotateScale(&oamMain, fx, 0, -invscale, invscale);
		oamSetXY(&oamMain, fx*4+0, x+scaledw, y);
		oamSetXY(&oamMain, fx*4+1, x, y);
		oamSetXY(&oamMain, fx*4+2, x+scaledw, y+scaledh);
		oamSetXY(&oamMain, fx*4+3, x, y+scaledh);
	}
}

#define INPUT_DIR_MASK (KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT)

static void animProcessInput(int fx, u32 input) {
	AnimFighter *f = &gFighters[fx];
	const AnimDesc *fdesc = f->desc;
	u16 fflags = fdesc->frames[f->curframe].flags;

	u32 dirinput = input & INPUT_DIR_MASK;
	switch (dirinput) {
	case 0:
		if (fflags & FCIDLE)
			f->curframe = fdesc->keyframes.idle;
		break;

	case KEY_RIGHT:
		if (!(f->flags & FLAGS_IS_RIGHT)) {
			if (fflags & FCFWD)
				f->curframe = fdesc->keyframes.forward;
		} else {
			if (fflags & FCBWD)
				f->curframe = fdesc->keyframes.backward;
		}
		break;

	case KEY_LEFT:
		if (!(f->flags & FLAGS_IS_RIGHT)) {
			if (fflags & FCBWD)
				f->curframe = fdesc->keyframes.backward;
		} else {
			if (fflags & FCFWD)
				f->curframe = fdesc->keyframes.forward;
		}
		break;
	}
}


void animUpdateStatus(u32 input) {
	// Update left<->right flags
	if (gFighters[0].x < gFighters[1].x) {
		gFighters[0].flags &= ~FLAGS_IS_RIGHT;
		gFighters[1].flags |= FLAGS_IS_RIGHT;
	} else {
		gFighters[0].flags |= FLAGS_IS_RIGHT;
		gFighters[1].flags &= ~FLAGS_IS_RIGHT;
	}

	// Fighter #0 is our own character; process the input
	animProcessInput(0, input);
}

void animRedraw() {
	for (int fx=0;fx<2;fx++) {
		AnimFighter *f = &gFighters[fx];
		const AnimDesc *fdesc = f->desc;
		const AnimFrame *curframe = &fdesc->frames[f->curframe];

		// See if it's time to display next frame.
		if (f->cftime > 0) {
			f->cftime--;
		} else {
			// Check which frame must be displayed and
			// set the new countdown.
			if (curframe->next == 0) {
				f->curframe++;
			} else {
				f->curframe = curframe->next-1;
			}
			f->cftime = curframe->speed;
		}

		animUpdateOam(fx);
	}
}

void animVblank(void) {
	oamUpdate(&oamMain);

	for (int fx=0;fx<2;fx++) {
		AnimFighter *f = &gFighters[fx];
		int gfxidx = f->gfxindex[f->curframe];
		u8 *gfx = &f->gfx[gfxidx * FRAME_SIZE];

		for (int i=0;i<4;i++) {
			dmaCopyHalfWords(2,
				(u16*)(gfx + i*SPRITE_SIZE),
				(u16*)f->vramptr[i],
				SPRITE_SIZE);
		}
	}
}


void animFighterSetPosition(int fx, u32 x, u32 y) {
	gFighters[fx].x = x;
	gFighters[fx].y = y;
}

void animFighterSetScale(int fx, u32 scale) {
	gFighters[fx].scale = scale;
}

void animFighterGetState(int fx, int *status, int *movex, int *movey) {
	AnimFighter *f = &gFighters[fx];
	const AnimDesc *fdesc = f->desc;
	const AnimFrame *fframe = &fdesc->frames[f->curframe];

	if (status) *status = FSTATUS(fframe->flags);
	if (movex)  *movex = fframe->movex;
	if (movey)  *movey = fframe->movey;
}
