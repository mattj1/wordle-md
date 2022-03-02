#include "common.h"

void InstructionsScreen() {
    VDP_clearPlane(VDP_PLAN_A, 0);

    PAL_setColors(0, blackPal, 64);

    menu_init();
    menu_add(8, 19, "BACK", 0);
    menu_draw();

    VDP_drawTextBG(VDP_PLAN_A, "Instructions", 4, 4);
    VDP_drawTextBG(VDP_PLAN_A, "------------", 4, 5);

    VDP_drawTextBG(VDP_PLAN_A, "Use the D-Pad to move the cursor.", 4, 7);
    VDP_drawTextBG(VDP_PLAN_A, "A - Options", 4, 9);
    VDP_drawTextBG(VDP_PLAN_A, "B - Backspace", 4, 11);
    VDP_drawTextBG(VDP_PLAN_A, "C - Pick letters", 4, 13);

    VDP_drawTextBG(VDP_PLAN_A, "Press Start to guess the word.", 4, 15);
    PAL_fadeInAll(mainPal, 15, 0);

    menu_run();

    PAL_fadeOutAll(15, 0);
}