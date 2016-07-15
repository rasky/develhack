#include "specials.h"
#include "mmap.h"
#include "debug.h"
#include "pool.h"
#include "anim.h"

#include <nds.h>
#include <stdio.h>

typedef struct {
	u8 type;
	u8 *gfx;
    u8 *pal;
    u16 *vramptr;
} SpecialDesc;

typedef struct {
    SpecialDesc *desc;
    s32 x,y;
    u16 frame;
    u8 flags;
} Special;

#define SPRITE_W 64
#define SPRITE_H 64
#define FLAG_ACTIVE    (0x80)
#define MAX_SPECIALS   16

static SpecialDesc gdesc[2];
static Special specials[MAX_SPECIALS]; // instances
static u32 gscale; // scale factor

#define SPECIAL_IDX(s) ((s) - &specials[0])

void destroy(Special *s);


// **************** HAGOKEN

void loadGfx(char *name, u8 **gfx, u8 **pal) {
    char fnbuf[128];

    strcpy(fnbuf, name);
    strcat(fnbuf, ".img.bin");
    debugf("loading %s\n", fnbuf);
    *gfx = poolSlurp(POOL_FIGHT_SPECIAL, fnbuf);

    strcpy(fnbuf, name);
    strcat(fnbuf, ".pal.bin");
    debugf("loading %s\n", fnbuf);
    *pal = poolSlurp(POOL_FIGHT_SPECIAL, fnbuf);
}

void hagokenLoad(SpecialDesc *s) {
    s->type = TYPE_HAGOKEN;
    loadGfx("hagoken", &s->gfx, &s->pal);
    s->vramptr = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);

    u8 tmp;
    for (int i=0;i<64*64*3/2;i++){
        u8 px = s->gfx[i];
        u8 px0 = px&0xF;
        u8 px1 = (px>>4)&0xF;
        if (px0==0xF) px0=0; else if (px0==0) px0=0xF;
        if (px1==0xF) px1=0; else if (px1==0) px1=0xF;
        s->gfx[i] = px0|(px1<<4);
    }
    tmp=s->pal[0]; s->pal[0]=s->pal[30]; s->pal[30]=tmp;
    tmp=s->pal[1]; s->pal[1]=s->pal[31]; s->pal[31]=tmp;

    DC_FlushRange(s->gfx, 64*64/2);
    DC_FlushRange(s->pal, 16*2);
    dmaCopyHalfWords(2, s->gfx, s->vramptr, 64*64/2);
    dmaCopyHalfWords(2, s->pal, SPRITE_PALETTE + MMAP_PAL_SPECIALS(TYPE_HAGOKEN-1)*16, 16*2);
}

void hagokenUpdate(Special *s)
{
    s->x += 1<<8;
    if (s->x >= 512<<8) { // FIXME
        destroy(s);
        return;
    }

    s->frame++;
    if ((s->frame>>3)==3) s->frame=0;

    // DEBUG POSITIONING
    int scale = gscale;

    int pivotx = 32;
    int pivoty = 32;
    if (false) { // flipped
        pivotx = 64 - pivotx - 1;
    }

    int scaledw = (SPRITE_W * scale) >> SCALE_BITS;
    int scaledh = (SPRITE_H * scale) >> SCALE_BITS;
    int x = (s->x >> 8) - ((pivotx * scale) >> SCALE_BITS) - (SPRITE_W - scaledw) / 2;
    int y = (s->y >> 8) - ((pivoty * scale) >> SCALE_BITS) - (SPRITE_H - scaledh) / 2;
    int invscale = div32(1 << (8 + SCALE_BITS), scale);

    oamRotateScale(&oamMain, 9, 0, invscale, invscale);
    oamSetXY(&oamMain, MMAP_OAM_SPECIALS+SPECIAL_IDX(s), x, y);
}

void hagokenVblank(Special *s)
{
    int frame = s->frame>>3;
    dmaCopyHalfWords(2, s->desc->gfx+frame*64*64/2, s->desc->vramptr, 64*64/2);
}


// ******************** GENERAL

Special* alloc()
{
    for (int i=0;i<MAX_SPECIALS;i++)
        if ((specials[i].flags & FLAG_ACTIVE) == 0)
            return &specials[i];
    debugf("TOO MANY SPECIALS, CRASH!\n");
    while(1);
    return NULL;
}

void destroy(Special *s)
{
    oamSet(&oamMain, MMAP_OAM_SPECIALS+SPECIAL_IDX(s),
        0,0,0,0,0,0,0,0,0,0,0,0,0);
    memset(s, 0, sizeof(Special));
}

void specialCreate(u8 type, s32 x, s32 y)
{
    Special *s = alloc();
    s->flags |= FLAG_ACTIVE;
    s->desc = &gdesc[type-1];
    s->x = x;
    s->y = y;

    oamSet(&oamMain, MMAP_OAM_SPECIALS+SPECIAL_IDX(s),
        /*pos*/ 1000, 1000,
        /*pri*/ 0,
        /*pal*/ MMAP_PAL_SPECIALS(TYPE_HAGOKEN-1),
        SpriteSize_64x64, SpriteColorFormat_16Color, s->desc->vramptr,
        /*affine*/ 9, 0,
        /*hide*/ 0,
        /*flip*/ 0, 0,
        /*mosaic*/ 0);

    debugf("specialCreate: type %d, %d %d\n", type, x>>8, y>>8);
}

void specialSetScale(u32 scale)
{
    gscale = scale;
}

void specialVblank(void)
{
   for (int i=0;i<MAX_SPECIALS;i++)
        if ((specials[i].flags & FLAG_ACTIVE) != 0) {
            switch (specials[i].desc->type) {
            case TYPE_HAGOKEN:
                hagokenVblank(&specials[i]);
                break;
            }
        }
}

void specialUpdate(void)
{
    for (int i=0;i<MAX_SPECIALS;i++)
        if ((specials[i].flags & FLAG_ACTIVE) != 0) {
            switch (specials[i].desc->type) {
            case TYPE_HAGOKEN:
                hagokenUpdate(&specials[i]);
                break;
            }
        }
}

void specialInit(void)
{
    hagokenLoad(&gdesc[TYPE_HAGOKEN-1]);
}
