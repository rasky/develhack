#include <nds.h>

#include "io.h"

#include "debug.h"

void loadStage()
{
    Bytes image = slurp("test.img.bin");
    Bytes map = slurp("test.map.bin");
    Bytes palette = slurp("test.pal.bin");

    int bg = bgInit(3, BgType_Rotation, BgSize_R_256x256, 0, 1);
    bgSetPriority(bg, 3);

    // Set rotation and scaling
    REG_BG3PA = 1 << 8;
    REG_BG3PB = 0;
    REG_BG3PC = 0;
    REG_BG3PD = 1 << 8;

    dmaCopy(image.data, bgGetGfxPtr(bg), image.size);
    dmaCopy(map.data, bgGetMapPtr(bg), map.size);
    dmaCopy(palette.data, BG_PALETTE, palette.size);
}
