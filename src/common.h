//
// Created by Matt on 2/22/2022.
//

#ifndef WORDLE_MD_COMMON_H
#define WORDLE_MD_COMMON_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "vdp.h"

extern const uint32_t TILES[];

extern char guess[6][5];

extern uint8_t is_animating_invalid_word, is_animating_guess;

extern VDPSprite sprites[];
extern int16_t hscroll[];

extern int tiles_cursor;
extern int tiles_word_tiles_filled;
extern int tiles_word_tiles_outline;
extern int tiles_large_font;
extern int tiles_misc;

extern char check_result[5];

void font_sprite(VDPSprite *sprite, char letter, int x, int y, int frame);

void draw_tile(int x, int y, int t, int frame);

inline VDPSprite *letter_sprite(int row, int col) {
    return &sprites[col + row * 5];
}

inline void set_letter_sprite_location(VDPSprite *sprite, int row, int col) {
    sprite_pos(*sprite, 100 + col * 24, 4 + row * 24);
}


//extern void animate_invalid_word(int8_t row);
void update_invalid_word_animation();

void update_guess_animation();

void animate_invalid_word(int8_t row);

void animate_guess(int8_t row);

void graphics_load();

#endif //WORDLE_MD_COMMON_H
