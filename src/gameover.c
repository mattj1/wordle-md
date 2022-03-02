#include "common.h"

// scroll offsets
static s16 planeX = 0, planeY = 0;
static int do_update_keyboard_remove_animation;
static int do_message_scroll = 0;

int is_animating_gameover = 0;

static void update_keyboard_remove_animation() {
    planeY -= 4;

    // 11 + col * 2, 21 + row * 2

    if(planeY == -32) {
        VDP_clearTextAreaBG(BG_B, 11, 25, 22, 1);
    }

    if(planeY == -40) {
        VDP_clearTextAreaBG(BG_B, 11, 23, 22, 1);
    }

    if(planeY == -48) {
        VDP_clearTextAreaBG(BG_B, 11, 21, 22, 1);
    }

    if(planeY == -64) {
        do_update_keyboard_remove_animation = 0;
        planeX = 320 - 32;
        planeY = 0;
        do_message_scroll = 1;

        font_print_string_to_tilemap(12, 22, BG_B, "YOU WIN");
    }
}

void update_message_scroll() {
    if(planeX > 0) {
        planeX -= 8;
    }
}

void GameOver_Start(bool didWin) {
    is_animating_gameover = 1;

    do_update_keyboard_remove_animation = 1;
    do_message_scroll = 0;

    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

    do_update_keyboard_remove_animation = 1;

    // Remove the cursor
    VDP_setSprite(cursor_sprite, 0,0, SPRITE_SIZE(1,1), TILE_ATTR_FULL(0, 0, 0, 0, 0));
}

void GameOver_Update() {

    if(do_update_keyboard_remove_animation) {
        update_keyboard_remove_animation();
    }

    if(do_message_scroll) {
        update_message_scroll();
    }

    VDP_setVerticalScroll(BG_B, planeY);
    VDP_setHorizontalScroll(BG_B, planeX);
}
