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
    s32 x_offset;
    s32 y_offset;
    s32 width;
    s32 height;
    bool wraps;
} Layer;

StageDesc STAGES[] = {
    /* name     width   height   floory    fighter start X   fighter limits    */
    { "lounge", 1024,   256,     250,      { 412, 612 },     { 50, 970 },      },
    { NULL }
};

Layer LAYERS[] = {
    { "lounge", "lounge-00", 3, 0, BgType_ExRotation, BgSize_ER_1024x1024, 0, 0, 1024, 256, false },
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
        gStage.backgrounds[layer->background].scale = 1 << SCALE_BITS;

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
        // debugf("STAGE: Stage Position %ix%i\n", x >> 8, y >> 8);
        gStage.camera.x = x;
        gStage.camera.y = y;
    }
}

void stageSetZoom(u32 zoom)
{
    if (zoom != gStage.camera.zoom)
    {
        gStage.camera.zoom = zoom;

        // s32 invscale = div32(1<<(8+SCALE_BITS), gStage.camera.zoom);
        // debugf("STAGE: Zoom %f (%f)\n",
        //     fixedToFloat(gStage.camera.zoom, SCALE_BITS),
        //     fixedToFloat(invscale, 8));
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

        // if each layer had a scale value associated with it, you'd use it here:
        s32 scale = (invscale * gStage.backgrounds[i].scale) >> SCALE_BITS;

        // TODO: offset the different layers by different amounts for parallax
        bgSet(gStage.backgrounds[i].id, 0, scale, scale,
            layer->x_offset + gStage.camera.x,
            layer->y_offset + gStage.camera.y,
            0, 0);
    }

    bgUpdate();
}

void stageVblank()
{
}
