#include "stage.h"

#include <nds.h>
#include <stdio.h>

#include <lauxlib.h>

#include "anim.h" // just for SCALE_BITS
#include "debug.h"
#include "io.h"

// TODO: these two structs could probably be merged now
typedef struct
{
    bool inited;
    char id[32];
    char file[32];
    int priority;
    BgType bg_type;
    BgSize bg_size;
    s32 x_offset;
    s32 y_offset;
    s32 width;
    s32 height;
    bool wraps;
} Layer;

typedef struct {
    int id; // the background id returned from bgInit; -1 if unused
    bool scales;
    s32 parallax_x;
    s32 parallax_y;
} Background;

struct {
    StageDesc desc;
    Layer layers[4];
    Background backgrounds[4];

    struct {
        s32 x; // centre x coordinate
        s32 y; // floor y coordinate
        u32 zoom;
    } camera;

    lua_State *lua;
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

typedef struct {
    BgType type;
    BgSize size;
} BackgroundType;

BackgroundType background_types[] = {
    { BgType_Text8bpp, BgSize_T_512x512 },
    { BgType_ExRotation, BgSize_ER_1024x1024 },
};

static int setStage(lua_State *L)
{
    const char* name = lua_tostring(L, 1);

    strncpy(gStage.desc.id, name, sizeof(gStage.desc.id));
    gStage.desc.w = lua_tointeger(L, 2);
    gStage.desc.h = lua_tointeger(L, 3);
    gStage.desc.floory = lua_tointeger(L, 4);
    gStage.desc.fighterStartX[0] = 412;
    gStage.desc.fighterStartX[1] = 612;
    gStage.desc.fighterLimits[0] = 50;
    gStage.desc.fighterLimits[1] = 970;

    return 0;
}

static int setLayer(lua_State *L)
{
    int bg_num = lua_tointeger(L, 1);

    Layer *l = gStage.layers + bg_num;

    l->inited = true;

    const char* str;
    str = lua_tostring(L, 2);
    strncpy(l->id, str, sizeof(l->id));

    str = lua_tostring(L, 3);
    strncpy(l->file, str, sizeof(l->file));

    l->priority = lua_tointeger(L, 4);

    const BackgroundType* bg_type = lua_topointer(L, 5);

    l->bg_type = bg_type->type;
    l->bg_size = bg_type->size;

    l->x_offset = lua_tointeger(L, 6);
    l->y_offset = lua_tointeger(L, 7);
    l->width = lua_tointeger(L, 8);
    l->height = lua_tointeger(L, 9);
    l->wraps = lua_toboolean(L, 10);

    debugf("Stage: %s %s %i %i %i %i %i %i %i %i %i\n",
        l->id, l->file, bg_num, l->priority,
        l->bg_type, l->bg_size,
        l->x_offset, l->y_offset,
        l->width, l->height,
        l->wraps);

    return 0;
}

void stageInit()
{
    lua_State *L = gStage.lua = luaL_newstate();

    lua_createtable(L, 0, 10);

    lua_pushstring(L, "text_8_bpp_512x512");
    lua_pushlightuserdata(L, &background_types[0]);
    lua_settable(L, -3);

    lua_pushstring(L, "exrot_1024x1024");
    lua_pushlightuserdata(L, &background_types[1]);
    lua_settable(L, -3);

    lua_setglobal(L, "Background");

    lua_register(L, "set_stage", &setStage);
    lua_register(L, "set_layer", &setLayer);
}

const StageDesc* stageLoad(const char* id)
{
    lua_State *L = gStage.lua;
    if (luaL_dofile(L, "lounge.luac") != 0) {
        debugf("Error reading lounge.luac: %s\n", lua_tostring(L, -1));
    }

    for (int i = 0; i < 4; ++i) {
        gStage.layers[i].inited = false;
        gStage.backgrounds[i].id = -1;
    }

    lua_getglobal(L, "init");
    lua_pcall(L, 0, 0, 0);

    // lua_getglobal(L, "stage");
    // assert(lua_istable(L, -1));

    // lua_pushstring(L, "palette");
    // lua_gettable(L, -2);

    // lua_pop(L, 1);

    // lua_pushstring(L, "layers");
    // lua_gettable(L, -2);

    // lua_pushnil(L);
    // while(lua_next(L, -2) != 0) {
    //     debugf("%s - %s\n",
    //         lua_tostring(L, -2),
    //         lua_typename(L, lua_type(L, -1)));
    //     lua_pop(L, 1);
    // }

    char filename[64];

    // an offset into VRAM in 2K chunks
    int tile_map_offset = 0;

    snprintf(filename, sizeof(filename), "%s.pal.bin", id);
    Bytes palette = slurp(filename);
    dmaCopy(palette.data, BG_PALETTE, palette.size);
    free(palette.data);

    bool loaded_tiles = false;
    for (int bg=0; bg<4; ++bg) {
        Layer* layer = gStage.layers + bg;
        if (!layer->inited) {
            continue;
        }

        int id = bgInit(bg, layer->bg_type, layer->bg_size, tile_map_offset, 5);
        bgSetPriority(id, layer->priority);
        if (layer->wraps) {
            bgWrapOn(id);
        } else {
            bgWrapOff(id);
        }

        Background *background = gStage.backgrounds + bg;

        background->id = bg;
        background->parallax_x = div32((layer->width << 16), (gStage.desc.w << 8));
        background->parallax_y = div32((layer->height << 16), (gStage.desc.h << 8));

        switch (layer->bg_type) {
            case BgType_Text4bpp:
            case BgType_Text8bpp:
                background->scales = false;
                break;

            case BgType_Rotation:
            case BgType_ExRotation:
                background->scales = true;
                break;

            default:
                debugf("I know nothing about your background type: %i", layer->bg_type);
                break;
        }

        if (!loaded_tiles) {
            snprintf(filename, sizeof(filename), "%s.img.bin", layer->id);
            Bytes image = slurp(filename);
            dmaCopy(image.data, bgGetGfxPtr(bg), image.size);
            free(image.data);
            loaded_tiles = true;
        }

        // snprintf(filename, sizeof(filename), "%s.map.bin", layer->file);
        debugf("File: %s\n", layer->file);
        Bytes map = slurp(layer->file);

        tile_map_offset += num_chunks(map.size, 2048);

        dmaCopy(map.data, bgGetMapPtr(bg), map.size);
        free(map.data);
    }

    return &gStage.desc;
}

void stageSetPosition(s32 left, s32 right, s32 top)
{
    // calculate width and height of the rectangle
    s32 width = (right - left);
    s32 height = (width * (SCREEN_HEIGHT<<8 / SCREEN_WIDTH)) >> 8;

    s32 x = left + (width / 2);
    s32 y = top + height;

    if (x != gStage.camera.x || y != gStage.camera.y) {
        // debugf("STAGE: Stage Position %ix%i\n", x >> 8, y >> 8);
        gStage.camera.x = x;
        gStage.camera.y = y;
    }
}

void stageSetZoom(u32 zoom)
{
    if (zoom != gStage.camera.zoom) {
        gStage.camera.zoom = zoom;

        // s32 invscale = div32(1<<(8+SCALE_BITS), gStage.camera.zoom);
        // debugf("STAGE: Zoom %f (%f)\n",
        //     fixedToFloat(gStage.camera.zoom, SCALE_BITS),
        //     fixedToFloat(invscale, 8));
    }
}

void stageUpdate()
{
    s32 invscale = div32(1 << (8 + SCALE_BITS), gStage.camera.zoom);

    for (int i = 0; i < 4; ++i) {
        Background *bg = gStage.backgrounds + i;
        Layer *layer = gStage.layers + i;
        if (bg->id == -1) {
            continue;
        }

        if (bg->scales) {
            s32 width = div32(SCREEN_WIDTH << (8 + SCALE_BITS), gStage.camera.zoom);
            s32 height = div32(SCREEN_HEIGHT << (8 + SCALE_BITS), gStage.camera.zoom);

            s32 x_offset = gStage.camera.x - (width / 2);
            s32 y_offset = gStage.camera.y - height;

            x_offset = (x_offset * bg->parallax_x) >> 8;
            y_offset = (y_offset * bg->parallax_y) >> 8;

            bgSet(bg->id, 0, invscale, invscale,
                x_offset,
                y_offset,
                0, 0);
        } else {
            s32 x_offset = gStage.camera.x - (SCREEN_WIDTH / 2);
            s32 y_offset = gStage.camera.y;

            x_offset = (x_offset * bg->parallax_x) >> 8;
            y_offset = (y_offset * bg->parallax_y) >> 8;

            bgSetScrollf(bg->id,
                x_offset + layer->x_offset,
                y_offset + layer->y_offset);
        }
    }

    bgUpdate();
}

void stageVblank()
{
}
