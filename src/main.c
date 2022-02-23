#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#define hard_reset() __asm__("move   #0x2700,%sr\n\t" \
                             "move.l (0),%a7\n\t"     \
                             "jmp    _hard_reset")

#define enable_ints __asm__("move #0x2500,%sr")
#define disable_ints __asm__("move #0x2700,%sr")


#include "common.h"

#include "joy.h"
#include "string.h"
#include "res_generated.h"

extern const char WORDS[12947][5];
extern const uint16_t WORDLE_WORDS[];


extern const uint32_t FONT_TILES[];
extern const uint16_t WORD_OFFSETS[];


int message_time;

uint16_t joystate = ~0, oldstate;

VDPSprite sprites[80];

char check_result[5];

const char *kbRows[3] = {
        "QWERTYUIOP",
        "ASDFGHJKL ",
        " ZXCVBNM  "
};

uint8_t kbState[26];
int cursor_row = 0, cursor_col = 0;

int current_word;

char guess[6][5];
int8_t cur_guess = 0;
int guess_x = 0;

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

void message_clear() {
    vdp_map_fill_rect(VDP_PLAN_A, 0, 0, 19, 40, 1, 0);
}

void message_draw(const char *message) {
    int l = strlen(message);

    message_time = 120;
//    vdp_map_fill_rect(VDP_PLAN_A, 31, 20 - ((l + 4) >> 1), 8, l + 4, 3, 0);

    vdp_puts(VDP_PLAN_A, message, 20 - (l >> 1), 19);
}


void add_letter_to_guess(char ch) {
    int y = 4; //todo

    if (guess_x == 5)
        return;

    guess[cur_guess][guess_x] = ch;

    VDPSprite *s = letter_sprite(cur_guess, guess_x);
    set_letter_sprite_location(s, cur_guess, guess_x);

    font_sprite(s, ch, 100 + guess_x * 24, y + cur_guess * 24, 0);

    guess_x++;
}

void enter_guess() {

    if(is_animating_guess || is_animating_invalid_word)
        return;

    if (guess_x != 5)
        return;

    if (!is_valid_guess(&guess[cur_guess][0])) {
        message_draw("Not a valid word");
        animate_invalid_word(cur_guess);
        return;
    }

    int accepted = check_guess();

    if (accepted) {
        animate_guess(cur_guess);

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

    VDPSprite *cursor_sprite = &sprites[30];

    int y = 4;

//    draw_tile(12, 0, 0);

    cursor_sprite->attr = tiles_cursor;
    sprite_pos(*cursor_sprite, cursor_x, 64 + 8);
    sprite_size(*cursor_sprite, 2, 2);

    for (int j = 0; j < 6; j++) {
        for (int i = 0; i < 5; i++) {
            draw_tile(12 + i * 3, j * 3, 3, 0);
        }
    }

    add_letter_to_guess('C');
    add_letter_to_guess('R');
    add_letter_to_guess('A');
    add_letter_to_guess('N');
    add_letter_to_guess('E');

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
                font_sprite(s, 0, 100 + guess_x * 24, y + cur_guess * 24, 0);
            }
        }

        if ((joystate & BUTTON_C) && !(oldstate & BUTTON_C)) {
            char ch = kbRows[cursor_row][cursor_col];
            add_letter_to_guess(ch);
        }

        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 10; col++) {
                int ch = kbRows[row][col];
                uint16_t attr = TILE_ATTR(kbState[ch - 'A'], 0, 0, 0, TILE_FONTINDEX + ch - 32);
                vdp_map_xy(VDP_PLAN_A, attr, 11 + col * 2, 21 + row * 2);
            }
        }

        if (message_time) {
            message_time--;
            if (message_time == 0) {
                message_clear();
            }
        }

        if (is_animating_invalid_word) {
            update_invalid_word_animation();
        }

        if (is_animating_guess) {
            update_guess_animation();
        }

        sprite_pos(*cursor_sprite, 84 + cursor_col * 16, 164 + cursor_row * 16);

        for (int k = 0; k < 41; k++) vdp_sprite_add(&sprites[k]);
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
    vdp_set_scrollmode(2, 0);
    for (int i = 0; i < 32; i++) hscroll[i] = 0;
    vdp_hscroll_tile(VDP_PLAN_B, hscroll);

    vdp_color(0, RGB24_TO_VDPCOLOR(0x121213)); // #
    vdp_color(1, RGB24_TO_VDPCOLOR(0));
    vdp_color(2, RGB24_TO_VDPCOLOR(0x3a3a3c));
    vdp_color(3, RGB24_TO_VDPCOLOR(0x538e4e));
    vdp_color(4, RGB24_TO_VDPCOLOR(0xb59f3a));

    vdp_color(15, RGB24_TO_VDPCOLOR(0xffffff));

    vdp_color(16 + 15, RGB24_TO_VDPCOLOR(0x999999)); // dark gray keyboard

    vdp_color(32 + 15, RGB24_TO_VDPCOLOR(0xb59f3a)); // orange keyboard

    vdp_color(48 + 15, RGB24_TO_VDPCOLOR(0x538e4e)); // green keyboard

    graphics_load();

    vdp_puts(VDP_PLAN_A, "Press START", 8, 16);

    while (true) {
        rand();

        joy_update();

        oldstate = joystate;
        joystate = joy_get_state(JOY_1);
        GameScreen();

        if ((joystate & BUTTON_START) && !(oldstate & BUTTON_START)) {
//            GameScreen();
        }


        vdp_vsync();
    }

    return 0;
}

#pragma clang diagnostic pop