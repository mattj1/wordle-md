#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#define hard_reset() __asm__("move   #0x2700,%sr\n\t" \
                             "move.l (0),%a7\n\t"     \
                             "jmp    _hard_reset")

#define enable_ints __asm__("move #0x2500,%sr")
#define disable_ints __asm__("move #0x2700,%sr")

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "vdp.h"
#include "joy.h"

extern const char WORDS[12947][5];
extern const uint16_t WORDLE_WORDS[];
extern const uint32_t TILES[];

extern const uint32_t FONT_TILES[];
extern const uint16_t WORD_OFFSETS[];

char buf[6];

int message_time;

uint16_t joystate = ~0, oldstate;

VDPSprite sprites[80];

const char *kbRows[3] = {
        "QWERTYUIOP",
        "ASDFGHJKL ",
        " ZXCVBNM  "
};

uint8_t kbState[26];

int current_word;

char guess[6][5];
int cur_guess = 0;
int guess_x = 0;

int cursor_row = 0, cursor_col = 0;

void font_sprite(VDPSprite *sprite, char letter, int x, int y) {
    sprite->attr = TILE_USERINDEX + 4 + 9 * 4 + (letter - 'A') * 4;
    sprite_pos(*sprite, x, y);
    sprite_size(*sprite, 2, 2);
}

void draw_tile(int x, int y, int t) {
    int st = TILE_USERINDEX + 4 + t * 9;

    for (int i = 0; i < 3; i++) {
        vdp_map_xy(VDP_PLAN_B, st, x, y);
        st++;
        vdp_map_xy(VDP_PLAN_B, st, x, y + 1);
        st++;
        vdp_map_xy(VDP_PLAN_B, st, x, y + 2);
        st++;
        x++;
    }
}

int words_equal(const char *g, const char *word) {
    for (int i = 0; i < 5; i++) {
        if (*g++ != *word++) {
            return 0;
        }
    }

    return 1;
}


int is_valid_guess(const char *g) {
    int j = WORD_OFFSETS[g[0] - 'A'];
    const char *word = &WORDS[j][0];
    while (true) {
        if (g[0] != word[0]) {
            return 0;
        }

        if (words_equal(g, word)) {
            return 1;
        } else {
            word += 5;
        }
    }
}


char check_result[5];

int check_guess() {
    uint8_t checked = 0;

    const char *word = WORDS[current_word];
    const char *g = &guess[cur_guess][0];

//    vdp_puts(VDP_PLAN_A, "Check guess", 0, 0);
//    sprintf(buf, "%c%c%c%c%c", word[0], word[1], word[2], word[3], word[4]);


//    vdp_puts(VDP_PLAN_A, buf, 0, 1);
//    sprintf(buf, "%c%c%c%c%c", g[0], g[1], g[2], g[3], g[4]);
//    vdp_puts(VDP_PLAN_A, buf, 0, 2);

    // Check for correct letters
    for (int i = 0; i < 5; i++) {
        check_result[i] = 0;
        if (g[i] == word[i]) {
            check_result[i] = 2;
            checked |= (1 << i);
        }
    }

    for (int i = 0; i < 5; i++) {
        // check if letter is elsewhere in the word (unchecked)
        for (int j = 0; j < 5; j++) {
            if (checked & (1 << j))
                continue;

            if (word[j] == g[i]) {
                check_result[i] = 1;
                checked |= (1 << j);
                break;
            }
        }
    }

    //check_result[0] = 2;

//    sprintf(buf, "%d%d%d%d%d", check_result[0], check_result[1], check_result[2], check_result[3], check_result[4]);
//    vdp_puts(VDP_PLAN_A, buf, 0, 3);

    return 1;
}

void set_key_state(int k, uint8_t state) {
    if (kbState[k] < state) {
        kbState[k] = state;
    }
}

void clear_message() {
    vdp_map_fill_rect(VDP_PLAN_W, 0, 0, 20, 40, 1, 0);
}

void draw_message(const char *message) {
    message_time = 120;
    vdp_puts(VDP_PLAN_B, message, 16, 20);
}

void enter_guess() {

    if (guess_x != 5)
        return;

    if (!is_valid_guess(&guess[cur_guess][0])) {

        draw_message("Not a valid word");
        return;
    }

    int accepted = check_guess();

    for (int i = 0; i < 5; i++) {
        draw_tile(12 + i * 3, cur_guess * 3, check_result[i]);
    }

    if (accepted) {
        for (int i = 0; i < 5; i++) {
            int ch = guess[cur_guess][i] - 'A';
            set_key_state(ch, check_result[i] + 1);
        }

        guess_x = 0;
        cur_guess++;
    }
}

int GameScreen() {
    int cursor_x = 0;
    for (int i = 0; i < 26; i++) kbState[i] = 0;

    vdp_map_clear(VDP_PLAN_A);
    current_word = WORDLE_WORDS[rand() % 2315];

    vdp_vsync();
    vdp_tiles_load(&TILES[0], TILE_USERINDEX, 4 + 4 * 9 + 26 * 4);

    VDPSprite *cursor_sprite = &sprites[30];

    int y = 4;

//    draw_tile(12, 0, 0);

    cursor_sprite->attr = TILE_USERINDEX;
    sprite_pos(*cursor_sprite, cursor_x, 64 + 8);
    sprite_size(*cursor_sprite, 2, 2);

    for (int j = 0; j < 6; j++) {
        for (int i = 0; i < 5; i++) {
            draw_tile(12 + i * 3, j * 3, 3);
        }
    }

    while (true) {
        joy_update();
        oldstate = joystate;
        joystate = joy_get_state(JOY_1);

        if ((joystate & BUTTON_UP) && !(oldstate & BUTTON_UP)) {
            if (cursor_row > 0) cursor_row--;
        }

        if ((joystate & BUTTON_DOWN) && !(oldstate & BUTTON_DOWN)) {
            if (cursor_row < 2) cursor_row++;
        }

        if ((joystate & BUTTON_LEFT) && !(oldstate & BUTTON_LEFT)) {
            if (cursor_col > 0) cursor_col--;
        }

        if ((joystate & BUTTON_RIGHT) && !(oldstate & BUTTON_RIGHT)) {
            if (cursor_col < 9) cursor_col++;
        }

        if ((joystate & BUTTON_START) && !(oldstate & BUTTON_START)) {
            enter_guess();

        }

        if ((joystate & BUTTON_B) && !(oldstate & BUTTON_B)) {
            if (guess_x > 0) {
                guess_x--;
                guess[cur_guess][guess_x] = 0;
                VDPSprite *s = &sprites[guess_x + cur_guess * 5];
                font_sprite(s, 0, 100 + guess_x * 24, y + cur_guess * 24);
            }
        }

        if ((joystate & BUTTON_C) && !(oldstate & BUTTON_C)) {
            if (guess_x < 5) {
                char ch = kbRows[cursor_row][cursor_col];
                guess[cur_guess][guess_x] = ch;
                VDPSprite *s = &sprites[guess_x + cur_guess * 5];
                font_sprite(s, ch, 100 + guess_x * 24, y + cur_guess * 24);

                guess_x++;
            }
        }

        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 10; col++) {
                int ch = kbRows[row][col];
                uint16_t attr = TILE_ATTR(kbState[ch - 'A'], 0, 0, 0, TILE_FONTINDEX + ch - 32);
                vdp_map_xy(VDP_PLAN_A, attr, 11 + col * 2, 21 + row * 2);
            }
        }

        if(message_time) {
            message_time --;
            if(message_time == 0) {
                clear_message();
            }
        }

        sprite_pos(*cursor_sprite, 84 + cursor_col * 16, 164 + cursor_row * 16);

        vdp_sprites_add(sprites, 80);
        vdp_sprites_update();
        vdp_vsync();
    }
}

int main() {

    vdp_init();
    joy_init();
    enable_ints;
    // gray tile : #3a3a3c
    // green tile: #538e4e
    // orange tile: #b59f3a

//	vdp_color(0, 0x080);
    vdp_color(0, RGB24_TO_VDPCOLOR(0x121213)); // #
    vdp_color(1, RGB24_TO_VDPCOLOR(0));
    vdp_color(2, RGB24_TO_VDPCOLOR(0x3a3a3c));
    vdp_color(3, RGB24_TO_VDPCOLOR(0x538e4e));
    vdp_color(4, RGB24_TO_VDPCOLOR(0xb59f3a));

    vdp_color(15, RGB24_TO_VDPCOLOR(0xffffff));

    vdp_color(16 + 15, RGB24_TO_VDPCOLOR(0x999999)); // dark gray keyboard

    vdp_color(32 + 15, RGB24_TO_VDPCOLOR(0xb59f3a)); // orange keyboard

    vdp_color(48 + 15, RGB24_TO_VDPCOLOR(0x538e4e)); // green keyboard
    vdp_puts(VDP_PLAN_A, "Press START", 4, 4);

    while (true) {
        rand();

        joy_update();

        oldstate = joystate;
        joystate = joy_get_state(JOY_1);
//        GameScreen();

        if ((joystate & BUTTON_START) && !(oldstate & BUTTON_START)) {
            GameScreen();
        }


        vdp_vsync();
    }

    return 0;
}

#pragma clang diagnostic pop