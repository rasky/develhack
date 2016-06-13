#include "hitbox.h"

#include <stdbool.h>

#include "debug.h"
#include "fixmath.h"

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
    const Hitbox* src, fix23_8 srcX, fix23_8 srcY,
    const Hitbox* dst, fix23_8 dstX, fix23_8 dstY)
{
    return !(((src->x + src->w + (srcX >> 8)) < (dst->x + (dstX >> 8)))
        || ((dst->x + dst->w + (dstX >> 8)) < (src->x + (srcX >> 8)))
        || ((src->y + src->h + (srcY >> 8)) < (dst->y + (dstY >> 8)))
        || ((dst->y + dst->h + (dstY >> 8)) < (src->y + (srcY >> 8))));
}

bool hitboxCheckHit(
    const Hitbox* src, int nsrc, fix23_8 srcX, fix23_8 srcY,
    const Hitbox* dst, int ndst, fix23_8 dstX, fix23_8 dstY)
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
            if (hitboxIntersects(h1, srcX, srcY, h2, dstX, dstY))
                return true;
        }
    }

    return false;
}

void testHitbox()
{
    Hitbox h1 = {.x = 0, .y = 0, .w = 10, .h = 10, .red = 0 };
    Hitbox h2 = {.x = 30, .y = 30, .w = 10, .h = 10, .red = 1 };
    debugf("TEST: No collision: (exp = 0, ret = %d)\n", hitboxIntersects(&h1, 0, 0, &h2, 0, 0));
    debugf("TEST: No collision, reversed: (exp = 0, ret = %d)\n", hitboxIntersects(&h2, 0, 0, &h1, 0, 0));
    debugf("TEST: Hitbox is blue: (exp = 0, ret = %d)\n", hitboxIsBlue(&h2));
    debugf("TEST: Hitbox is red: (exp = 1, ret = %d)\n", hitboxIsRed(&h2));

    Hitbox h3 = {.x = 0, .y = 0, .w = 10, .h = 10, .red = 0 };
    Hitbox h4 = {.x = 10, .y = 10, .w = 10, .h = 10, .red = 0 };
    debugf("TEST: Collision: (exp = 1, ret = %d)\n", hitboxIntersects(&h3, 0, 0, &h4, 0, 0));
    debugf("TEST: Collision, reversed: (exp = 1, ret = %d)\n", hitboxIntersects(&h4, 0, 0, &h3, 0, 0));
}
