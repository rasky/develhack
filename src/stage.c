#include "stage.h"

#include <nds.h>
#include <stdio.h>

#include "io.h"

#include "anim.h" // just for SCALE_BITS

#include "debug.h"

typedef struct
{
    const char* id;
    const char* file;
    int background;
    int priority;
    BgType bg_type;
    BgSize bg_size;
    int x_offset;
    int y_offset;
    int width;
    int height;
    bool wraps;
} Layer;

StageDesc STAGES[] = {
    { "forest", 500, 256, 165, { 64, 160 } },
    { NULL }
};

Layer LAYERS[] = {
    { "forest", "forest-00", 0, 1, BgType_Text8bpp, BgSize_ER_512x512, 0, 0, 512, 192, true },
    { "forest", "forest-03", 1, 3, BgType_Text8bpp, BgSize_ER_512x512, 0, 0, 512, 160, true },
    { "forest", "forest-01", 2, 0, BgType_ExRotation, BgSize_ER_512x512, 0, 0, 512, 160, true },
    { "forest", "forest-02", 3, 2, BgType_ExRotation, BgSize_ER_512x512, 0, 0, 512, 160, true },
    { NULL, NULL }
};

struct {
    struct {
        int id; // the background id returned from bgInit; -1 if unused
        s32 scale;
        Layer *layer;
    } backgrounds[4];

    struct {
        s32 x;
        s32 y;
        u32 zoom;
    } camera;
} gStage;


int num_chunks(int size, int chunk_size)
{
    int chunks = 0;
    while (size > 0) {
        size -= chunk_size;
        ++chunks;
    }
    return chunks;
}

const StageDesc* stageLoad(const char* id)
{
    StageDesc* stage = STAGES;
    for ( ; stage->id != NULL; ++stage) {
        if (stage->id == NULL) {
            debugf("Stage not found: %s\n", id);
            return NULL;
        }

        if (strcmp(stage->id, id) == 0) {
            break;
        }
    }

    char filename[64];

    for (int i=0; i<4; ++i) {
        gStage.backgrounds[i].id = -1;
    }

    // an offset into VRAM in 2K chunks
    int tile_map_offset = 0;

    snprintf(filename, sizeof(filename), "%s.pal.bin", id);
    Bytes palette = slurp(filename);
    dmaCopy(palette.data, BG_PALETTE, palette.size);

    free(palette.data);

    bool loaded_tiles = false;
    for (Layer* layer = LAYERS; layer->id != NULL; ++layer) {
        if (strcmp(layer->id, id) != 0) {
            continue;
        }

        int bg = bgInit(layer->background, layer->bg_type, layer->bg_size, tile_map_offset, 2);
        bgSetPriority(bg, layer->priority);
        if (layer->wraps) {
            bgWrapOn(bg);
        } else {
            bgWrapOff(bg);
        }

        gStage.backgrounds[layer->background].layer = layer;
        gStage.backgrounds[layer->background].id = bg;
        s32 scale = div32(SCREEN_HEIGHT << (16+SCALE_BITS), layer->height << 16);

        debugf("%i scale: (%i %i) %f\n", layer->background, SCREEN_HEIGHT, layer->height,
            fixedToFloat(scale, SCALE_BITS));

        gStage.backgrounds[layer->background].scale = scale;

        if (!loaded_tiles) {
            snprintf(filename, sizeof(filename), "%s.img.bin", id);
            Bytes image = slurp(filename);
            dmaCopy(image.data, bgGetGfxPtr(bg), image.size);
            free(image.data);
            loaded_tiles = true;
        }

        snprintf(filename, sizeof(filename), "%s.map.bin", layer->file);
        Bytes map = slurp(filename);

        tile_map_offset += num_chunks(map.size, 2048);

        dmaCopy(map.data, bgGetMapPtr(bg), map.size);

        free(map.data);
    }

    return stage;
}

void stageSetPosition(s32 x, s32 y)
{
    if (x != gStage.camera.x || y != gStage.camera.y)
    {
        debugf("STAGE: Stage Position %ix%i\n", x >> 8, y >> 8);
        gStage.camera.x = x;
        gStage.camera.y = y;
    }
}

void stageSetZoom(u32 zoom)
{
    if (zoom != gStage.camera.zoom)
    {
        gStage.camera.zoom = zoom;

        s32 invscale = div32(1<<(8+SCALE_BITS), gStage.camera.zoom);

        debugf("STAGE: Zoom %f (%f)\n",
            fixedToFloat(gStage.camera.zoom, SCALE_BITS),
            fixedToFloat(invscale, 8));
    }
}

void stageUpdate()
{
    s32 invscale = div32(1<<(8+SCALE_BITS), gStage.camera.zoom);

    for (int i=0; i<4; ++i)
    {
        if (gStage.backgrounds[i].id == -1) {
            continue;
        }

        Layer* layer = gStage.backgrounds[i].layer;

        // s32 scale = gStage.backgrounds[i].scale;

        s32 x_offset = (-layer->x_offset * invscale);
        // s32 y_offset = (-layer->y_offset * invscale);

        bgSet(gStage.backgrounds[i].id, 0, invscale, invscale,
            x_offset + (gStage.camera.x * (4-i)),
            0,
            0, 0);
    }

    bgUpdate();
}

void stageVblank()
{
}
