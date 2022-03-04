#include "common.h"

// scroll offsets
static s16 planeX = 0, planeY = 0;
static int do_update_keyboard_remove_animation;
static int do_message_scroll = 0;
static int did_win = 0;
static int message_state = 0, message_step = 0;
static int frame = 0;
int is_animating_gameover = 0;

static u8 green_amount = 0;

static const char *message;

static void start_message(int step) {
    planeX = 320 - 32;
    planeY = 0;
    do_message_scroll = 1;
    message_state = 0;
    message_step = 0;
    frame = 0;

    message_step = step;
    char buf[6];

    switch(step) {
        case 0:
            font_print_string_to_tilemap(20 - (strlen(message)), 21, BG_B,  message);
        break;
        case 1:
            if(did_win) {
                do_message_scroll = 0;
                return;
            }

            VDP_clearPlane(BG_B, TRUE);
            VDP_drawTextBG(BG_B, "THE WORD WAS", 14, 20);

            for(int i = 0; i < 5; i++) {
                buf[i] = WORDS[current_word * 5 + i];
            }

            buf[5] = 0;

            font_print_string_to_tilemap(15, 22, BG_B, buf);
            break;
        case 2:
            do_message_scroll = 0;
            break;
    }
}

static void update_keyboard_remove_animation() {
    planeY -= 4;

    if(planeY == -64) {
        VDP_clearTextAreaBG(BG_B, 0,20,40,8);
        do_update_keyboard_remove_animation = 0;

        start_message(0);
    }
}

void set_message_state(int state) {
    message_state = state;

    // Show the menu

    if(state == 2) {
        if(!did_win && message_step == 1) {
            do_message_scroll = 0;
        } else if (did_win && message_step == 0) {
            do_message_scroll = 0;
        }
    }
}

void update_message_scroll() {
    switch (message_state) {
        case 0:
            if (planeX > 0) {
                planeX -= 8;
            } else set_message_state(1);
            break;
        case 1:
            if (frame < 60) frame++;
            else set_message_state(2);
            break;
        case 2:
            if (planeX > -200 - 32) {
                planeX -= 8;
            } else {
                set_message_state(3);
            }
            break;
    }
}

void Gameover_Menu() {
    menu_init(BG_B);
    menu_add(16, 25, "Continue");
    menu_draw();
    menu_run();

    bottom_out();


    menu_init(BG_B);
    menu_add(16, 22, "Play again");
    menu_add(16, 24, "Quit");
    menu_draw();

    bottom_in();

    switch(menu_run()) {
        case 0:
            next_screen = SCREEN_NEWGAME;
            break;
        case 1:
            next_screen = SCREEN_TITLE;
            break;
    }
}

void GameOver_Start(bool didWin, const char *gameover_message) {
    message = gameover_message;
    game_state = GAME_STATE_GAMEOVER;
    is_animating_gameover = 1;
    did_win = didWin;
    green_amount = 0;
    do_update_keyboard_remove_animation = 1;
    do_message_scroll = 0;

    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

    do_update_keyboard_remove_animation = 1;

    cursor_hide();

    if(did_win) {
        VDP_setPaletteColor(0, RGB24_TO_VDPCOLOR(0x110011));
    }
}

void GameOver_Update() {

    if(green_amount < 96) {
        green_amount += 4;
    }

    if(do_update_keyboard_remove_animation) {
        update_keyboard_remove_animation();
    }

    if(do_message_scroll) {
        update_message_scroll();

        if(!do_message_scroll) {
            Gameover_Menu();

            is_animating_gameover = 0;
            return;
        }

        if(message_state == 3) {
            start_message(message_step + 1);
        }
    }

    VDP_setVerticalScroll(BG_B, planeY);
    VDP_setHorizontalScroll(BG_B, planeX);

    if(did_win) {
        VDP_setPaletteColor(0, RGB24_TO_VDPCOLOR(0x110011 | (green_amount << 8)));
    }
}
