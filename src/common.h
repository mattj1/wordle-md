//
// Created by Matt on 2/22/2022.
//

#ifndef WORDLE_MD_COMMON_H
#define WORDLE_MD_COMMON_H

#include <genesis.h>

#define cursor_sprite 30

extern const uint32_t TILES[];
extern const char WORDS[];

extern char guess[6][5];

extern uint8_t is_animating_invalid_word, is_animating_guess;

#define GAME_STATE_GAME 0
#define GAME_STATE_GAMEOVER 1

extern int game_state;

extern s16 hscroll[];

void clear_screen();

extern int current_word;

extern int tiles_cursor;
extern int tiles_word_tiles_filled;
extern int tiles_word_tiles_outline;
extern int tiles_large_font;
extern int tiles_misc;

extern char check_result[5];

void font_print_string_to_tilemap(int x, int y, VDPPlane plane, const char *string);
int font_sprite_index(char letter, int frame);
void font_sprite(int sprite, char letter, s16 x, s16 y, int frame);

void sprite_to_tilemap(u16 index, int x, int y, int sprite_width, int sprite_height, u16 base_attr, VDPPlane plane);
void draw_tile(int x, int y, int t, int frame, VDPPlane plane);

inline u16 letter_sprite(int row, int col) {
    return col + row * 5;
}

inline void letter_sprite_location(s16 row, s16 col, s16 *x, s16 *y) {
    *x = 100 + col * 24;
    *y = 4 + row * 24;
}

//extern void animate_invalid_word(int8_t row);
void update_invalid_word_animation();

void update_guess_animation();

void animate_invalid_word(int8_t row);

void animate_guess(int row);

void graphics_load();

void cursor_show();
void cursor_hide();

void bottom_out();
void bottom_in();

void InstructionsScreen();
void GameScreen();
void TitleScreen();


void GameOver_Start(bool didWin, const char *message);
void GameOver_Update();
extern int is_animating_gameover;

extern uint16_t blackPal[];
extern uint16_t mainPal[];

void menu_init(VDPPlane plane);
void menu_add(int x, int y, const char *label);
void menu_draw();
int menu_run();


#define SCREEN_TITLE 0
#define SCREEN_GAME 1
#define SCREEN_INSTRUCTIONS 2
#define SCREEN_NEWGAME 3

extern int next_screen;


void sound_init();
void sound_play_click();
void sound_play_flip();
void sound_play_ui_click();
void sound_play_ping();

extern int cursorSpritePos[2];

#endif //WORDLE_MD_COMMON_H
