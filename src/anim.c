#include <stdio.h>
#include <nds.h>
#include "debug.h"
#include "frames.h"

#define SPRITE_W 64
#define SPRITE_H 64
#define SPRITE_SIZE (SPRITE_W*SPRITE_H/2)
#define FRAME_SIZE (SPRITE_SIZE*4)
#define MAX_FRAMES 64

// Number of fractional bits for scale in AnimFighter
#define SCALE_BITS 10

typedef struct {
	int x,y;   // fixed point (.8)
	int scale; // fixed point (SCALE_BITS)
	int curframe;
	int cftime;
	u16 *vramptr[4];
	const AnimDesc *desc;
	u8 gfx[MAX_FRAMES * FRAME_SIZE];
	u16 pal[256];
} AnimFighter;

AnimFighter afight[2];

static void fighterInit(int fidx, const AnimDesc *desc) {
	const char *lastfn = NULL;

	AnimFighter *f = &afight[fidx];
	f->desc = desc;
	for (int i=0;i<ANIM_DESC_MAX_FRAMES;i++) {
		if (desc->frames[i].filename == NULL) {
			break;
		}
		if (desc->frames[i].idx != i) {  // consistency check
			debugf("error: invalid index in frame\n");
			return;
		}

		const AnimFrame *cur = &desc->frames[i];
		if (lastfn != cur->filename) {
			char fnbuf[128];
			strcpy(fnbuf, cur->filename);
			strcat(fnbuf, ".img.bin");

			FILE *fp = fopen(fnbuf, "rb");
			if (fp == NULL) {
				debugf("error: cannot load %s\n", fnbuf);
				return;
			}
			fread(&f->gfx[i * FRAME_SIZE], cur->animsz, FRAME_SIZE, fp);
			fclose(fp);

			lastfn = cur->filename;
		}
	}
}


void animInit(void) {
	fighterInit(0, &Rasky);

	oamInit(&oamMain, SpriteMapping_1D_64, false);
	oamDisable(&oamMain);

	for (int fx=0;fx<1;fx++) {
		AnimFighter *f = &afight[fx];
		for (int i=0;i<4;i++) {
			u16 *ptr = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
			f->vramptr[i] = ptr;
			oamSet(&oamMain, fx*4+i,
				/*pos*/ 0, 0,
				/*pri*/ 3,
				/*pal*/ 0,
				SpriteSize_64x64, SpriteColorFormat_16Color, ptr,
				/*affine*/ fx,0,
				/*hide*/ 0,
				/*flip*/ 0, 0,
				/*mosaic*/ 0);
		}
	}


	oamEnable(&oamMain);
	debugf("sprites ok!\n");

	// Load palette and copy to palette RAM
	FILE *f = fopen("rasky-idle.pal.bin", "rb");
	if (f == NULL) {
		debugf("error cannot load rasky-pal\n");
		return;
	}
	fread(afight[0].pal, 1, 256*2, f);
	fclose(f);

	for (int i=0;i<16;i++) {
		afight[0].pal[i]|=0x8000;
	}

	dmaCopyHalfWords(0, afight[0].pal, SPRITE_PALETTE, 256*2);

	afight[0].curframe = 8;
	afight[0].x = 100<<8;
	afight[0].y = 191<<8;
}

static void animUpdateOam(int fx) {
	AnimFighter *f = &afight[fx];
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
	oamRotateScale(&oamMain, fx, 0, invscale, invscale);
	oamSetXY(&oamMain, fx*4+0, x, y);
	oamSetXY(&oamMain, fx*4+1, x+scaledw, y);
	oamSetXY(&oamMain, fx*4+2, x, y+scaledh);
	oamSetXY(&oamMain, fx*4+3, x+scaledw, y+scaledh);
}


static int scale = 1 * (1<<SCALE_BITS);

void animUpdate(void) {
	for (int fx=0;fx<1;fx++) {
		AnimFighter *f = &afight[fx];
		const AnimDesc *fdesc = f->desc;

		f->scale = scale;
		scale -= 2;

		f->x += (int)fdesc->frames[f->curframe].movex * 32;
		f->y += (int)fdesc->frames[f->curframe].movey * 32;
		if (f->cftime > 0) {
			f->cftime--;
		} else {
			int ffirst = f->curframe - fdesc->frames[f->curframe].animidx;
			int animsz = fdesc->frames[f->curframe].animsz;
			int flast = ffirst + animsz;
			f->curframe++;
			if (f->curframe == flast) {
				f->curframe -= animsz;
			}
			f->cftime = fdesc->frames[f->curframe].speed;
		}

		animUpdateOam(fx);
	}
}

void animVblank(void) {
	oamUpdate(&oamMain);

	for (int fx=0;fx<1;fx++) {
		AnimFighter *f = &afight[fx];
		u8 *gfx = &f->gfx[f->curframe * FRAME_SIZE];

		for (int i=0;i<4;i++) {
			dmaCopyHalfWords(2,
				(u16*)(gfx + i*SPRITE_SIZE),
				(u16*)f->vramptr[i],
				SPRITE_SIZE);
		}
	}
}
