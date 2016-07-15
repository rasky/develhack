#include "anim.h"

#include <stdio.h>

#include <nds.h>

#include "debug.h"
#include "frames.h"
#include "input.h"
#include "specials.h"

#define SPRITE_W 64
#define SPRITE_H 64
#define SPRITE_SIZE (SPRITE_W * SPRITE_H / 2)
#define FRAME_SIZE (SPRITE_SIZE * 4)
#define MAX_GFX_FRAMES 64

// Misc flags for AnimFighter
#define FLAGS_IS_RIGHT (1 << 0)

typedef struct {
    // Screen position of the fighter (fixed point .8).
    // This is the point where the fighter's pivot will be placed.
    u32 x, y;

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
    u16* vramptr[4];

    // Pointer to the animation description of this fighter (see frames.c)
    const AnimDesc* desc;

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

static void animLoad(int fidx, const AnimDesc* desc)
{
    const int MAX_ANIMS = 16;

    int lastgfxindex = 0;
    struct {
        const char* filename;
        int gfxindex;
    } loadedAnims[MAX_ANIMS];
    memset(loadedAnims, 0, sizeof(loadedAnims));

    AnimFighter* f = &gFighters[fidx];
    f->desc = desc;
    for (int i = 0; i < ANIM_DESC_MAX_FRAMES; i++) {
        const AnimFrame* cur = &desc->frames[i];

        if (cur->filename == NULL) {
            break;
        }
        if (cur->idx != i) { // consistency check
            debugf("error: invalid index in frame\n");
            return;
        }

        // Find the animation (if already loaded)
        int a = 0;
        for (; a < MAX_ANIMS; a++) {
            if (loadedAnims[a].filename == NULL || loadedAnims[a].filename == cur->filename) {
                break;
            }
        }
        // ... or load the new animation
        if (loadedAnims[a].filename != cur->filename) {
            char fnbuf[128];
            strcpy(fnbuf, cur->filename);
            strcat(fnbuf, ".img.bin");

            debugf("loading %s\n", fnbuf);
            FILE* fp = fopen(fnbuf, "rb");
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

    // Load palette and copy to palette RAM
    debugf("loading palette %s\n", f->desc->palette);
    FILE* fp = fopen(f->desc->palette, "rb");
    if (fp == NULL) {
        debugf("error: cannot load %s\n", f->desc->palette);
        return;
    }
    fread(f->pal, 1, 256 * 2, fp);
    fclose(fp);

    // Flush the data cache for graphics, so that it can be
    // immediately blitted to VRAM.
    DC_FlushRange(f->gfx, sizeof(f->gfx));
    DC_FlushRange(f->pal, sizeof(f->pal));
}

void animInit(void)
{
    animLoad(0, &Dummy);
    animLoad(1, &Dummy);

    oamInit(&oamMain, SpriteMapping_1D_64, false);
    oamDisable(&oamMain);

    for (int fx = 0; fx < 2; fx++) {
        AnimFighter* f = &gFighters[fx];
        for (int i = 0; i < 4; i++) {
            u16* ptr = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
            f->vramptr[i] = ptr;
            oamSet(&oamMain, fx * 4 + i,
                /*pos*/ 0, 0,
                /*pri*/ 0,
                /*pal*/ fx,
                SpriteSize_64x64, SpriteColorFormat_16Color, ptr,
                /*affine*/ fx, 0,
                /*hide*/ 0,
                /*flip*/ 0, 0,
                /*mosaic*/ 0);
        }

        // Start with idle animation
        f->scale = 1 << SCALE_BITS;
        f->curframe = f->desc->keyframes.idle;
    }

    dmaCopyHalfWords(0, gFighters[0].pal, SPRITE_PALETTE, 16 * 2);
    dmaCopyHalfWords(0, gFighters[1].pal, SPRITE_PALETTE + 16, 16 * 2);

    oamEnable(&oamMain);
    debugf("sprites ok!\n");

    // Sane defaults for scaling (position can be 0,0, it doesn't matter,
    // but scale=0 means nothing is displayed by default).
    gFighters[0].scale = 1 * (1 << SCALE_BITS);
    gFighters[1].scale = 1 * (1 << SCALE_BITS);
}

static void animUpdateOam(int fx)
{
    AnimFighter* f = &gFighters[fx];
    const AnimDesc* fdesc = f->desc;

    int pivotx = fdesc->pivotx;
    int pivoty = fdesc->pivoty;
    if (f->flags & FLAGS_IS_RIGHT) {
        pivotx = 127 - pivotx;
    }

    // Compute coordinates so that the scale factor is computed using the pivot as
    // center of scaling. This means that the sprite's pivot will be positioned at
    // the specified coordinate (f->x, f->y), and then the sprite will be scaled.
    int scaledw = (SPRITE_W * f->scale) >> SCALE_BITS;
    int scaledh = (SPRITE_H * f->scale) >> SCALE_BITS;
    int x = (f->x >> 8) - ((pivotx * f->scale) >> SCALE_BITS) - (SPRITE_W - scaledw) / 2;
    int y = (f->y >> 8) - ((pivoty * f->scale) >> SCALE_BITS) - (SPRITE_H - scaledh) / 2;

    // Calculate inverse of scale in .8 fractional format (which is what
    // oamRotateScale() expects).
    int invscale = div32(1 << (8 + SCALE_BITS), f->scale);
    if (!(f->flags & FLAGS_IS_RIGHT)) {
        oamRotateScale(&oamMain, fx, 0, invscale, invscale);
        oamSetXY(&oamMain, fx * 4 + 0, x, y);
        oamSetXY(&oamMain, fx * 4 + 1, x + scaledw, y);
        oamSetXY(&oamMain, fx * 4 + 2, x, y + scaledh);
        oamSetXY(&oamMain, fx * 4 + 3, x + scaledw, y + scaledh);
    } else {
        oamRotateScale(&oamMain, fx, 0, -invscale, invscale);
        oamSetXY(&oamMain, fx * 4 + 0, x + scaledw, y);
        oamSetXY(&oamMain, fx * 4 + 1, x, y);
        oamSetXY(&oamMain, fx * 4 + 2, x + scaledw, y + scaledh);
        oamSetXY(&oamMain, fx * 4 + 3, x, y + scaledh);
    }
}

static void animBeginAnimation(int fx, int keyframe)
{
    AnimFighter* f = &gFighters[fx];
    const AnimDesc* fdesc = f->desc;

    f->curframe = keyframe;
    f->cftime = fdesc->frames[f->curframe].speed;
}

#define INPUT_DIR_MASK (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT)

static void animProcessInput(int fx, u32 input)
{
    AnimFighter* f = &gFighters[fx];
    const AnimDesc* fdesc = f->desc;
    u16 fflags = fdesc->frames[f->curframe].flags;

    // Check keys for hit/special moves
    if (FSTATUS(fflags) != FST_NONE) {
        // Normal walk/jump animation, check buttons
        if (input & KEY_SPECIAL_HADUKEN) {
            if (FSTATUS(fflags) != FST_JMP) {
                animBeginAnimation(fx, fdesc->keyframes.special);
                return;
            }
        } else if (input & KEY_A) {
            if (FSTATUS(fflags) != FST_JMP) {
                animBeginAnimation(fx, fdesc->keyframes.punch);
                return;
            } else {
                // TODO: jump punch
            }
        } else if (input & KEY_B) {
            if (FSTATUS(fflags) != FST_JMP) {
                animBeginAnimation(fx, fdesc->keyframes.kick);
                return;
            } else {
                // TODO: jump kick
            }
        }
    }

    // Check inputs for movements
    u32 dirinput = input & INPUT_DIR_MASK;

    if (dirinput & KEY_UP) {
        if (fflags & FCJMP) {
            animBeginAnimation(fx, fdesc->keyframes.jump);
            return;
        }
    }

    switch (dirinput) {
    case 0:
        if (fflags & FCIDLE) {
            animBeginAnimation(fx, fdesc->keyframes.idle);
        }
        break;

    case KEY_RIGHT:
        if (!(f->flags & FLAGS_IS_RIGHT)) {
            if (fflags & FCFWD) {
                animBeginAnimation(fx, fdesc->keyframes.forward);
            }
        } else {
            if (fflags & FCBWD) {
                animBeginAnimation(fx, fdesc->keyframes.backward);
            }
        }
        break;

    case KEY_LEFT:
        if (!(f->flags & FLAGS_IS_RIGHT)) {
            if (fflags & FCBWD) {
                animBeginAnimation(fx, fdesc->keyframes.backward);
            }
        } else {
            if (fflags & FCFWD) {
                animBeginAnimation(fx, fdesc->keyframes.forward);
            }
        }
        break;
    }
}

void animUpdateStatus(u32 input)
{
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

void animRedraw()
{
    for (int fx = 0; fx < 2; fx++) {
        AnimFighter* f = &gFighters[fx];
        const AnimDesc* fdesc = f->desc;
        const AnimFrame* curframe = &fdesc->frames[f->curframe];

        // See if it's time to display next frame.
        if (f->cftime > 0) {
            f->cftime--;
        } else {
            // Check which frame must be displayed and
            // set the new countdown.
            if (curframe->next == 0) {
                f->curframe++;
            } else {
                f->curframe = curframe->next - 1;
            }
            f->cftime = fdesc->frames[f->curframe].speed;
            if (fdesc->frames[f->curframe].flags & FSPECIAL)
                specialCreate(fdesc->special.type,
                    f->x+((s32)fdesc->special.offx<<8),
                    f->y+((s32)fdesc->special.offy<<8));
        }

        animUpdateOam(fx);
    }
}

void animVblank(void)
{
    oamUpdate(&oamMain);

    for (int fx = 0; fx < 2; fx++) {
        AnimFighter* f = &gFighters[fx];
        int gfxidx = f->gfxindex[f->curframe];
        u8* gfx = &f->gfx[gfxidx * FRAME_SIZE];

        for (int i = 0; i < 4; i++) {
            dmaCopyHalfWords(2,
                (u16*)(gfx + i * SPRITE_SIZE),
                (u16*)f->vramptr[i],
                SPRITE_SIZE);
        }
    }
}

void animFighterSetPosition(int fx, u32 x, u32 y)
{
    gFighters[fx].x = x;
    gFighters[fx].y = y;
}

void animFighterSetScale(int fx, u32 scale)
{
    gFighters[fx].scale = scale;
}

void animFighterGetState(int fx, int* status, int* movex, int* movey)
{
    AnimFighter* f = &gFighters[fx];
    const AnimDesc* fdesc = f->desc;
    const AnimFrame* fframe = &fdesc->frames[f->curframe];

    if (status)
        *status = FSTATUS(fframe->flags);
    if (movex)
        *movex = fframe->movex;
    if (movey)
        *movey = fframe->movey;
}

const Hitbox* animFighterGetHitboxes(int fx, u8* dmg, bool *flipped)
{
    AnimFighter* f = &gFighters[fx];
    const AnimDesc* fdesc = f->desc;
    const AnimFrame* fframe = &fdesc->frames[f->curframe];

    if (dmg)
        *dmg = fframe->damage;
    if (flipped)
        *flipped = (f->flags & FLAGS_IS_RIGHT);

    return fframe->boxes;
}

void animFighterHit(int fx)
{
    AnimFighter* f = &gFighters[fx];
    const AnimDesc* fdesc = f->desc;

    // TODO: handle hit in jump
    animBeginAnimation(fx, fdesc->keyframes.hit);
}

void animFighterFall(int fx)
{
    AnimFighter* f = &gFighters[fx];
    const AnimDesc* fdesc = f->desc;

    animBeginAnimation(fx, fdesc->keyframes.fall);
}
