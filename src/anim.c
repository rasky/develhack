#include <stdio.h>
#include <nds.h>
#include "debug.h"
#include "frames.h"

#define SPRITE_SIZE (64*64/2)
#define FRAME_SIZE (SPRITE_SIZE*4)
#define MAX_FRAMES 64

typedef struct {
	int x,y;
	int scale; // fixed point .8
	int curframe; // fixed point .8
	int cftime;
	u16 *vramptr[4];
	const AnimFrame *frames;
	u8 gfx[MAX_FRAMES * FRAME_SIZE];
	u16 pal[256];
} AnimFighter;

AnimFighter afight[2];

static void fighterInit(int fidx, const AnimFrame *frames, int numframes) {
	char *lastfn = NULL;

	AnimFighter *f = &afight[fidx];
	f->frames = frames;
	for (int i=0;i<numframes;i++) {
		if (frames[i].idx != i) {  // consistency check
			debugf("error: invalid index in frame\n");
			return;
		}

		const AnimFrame *cur = &frames[i];
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
	fighterInit(0, Rasky, RaskyCount);

	oamInit(&oamMain, SpriteMapping_1D_64, false);
	oamDisable(&oamMain);

	for (int fx=0;fx<1;fx++) {
		AnimFighter *f = &afight[fx];
		for (int i=0;i<4;i++) {
			u16 *ptr = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
			f->vramptr[i] = ptr;
			oamSet(&oamMain, fx*4+i,
				/*pos*/ 100, 100,
				/*pri*/ 0,
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
	dmaCopyHalfWords(0, afight[0].pal, SPRITE_PALETTE, 256*2);

	afight[0].curframe = 0;
}

void animUpdate(void) {
	for (int fx=0;fx<1;fx++) {
		AnimFighter *f = &afight[fx];

		f->scale = 1.5 * (1<<8); // scale 0.5

		f->x += (int)f->frames[f->curframe].movex * 32;
		f->y += (int)f->frames[f->curframe].movey * 32;
		if (f->cftime > 0) {
			f->cftime--;
		} else {
			int ffirst = f->curframe - f->frames[f->curframe].animidx;
			int animsz = f->frames[f->curframe].animsz;
			int flast = ffirst + animsz;
			f->curframe++;
			if (f->curframe == flast) {
				f->curframe -= animsz;
			}
			f->cftime = f->frames[f->curframe].speed;
		}

		int offset = div32(64<<8,  f->scale);

		int x = f->x >> 8;
		int y = f->y >> 8;
		oamRotateScale(&oamMain, fx, 0, f->scale, f->scale);
		oamSetXY(&oamMain, fx*4+0, x, y);
		oamSetXY(&oamMain, fx*4+1, x+offset, y);
		oamSetXY(&oamMain, fx*4+2, x, y+offset);
		oamSetXY(&oamMain, fx*4+3, x+offset, y+offset);
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
