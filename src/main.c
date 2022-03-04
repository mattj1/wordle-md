#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#define hard_reset() __asm__("move   #0x2700,%sr\n\t" \
                             "move.l (0),%a7\n\t"     \
                             "jmp    _hard_reset")

#define enable_ints __asm__("move #0x2500,%sr")
#define disable_ints __asm__("move #0x2700,%sr")

#include "common.h"
#include "string.h"
#include "res_generated.h"

const char WORDS[64735]; //12947 * 5
extern const uint16_t WORDLE_WORDS[];


extern const uint32_t FONT_TILES[];
extern const uint16_t WORD_OFFSETS[];

int next_screen = SCREEN_TITLE;

int message_time;

char check_result[5];

int cursorSpritePos[2];

const char *kbRows[3] = {
        "QWERTYUIOP",
        "ASDFGHJKL ",
        " ZXCVBNM  "
};

u8 kbState[26];
int cursor_row, cursor_col;

int game_state;
char guess[6][5];
int cur_guess, guess_x;

int current_word;

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
    const char *word = &WORDS[j * 5];
    while (1) {
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
    u8 checked = 0;

    const char *word = &WORDS[current_word * 5];
    const char *g = &guess[cur_guess][0];

    // Check for correct letters
    for (int i = 0; i < 5; i++) {
        check_result[i] = 0;
        if (g[i] == word[i]) {
            check_result[i] = 2;
            checked |= (1 << i);
        }
    }

    if(checked == 0x1f) {
        return 1;
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

    char buf[10];
//    sprintf(buf, "%d%d%d%d%d", check_result[0], check_result[1], check_result[2], check_result[3], check_result[4]);
    return 0;
}

void set_key_state(int k, uint8_t state) {
    if (kbState[k] < state) {
        kbState[k] = state;
    }
}

void message_clear() {
    VDP_fillTileMapRect(BG_A, 0, 0, 19, 40, 1);
}

void message_draw(const char *message) {
    int l = strlen(message);

    message_time = 120;
    VDP_drawTextBG(BG_A, message, 20 - (l >> 1), 19);
}


void add_letter_to_guess(char ch) {

    if (guess_x == 5)
        return;

    sound_play_click();

    guess[cur_guess][guess_x] = ch;

    u16 s = letter_sprite(cur_guess, guess_x);

    s16 x, y;
    letter_sprite_location( cur_guess, guess_x, &x, &y);
    VDP_setSpritePosition(s, x, y);

    font_sprite(s, ch, 100 + guess_x * 24, 4 + cur_guess * 24, 0);

    guess_x++;
}


void draw_keyboard() {
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 10; col++) {
            int ch = kbRows[row][col];

            uint16_t attr = TILE_ATTR_FULL(kbState[ch - 'A'], 0, 0, 0, TILE_FONTINDEX + ch - 32);
            VDP_setTileMapXY(BG_B, attr, 11 + col * 2, 21 + row * 2);
        }
    }
}


void draw_cursor() {
    cursorSpritePos[0] = 84 + cursor_col * 16;
    cursorSpritePos[1] = 164 + cursor_row * 16;

    VDP_setSpritePosition(cursor_sprite, cursorSpritePos[0], cursorSpritePos[1]);
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

    if(cur_guess == 0) {
        // Pick the word at this point, so it's likely to be very random
        current_word = WORDLE_WORDS[random() % 2315];
    }

    if(check_guess()) {
        GameOver_Start(1, "NICE");
    } else {
        if(cur_guess == 5)  {
            GameOver_Start(0, "TOO BAD");
        }
    }

    animate_guess(cur_guess);

    for (int i = 0; i < 5; i++) {
        int ch = guess[cur_guess][i] - 'A';
        set_key_state(ch, check_result[i] + 1);
    }

    draw_keyboard();

    guess_x = 0;
    cur_guess++;
}

int repeatFrames = 0;

void cursor_up() {
    if (cursor_row > 0) cursor_row--;
}

void cursor_down() {
    if (repeatFrames > 0) {
        if (
                (cursor_row == 1 && cursor_col == 0) ||
                (cursor_row == 1 && cursor_col == 8)) {
            return;
        }
    }

    if (cursor_row < 2) cursor_row++;

    if (cursor_row == 1) {
        if (cursor_col == 9) {
            cursor_col--;
        }
    } else if (cursor_row == 2) {
        if (cursor_col == 8) {
            cursor_col--;
        } else if (cursor_col == 0) {
            cursor_col++;
        }
    }
}

void cursor_left() {
    if(repeatFrames > 0) {
        switch(cursor_row) {
            case 0:
            case 1:
                if (cursor_col > 0) cursor_col--;
            break;
            case 2:
                if (cursor_col > 1) cursor_col--;
                break;
            default:
                break;
        }

        return;
    }

    if (cursor_col > 0) cursor_col--;

    if (cursor_row == 2 && cursor_col == 0) {
        cursor_row --;
    }
}

void cursor_right() {

    if(repeatFrames > 0) {
        switch(cursor_row) {
            case 0:
                if (cursor_col < 9) cursor_col++;
                break;
            case 1:
                if (cursor_col < 8) cursor_col++;
                break;
            case 2:
                if(cursor_col < 7) cursor_col ++;
                break;
            default:
                break;
        }

        return;
    }

    if (cursor_col < 9) cursor_col++;

    if((cursor_row == 2 && cursor_col == 8) || (cursor_row == 1 && cursor_col == 9)) {
        cursor_row --;
    }
}


void GameMenu() {
    if(is_animating_invalid_word || is_animating_gameover || is_animating_guess)
        return;

    // TODO: remove cursor

    bottom_out();

    VDP_clearTextAreaBG(BG_B, 0,20,40,8);

    menu_init(BG_B);
    menu_add(16, 20, "Continue");
    menu_add(16, 22, "Give Up");
    menu_add(16, 24, "Quit");
    menu_draw();

    bottom_in();

    int choice = menu_run();

    switch(choice) {
        case 0:
            bottom_out();
            draw_keyboard();
            cursor_show();
            bottom_in();
            break;
        case 1:
            if(cur_guess == 0) {
                // Pick a random word now... doesn't matter.
                current_word = WORDLE_WORDS[random() % 2315];
            }

            GameOver_Start(0, "GAME OVER");
            break;
        case 2:
            next_screen = SCREEN_TITLE;
            break;
        default:
            break;
    }
}

void GameScreen() {

    clear_screen();

    VDP_setScrollingMode(HSCROLL_TILE, VSCROLL_PLANE);
    for (int i = 0; i < 32; i++) hscroll[i] = 0;
    VDP_setHorizontalScrollTile(BG_A, 0, hscroll, 32, DMA_QUEUE);

    int y = 4;

    u16 joystate = ~0, oldstate;

    // Keyboard
    cursor_row = cursor_col = 0;
    memset(kbState, 0, sizeof(kbState));

    // Game
    game_state = GAME_STATE_GAME;
    memset(guess, 0, sizeof(guess));
    cur_guess = guess_x = 0;

    VDP_linkSprites(0, 50);

    cursor_show();

    for (int j = 0; j < 6; j++) {
        for (int i = 0; i < 5; i++) {
            draw_tile(12 + i * 3, j * 3, 3, 0, BG_A);
        }
    }
//
//    add_letter_to_guess('C');
//    add_letter_to_guess('R');
//    add_letter_to_guess('A');
//    add_letter_to_guess('N');
//    add_letter_to_guess('E');

    draw_keyboard();
    draw_cursor();

    VDP_updateSprites(42, TRUE);
    SYS_doVBlankProcess();

    PAL_fadeInAll(mainPal, 15, 0);

    while (1) {
        JOY_update();
        oldstate = joystate;
        joystate = JOY_readJoypad(JOY_1);

        // Is any d-pad button down?
        if (joystate & 0xF) {

            if ((joystate & 0xF) ^ (oldstate & 0xF)) {
                // A d-pad button was pressed, so reset the repeat count
                repeatFrames = 0;
            } else {
                // No change
                repeatFrames ++;
            }

            u16 pressed = 0;

            for(int i = 0; i < 4; i++) {
                if ((joystate & (1 << i)) && !(oldstate & (1 << i))) pressed |= (1 << i);

                if(repeatFrames > 10 && (repeatFrames % 3) == 0) {
                    if (joystate & (1 << i)) pressed |= (1 << i);
                }
            }

            if (pressed & BUTTON_UP) {
                cursor_up();
            }

            if (pressed & BUTTON_DOWN) {
                cursor_down();
            }

            if (pressed & BUTTON_LEFT) {
                cursor_left();
            }

            if (pressed & BUTTON_RIGHT) {
                cursor_right();
            }
        }


        if(game_state != GAME_STATE_GAMEOVER) {

            if ((joystate & BUTTON_A) && !(oldstate & BUTTON_A)) {
                GameMenu();
            }

            if ((joystate & BUTTON_B) && !(oldstate & BUTTON_B)) {
                if (guess_x > 0) {
                    guess_x--;
                    guess[cur_guess][guess_x] = 0;
                    int s = guess_x + cur_guess * 5;
                    font_sprite(s, 0, 100 + guess_x * 24, y + cur_guess * 24, 0);
                }
            }

            if ((joystate & BUTTON_C) && !(oldstate & BUTTON_C)) {
                char ch = kbRows[cursor_row][cursor_col];
                add_letter_to_guess(ch);
            }

            if ((joystate & BUTTON_START) && !(oldstate & BUTTON_START)) {
                enter_guess();
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

        if(is_animating_gameover) {
            GameOver_Update();
        }

        draw_cursor();
        VDP_updateSprites(42, DMA_QUEUE);
        SYS_doVBlankProcess();

        random();

        if(next_screen != SCREEN_GAME) {
            VDP_fadeOutAll(30, 0);
            return;
        }
    }
}


int main() {

    VDP_init();
    JOY_init();
    sound_init();

    enable_ints;

//    sound_init();

    // gray tile : #3a3a3c
    // green tile: #538e4e
    // orange tile: #b59f3a

    VDP_setPlaneSize(64, 64, TRUE);

//	vdp_color(0, 0x080);

//
//    VDP_setPaletteColor(0, RGB24_TO_VDPCOLOR(0x121213));
//    VDP_setPaletteColor(1, RGB24_TO_VDPCOLOR(0));
//    VDP_setPaletteColor(2, RGB24_TO_VDPCOLOR(0x3a3a3c));
//    VDP_setPaletteColor(3, RGB24_TO_VDPCOLOR(0x538e4e));
//    VDP_setPaletteColor(4, RGB24_TO_VDPCOLOR(0xb59f3a));
//
//    VDP_setPaletteColor(15, RGB24_TO_VDPCOLOR(0xffffff));
//
//    VDP_setPaletteColor(16 + 15, RGB24_TO_VDPCOLOR(0x999999)); // dark gray keyboard
//
//    VDP_setPaletteColor(32 + 15, RGB24_TO_VDPCOLOR(0xb59f3a)); // orange keyboard
//
//    VDP_setPaletteColor(48 + 15, RGB24_TO_VDPCOLOR(0x538e4e)); // green keyboard

    graphics_load();

    while(1) {
        switch(next_screen) {
            case SCREEN_TITLE:
                TitleScreen();
                break;
            case SCREEN_GAME:
                GameScreen();
                break;
            case SCREEN_INSTRUCTIONS:
                InstructionsScreen();
                break;
            case SCREEN_NEWGAME:
                next_screen = SCREEN_GAME;
                break;
        }
    }

}

#pragma clang diagnostic pop