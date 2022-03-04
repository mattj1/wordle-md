#include "common.h"

uint8_t is_animating_invalid_word;


static int invalid_row, invalid_frame;


void update_invalid_word_animation() {
    invalid_frame++;
    u8 i = invalid_frame;

    s16 offset = ((i + 2) % 8);
    if (offset > 4) offset = 4 - (offset - 4);

    for (int j = 0; j < 5; j++) {
        s16 x, y;
        letter_sprite_location( invalid_row, j, &x, &y);
        x += offset - 2;
        VDP_setSpritePosition(letter_sprite(invalid_row, j), x, y);
    }

    for (int j = 0; j < 3; j++) {
        hscroll[j + invalid_row * 3] = offset - 2;
    }

    VDP_setHorizontalScrollTile(BG_A, 0, hscroll, 32, DMA);

    if (invalid_frame == 24) {
        is_animating_invalid_word = 0;
    }
}

void animate_invalid_word(int8_t row) {
    invalid_row = row;
    invalid_frame = 0;
    is_animating_invalid_word = 1;
}

