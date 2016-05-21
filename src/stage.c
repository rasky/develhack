#include <nds.h>
#include <stdio.h>

#include "io.h"

#include "debug.h"

typedef struct
{
    const char *id;
    const char *file;
    int priority;
    BgType bg_type;
    BgSize bg_size;
    int x_offset;
    int y_offset;
    int width;
    int height;
    bool wraps;
} Layer;

typedef struct
{
    const char *id;
    Layer layers[];
} Stage;

Stage STAGES[] = {
    {"forest"},
    {"industrial"},
    {NULL}
};

Layer LAYERS[] = {
    {"forest", "forest-00", 0, BgType_Text4bpp, BgSize_T_512x512, 0, 0, 272, 160, false},
    {"forest", "forest-03", 3, BgType_Text4bpp, BgSize_T_512x512, 0, 0, 272, 160, false},
    {"forest", "forest-01", 1, BgType_Rotation, BgSize_R_512x512, 0, 0, 272, 160, false},
    {"forest", "forest-02", 2, BgType_Rotation, BgSize_R_512x512, 0, 0, 272, 160, false},
    {"industrial", "industrial-00", 0, BgType_Rotation, BgSize_R_512x512, 0, 0, 272, 170, false},
    {"industrial", "industrial-01", 1, BgType_Rotation, BgSize_R_512x512, 0, 0, 272, 170, false},
    {"industrial", "industrial-02", 2, BgType_Rotation, BgSize_R_256x256, 0, 0, 272, 170, false},
    {"industrial", "industrial-03", 3, BgType_Rotation, BgSize_R_512x512, 0, 0, 272, 170, false},
    {NULL, NULL}
};

int num_chunks(int size, int chunk_size)
{
    int chunks = 0;
    while (size > 0)
    {
        size -= chunk_size;
        ++chunks;
    }
    return chunks;
}

void loadStage(const char *id)
{
    for (Stage *stage = STAGES; stage->id != NULL; ++stage)
    {
        if (stage->id == NULL)
        {
            debugf("Stage not found: %s\n", id);
            return;
        }

        if (strcmp(stage->id, id) == 0)
        {
            break;
        }
    }

    // an offset into VRAM in 2K chunks
    int tile_map_offset = 0;

    // an offset into VRAM in 16K chunks
    int graphics_data_offset = 2;

    int background = 0;
    for (Layer *layer = LAYERS; layer->id != NULL; ++layer)
    {
        if (strcmp(layer->id, id) != 0)
        {
            continue;
        }

        char filename[64];

        snprintf(filename, 63, "%s.img.bin", layer->file);
        Bytes image = slurp(filename);

        snprintf(filename, 63, "%s.pal.bin", layer->file);
        Bytes palette = slurp(filename);

        snprintf(filename, 63, "%s.map.bin", layer->file);
        Bytes map = slurp(filename);

        int old_tile_offset = tile_map_offset;
        int old_graphics_offset = graphics_data_offset;

        int bg = bgInit(background, layer->bg_type, layer->bg_size, tile_map_offset, graphics_data_offset);
        tile_map_offset += num_chunks(map.size, 2048);
        graphics_data_offset += num_chunks(image.size, 16384);
        debugf("Offsets: Tile: %x..%x / Graphics: %x..%x\n",
            old_tile_offset * 2048, (tile_map_offset) * 2048,
            old_graphics_offset * 16384, (graphics_data_offset) * 16384);
        bgSetPriority(bg, layer->priority);

        dmaCopy(image.data, bgGetGfxPtr(bg), image.size);
        dmaCopy(palette.data, BG_PALETTE, palette.size);
        dmaCopy(map.data, bgGetMapPtr(bg), map.size);

        if (++background > 3)
        {
            debugf("Ran out of backgrounds\n");
            break;
        }

        free(map.data);
        free(palette.data);
        free(image.data);
    }
}
