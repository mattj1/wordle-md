#include "common.h"
#include "res_generated.h"

int tiles_cursor;
int tiles_word_tiles_filled;
int tiles_word_tiles_outline;
int tiles_large_font;
int tiles_misc;

uint16_t num_tiles_loaded = TILE_USERINDEX;

s16 hscroll[32];

void clear_screen() {
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    for(int i = 0; i < MAX_VDP_SPRITE; i++) {
        VDP_setSprite(i, 0, 0, SPRITE_SIZE(1, 1), 0);
    }

    VDP_updateSprites(0, DMA);
}

uint16_t load_tileset(tileset_t *tileset) {
    uint16_t start_tile = num_tiles_loaded;
    VDP_loadTileData(&TILES[tileset->start_tile * 8], start_tile, tileset->num_tiles, DMA_QUEUE);

    num_tiles_loaded += tileset->num_tiles;

    return start_tile;
}

void font_print_string_to_tilemap(int x, int y, VDPPlane plane, const char *string) {
    char *ch = string;
    do {
        sprite_to_tilemap(font_sprite_index(*ch, 0), x ,y, 2, 2, TILE_ATTR(0, 0, 0, 0), plane);

        x += 2;
    } while(*++ch);
}

int font_sprite_index(char letter, int frame) {
    return tiles_large_font + (letter - 'A') * 4 * 4 + frame * 4;
}

void font_sprite(int sprite, char letter, s16 x, s16 y, int frame) {
    int index = font_sprite_index(letter, frame);

    // pri 0. Palette?
    VDP_setSprite(sprite, x, y, SPRITE_SIZE(2,2), TILE_ATTR_FULL(0, 0, 0, 0, index));
}

void sprite_to_tilemap(u16 index, int x, int y, int sprite_width, int sprite_height, u16 base_attr, VDPPlane plane) {
    for (int j = 0; j < sprite_width; j++) {
        for(int i = 0; i < sprite_height; i++) {
            VDP_setTileMapXY(plane, base_attr | index, x + j, y + i);
            index ++;
        }
    }
}

void draw_tile(int x, int y, int t, int frame, VDPPlane plane) {
    // Gray, orange, green, outline
    int st = frame * 9;
    int pal = t + 1;
    if (t == 3) {
        pal = 0;
        st += tiles_word_tiles_outline;
    } else {
        st += tiles_word_tiles_filled;
    }

    sprite_to_tilemap(st, x, y, 3, 3, TILE_ATTR(pal, 0, 0, 0), plane);
}

void graphics_load() {
    tiles_cursor = load_tileset(&tile_sets[TILESET_CURSOR]);
    tiles_word_tiles_filled = load_tileset(&tile_sets[TILESET_WORD_TILES_FILLED]);
    tiles_word_tiles_outline = load_tileset(&tile_sets[TILESET_WORD_TILES_OUTLINE]);
    tiles_large_font = load_tileset(&tile_sets[TILESET_LARGE_FONT]);
    tiles_misc = load_tileset(&tile_sets[TILESET_MISC]);
}

void cursor_show() {
    VDP_setSprite(cursor_sprite, cursorSpritePos[0], cursorSpritePos[1], SPRITE_SIZE(2,2), TILE_ATTR_FULL(0, 0, 0, 0, tiles_cursor));
}

void cursor_hide() {
    VDP_setSprite(cursor_sprite, 0,0, SPRITE_SIZE(1,1), TILE_ATTR_FULL(0, 0, 0, 0, 0));
}

void bottom_out() {
    for(int planeY = 0; planeY < 64; planeY += 8) {
        VDP_setVerticalScroll(BG_B, -planeY);
        VDP_setSpritePosition(cursor_sprite, cursorSpritePos[0], cursorSpritePos[1] + planeY);
        VDP_updateSprites(42, DMA);
        SYS_doVBlankProcess();
    }

    cursor_hide();
    VDP_clearTextAreaBG(BG_B, 0,20,40,8);
}

void bottom_in() {
    for(int planeY = 64; planeY >= 0; planeY -= 8) {
        VDP_setVerticalScroll(BG_B, -planeY);
        VDP_setSpritePosition(cursor_sprite, cursorSpritePos[0], cursorSpritePos[1] + planeY);
        VDP_updateSprites(42, DMA);
        SYS_doVBlankProcess();
    }
}
