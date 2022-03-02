#include "common.h"

uint8_t is_animating_guess;

static int row;          // Row to animate
static int letter;       // Letter being flipped
static int8_t step;         //
static int8_t frame;
static int8_t delay;

const int tileSprite = 40;

void update_guess_animation() {
    delay ++;
    if(delay != 2) return;
    delay = 0;

    int y = 4;
    char ch = guess[row][letter];
    int s = letter + row * 5;

    int tile_sprite_pal = 0, tile_sprite_index;

    if (step == 0 && frame == 0) {
        // put sprite
        VDP_setSprite(tileSprite, 100 + letter * 24 - 4, y + row * 24 - 4, SPRITE_SIZE(3,3), TILE_ATTR_FULL(0, 0, 0, 0, 0));

        // remove tile
        VDP_fillTileMapRect(VDP_PLAN_A, 0, 12 + letter * 3, row * 3, 3, 3);
    }

    if(step == 1) {
        tile_sprite_pal = check_result[letter] + 1;
    }

    if(step == 0) {
        tile_sprite_index = tiles_word_tiles_outline + frame * 9;
        font_sprite(s, ch, 100 + letter * 24, y + row * 24, frame);
    } else {
        tile_sprite_index = tiles_word_tiles_filled + frame * 9;
        font_sprite(s, ch, 100 + letter * 24, y + row * 24, frame);
    }

    VDP_setSpriteAttribut(tileSprite, TILE_ATTR_FULL(tile_sprite_pal, 0, 0, 0, tile_sprite_index));

    if (step == 0) {
        if (frame == 3) { step = 1; }
        else {
            frame++;
        }
    } else {
        if(frame == 0) {
            // Add tile
            draw_tile(12 + letter * 3, row * 3, check_result[letter], 0, BG_A);

            step = 0;
            letter ++;
            if(letter == 5) {
                is_animating_guess = 0;
            }
        } else {
            frame--;
        }
    }
}

void animate_guess(int guess_row) {
    row = guess_row;
    letter = 0;
    step = 0;
    frame = 0;
    is_animating_guess = 1;
    delay = 0;

    VDP_setSpriteSize(tileSprite, SPRITE_SIZE(3,3));
    VDP_setSpriteAttribut(tileSprite, TILE_ATTR(0, 0, 0, 0));
}
