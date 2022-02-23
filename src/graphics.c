#include "common.h"
#include "res_generated.h"

int tiles_cursor;
int tiles_word_tiles_filled;
int tiles_word_tiles_outline;
int tiles_large_font;
int tiles_misc;

uint16_t num_tiles_loaded = TILE_USERINDEX;

int16_t hscroll[32];

uint16_t load_tileset(tileset_t *tileset) {
    uint16_t start_tile = num_tiles_loaded;
    vdp_tiles_load(&TILES[tileset->start_tile * 8], start_tile, tileset->num_tiles);
    num_tiles_loaded += tileset->num_tiles;

    return start_tile;
}

void font_sprite(VDPSprite *sprite, char letter, int x, int y, int frame) {
    sprite->attr = tiles_large_font + (letter - 'A') * 4 * 4 + frame * 4;
    sprite_pos(*sprite, x, y);
    sprite_size(*sprite, 2, 2);
    sprite_pri(*sprite, 0); // 1?
}


void draw_tile(int x, int y, int t, int frame) {
    // Gray, orange, green, outline
    int st = frame * 9;
    int pal = t + 1;
    if (t == 3) {
        pal = 0;
        st += tiles_word_tiles_outline;
    } else {
        st += tiles_word_tiles_filled;
    }

    for (int i = 0; i < 3; i++) {
        vdp_map_xy(VDP_PLAN_B, TILE_ATTR(pal, 0, 0, 0, st), x, y);
        st++;
        vdp_map_xy(VDP_PLAN_B, TILE_ATTR(pal, 0, 0, 0, st), x, y + 1);
        st++;
        vdp_map_xy(VDP_PLAN_B, TILE_ATTR(pal, 0, 0, 0, st), x, y + 2);
        st++;
        x++;
    }
}

void graphics_load() {
    tiles_cursor = load_tileset(&tile_sets[TILESET_CURSOR]);
    tiles_word_tiles_filled = load_tileset(&tile_sets[TILESET_WORD_TILES_FILLED]);
    tiles_word_tiles_outline = load_tileset(&tile_sets[TILESET_WORD_TILES_OUTLINE]);
    tiles_large_font = load_tileset(&tile_sets[TILESET_LARGE_FONT]);
    tiles_misc = load_tileset(&tile_sets[TILESET_MISC]);
}