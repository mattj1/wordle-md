#include "common.h"

uint8_t is_animating_invalid_word;


static int8_t invalid_row, invalid_frame;


void update_invalid_word_animation() {
    invalid_frame++;
    int i = invalid_frame;

    int16_t offset = ((i + 2) % 8);
    if (offset > 4) offset = 4 - (offset - 4);

    for (int j = 0; j < 5; j++) {
        VDPSprite *s = letter_sprite(invalid_row, j);
        set_letter_sprite_location(s, invalid_row, j);
        s->x += offset - 2;
    }

    for (int j = 0; j < 3; j++) {
        hscroll[j + invalid_row * 3] = offset - 2;
    }

    vdp_hscroll_tile(VDP_PLAN_B, hscroll);

//    for(int k = 0; k < 41; k++) vdp_sprite_add(&sprites[k]);

    if (invalid_frame == 24) {
        is_animating_invalid_word = 0;
    }
}

void animate_invalid_word(int8_t row) {
    invalid_row = row;
    invalid_frame = 0;
    is_animating_invalid_word = true;
}

