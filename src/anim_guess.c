#include "common.h"

uint8_t is_animating_guess;

static int8_t row;          // Row to animate
static int8_t letter;       // Letter being flipped
static int8_t step;         //
static int8_t frame;
static int8_t delay;

static VDPSprite *tileSprite;

void update_guess_animation() {
    delay ++;
    if(delay != 2) return;
    delay = 0;

    int y = 4;
    char ch = guess[row][letter];
    VDPSprite *s = &sprites[letter + row * 5];

    if (step == 0 && frame == 0) {
        // put sprite
        sprite_pos(*tileSprite, 100 + letter * 24 - 4, y + row * 24 - 4);
        sprite_pal(*tileSprite, 0);

        // remove tile
        vdp_map_fill_rect(VDP_PLAN_B, 0, 12 + letter * 3, row * 3, 3, 3, 0);
    }

    if(step == 1 && frame == 3) {
        sprite_pal(*tileSprite, check_result[letter] + 1);
    }

    if(step == 0) {
        sprite_index(*tileSprite, tiles_word_tiles_outline + frame * 9);
        font_sprite(s, ch, 100 + letter * 24, y + row * 24, frame);
    } else {
        sprite_index(*tileSprite, tiles_word_tiles_filled + frame * 9)
        font_sprite(s, ch, 100 + letter * 24, y + row * 24, frame);
    }

    if (step == 0) {
        if (frame == 3) { step = 1; }
        else {
            frame++;
        }
    } else {
        if(frame == 0) {
            // Add tile
            draw_tile(12 + letter * 3, row * 3, check_result[letter], 0);

            step = 0;
            letter ++;
            if(letter == 5) {
                is_animating_guess = false;
            }
        } else {
            frame--;
        }
    }
}

void animate_guess(int8_t guess_row) {
    row = guess_row;
    letter = 0;
    step = 0;
    frame = 0;
    is_animating_guess = true;
    delay = 0;

    tileSprite = &sprites[40];
    sprite_size(*tileSprite, 3, 3);
    sprite_pri(*tileSprite, 0);
}
