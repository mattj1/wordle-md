#include "common.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

void TitleScreen() {

    while (1) {
        VDP_clearPlane(VDP_PLAN_A, 0);
        PAL_setColors(0, blackPal, 64);

        menu_init();
        menu_add(8, 16, "PLAY", 0);
        menu_add(8, 18, "INSTRUCTIONS", 0);
        menu_draw();

        font_print_string_to_tilemap(9, 10, BG_B, "WORDLE MD");
//
//        VDP_drawTextBG(VDP_PLAN_A, "WORDLE-MD", 8, 10);

        PAL_fadeInAll(mainPal, 15, 0);

        int choice = menu_run();

        switch (choice) {
            case 0:
                GameScreen();
                break;
            case 1:
                InstructionsScreen();
                break;
        }
    }
}

#pragma clang diagnostic pop