#include "hitbox.h"

#include <stdbool.h>

#include "debug.h"
#include "fixmath.h"

#define SPRITE_WIDTH 128  // a fighter

bool hitboxInvalid(const Hitbox* hitbox)
{
    return hitbox->w == 0 || hitbox->h == 0;
}

bool hitboxIsRed(const Hitbox* hitbox)
{
    return hitbox->red;
}

bool hitboxIsWhite(const Hitbox* hitbox)
{
    return hitbox->white;
}

bool hitboxIsBlue(const Hitbox* hitbox)
{
    return !hitboxIsRed(hitbox) && !hitboxIsWhite(hitbox);
}

bool hitboxIntersects(
    const Hitbox* src_, fix23_8 srcX, fix23_8 srcY, bool flipsrc,
    const Hitbox* dst_, fix23_8 dstX, fix23_8 dstY, bool flipdst)
{
    Hitbox src = *src_;
    Hitbox dst = *dst_;

    if (flipsrc)
        src.x = (SPRITE_WIDTH - src.x - 1) - (src.w - 1);
    if (flipdst)
        dst.x = (SPRITE_WIDTH - dst.x - 1) - (dst.w - 1);

    return !(((src.x + src.w + (srcX >> 8)) < (dst.x + (dstX >> 8)))
        || ((dst.x + dst.w + (dstX >> 8)) < (src.x + (srcX >> 8)))
        || ((src.y + src.h + (srcY >> 8)) < (dst.y + (dstY >> 8)))
        || ((dst.y + dst.h + (dstY >> 8)) < (src.y + (srcY >> 8))));
}

bool hitboxCheckHit(
    const Hitbox* src, int nsrc, fix23_8 srcX, fix23_8 srcY, bool flipsrc,
    const Hitbox* dst, int ndst, fix23_8 dstX, fix23_8 dstY, bool flipdst)
{

    // Go through all red boxes in src
    for (int i = 0; i < nsrc; i++) {
        const Hitbox* h1 = &src[i];
        if (hitboxInvalid(h1) || !hitboxIsRed(h1))
            continue;

        // Go through all blue boxes in dst
        for (int j = 0; j < ndst; j++) {
            const Hitbox* h2 = &dst[j];
            if (hitboxInvalid(h2) || !hitboxIsBlue(h2))
                continue;

            // Check if the red box from src intersects
            // a blue box in dst
            if (hitboxIntersects(h1, srcX, srcY, flipsrc, h2, dstX, dstY, flipdst))
                return true;
        }
    }

    return false;
}

Hitbox hitboxGetBlue(const Hitbox* src, int nsrc, fix23_8 srcX, fix23_8 srcY, bool flipsrc)
{
    Hitbox h = WBOX(0,0,0,0);

    // Go through all red boxes in src
    for (int i = 0; i < nsrc; i++) {
        const Hitbox* h1 = &src[i];
        if (hitboxInvalid(h1) || !hitboxIsBlue(h1))
            continue;

        h = *h1;
        h.x += srcX>>8;
        h.y += srcY>>8;
        if (flipsrc) {
            h.x = (SPRITE_WIDTH - h.x - 1) - (h.w - 1);
        }
        break;
    }

    return h;
}


void testHitbox()
{
    Hitbox h1 = {.x = 0, .y = 0, .w = 10, .h = 10, .red = 0 };
    Hitbox h2 = {.x = 30, .y = 30, .w = 10, .h = 10, .red = 1 };
    debugf("TEST: No collision: (exp = 0, ret = %d)\n", hitboxIntersects(&h1, 0, 0, false, &h2, 0, 0, false));
    debugf("TEST: No collision, reversed: (exp = 0, ret = %d)\n", hitboxIntersects(&h2, 0, 0, false, &h1, 0, 0, false));
    debugf("TEST: Hitbox is blue: (exp = 0, ret = %d)\n", hitboxIsBlue(&h2));
    debugf("TEST: Hitbox is red: (exp = 1, ret = %d)\n", hitboxIsRed(&h2));

    Hitbox h3 = {.x = 0, .y = 0, .w = 10, .h = 10, .red = 0 };
    Hitbox h4 = {.x = 10, .y = 10, .w = 10, .h = 10, .red = 0 };
    debugf("TEST: Collision: (exp = 1, ret = %d)\n", hitboxIntersects(&h3, 0, 0, false, &h4, 0, 0, false));
    debugf("TEST: Collision, reversed: (exp = 1, ret = %d)\n", hitboxIntersects(&h4, 0, 0, false, &h3, 0, 0, false));
}
