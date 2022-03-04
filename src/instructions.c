#include "common.h"

void InstructionsScreen() {
    clear_screen();

    PAL_setColors(0, blackPal, 64);

    menu_init(BG_A);
    menu_add(8, 19, "Back");
    menu_draw();

    VDP_drawTextBG(BG_A, "Instructions", 4, 4);
    VDP_drawTextBG(BG_A, "------------", 4, 5);

    VDP_drawTextBG(BG_A, "Use the D-Pad to move the cursor.", 4, 7);
    VDP_drawTextBG(BG_A, "A - Options", 4, 9);
    VDP_drawTextBG(BG_A, "B - Backspace", 4, 11);
    VDP_drawTextBG(BG_A, "C - Pick letters", 4, 13);

    VDP_drawTextBG(BG_A, "Press Start to guess the word.", 4, 15);
    PAL_fadeInAll(mainPal, 15, 0);

    menu_run();

    next_screen = SCREEN_TITLE;

    PAL_fadeOutAll(15, 0);
}