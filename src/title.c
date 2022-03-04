#include "common.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"


void TitleScreen() {
    PAL_setColors(0, blackPal, 64);
    clear_screen();

    font_print_string_to_tilemap(11, 6, BG_B, "WORDLE MD");

    VDP_drawTextBG(BG_A, "github.com/mattj1", 20, 25);
    VDP_drawTextBG(BG_A, "v0.1", 33, 27);
    menu_init(BG_A);
    menu_add(8, 14, "Play");
    menu_add(8, 16, "Instructions");
    menu_draw();

    PAL_fadeInAll(mainPal, 15, 0);

    int choice = menu_run();

    switch (choice) {
        case 0:
            next_screen = SCREEN_GAME;
            break;
        case 1:
            next_screen = SCREEN_INSTRUCTIONS;
            break;
    }

    PAL_fadeOutAll(15, 0);
}

#pragma clang diagnostic pop