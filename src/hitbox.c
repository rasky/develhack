#include "hitbox.h"

#include <stdbool.h>

#include "debug.h"
#include "fixmath.h"

bool hitboxIsRed(const Hitbox* hitbox)
{
    return hitbox->red == 1;
}

bool hitboxIsBlue(const Hitbox* hitbox)
{
    return !hitboxIsRed(hitbox);
}

bool hitboxesAreColliding(
    const Hitbox* src, fix23_8 srcX, fix23_8 srcY,
    const Hitbox* dst, fix23_8 dstX, fix23_8 dstY)
{
    return !(((src->x + src->w + fix23_to_int(srcX)) < (dst->x + fix23_to_int(dstX)))
        || ((dst->x + dst->w + fix23_to_int(dstX)) < (src->x + fix23_to_int(srcX)))
        || ((src->y + src->h + fix23_to_int(srcY)) < (dst->y + fix23_to_int(dstY)))
        || ((dst->y + dst->h + fix23_to_int(dstY)) < (src->y + fix23_to_int(srcY))));
}

void testHitbox()
{
    Hitbox h1 = {.x = 0, .y = 0, .w = 10, .h = 10, .red = 0 };
    Hitbox h2 = {.x = 30, .y = 30, .w = 10, .h = 10, .red = 1 };
    debugf("TEST: No collision: (exp = 0, ret = %d)\n", hitboxesAreColliding(&h1, 0, 0, &h2, 0, 0));
    debugf("TEST: No collision, reversed: (exp = 0, ret = %d)\n", hitboxesAreColliding(&h2, 0, 0, &h1, 0, 0));
    debugf("TEST: Hitbox is blue: (exp = 0, ret = %d)\n", hitboxIsBlue(&h2));
    debugf("TEST: Hitbox is red: (exp = 1, ret = %d)\n", hitboxIsRed(&h2));

    Hitbox h3 = {.x = 0, .y = 0, .w = 10, .h = 10, .red = 0 };
    Hitbox h4 = {.x = 10, .y = 10, .w = 10, .h = 10, .red = 0 };
    debugf("TEST: Collision: (exp = 1, ret = %d)\n", hitboxesAreColliding(&h3, 0, 0, &h4, 0, 0));
    debugf("TEST: Collision, reversed: (exp = 1, ret = %d)\n", hitboxesAreColliding(&h4, 0, 0, &h3, 0, 0));
}
