#include "common.h"

typedef struct menu_item_s menu_item_t;


static int num_items, selected_item;

static VDPPlane plane;

struct menu_item_s {
    int x;
    int y;
    const char *label;
};

#define MAX_MENU_ITEMS 3

menu_item_t menu_items[MAX_MENU_ITEMS];

void menu_init(VDPPlane p) {
    memset(menu_items, 0, sizeof(menu_items));
    num_items = 0;
    selected_item = 0;
    plane = p;
}

void menu_add(int x, int y, const char *label) {
    if(num_items == MAX_MENU_ITEMS)
        return;

    menu_item_t *item = &menu_items[num_items ++];

    item->x = x;
    item->y = y;
    item->label = label;
}

void menu_draw() {
    for(int i = 0; i < num_items; i++) {
        menu_item_t *item = &menu_items[i];
        VDP_drawTextBG(plane, item->label, item->x, item->y);
        VDP_drawTextBG(plane, selected_item == i ? ">" : " ", item->x - 2, item->y);
    }
}

int menu_run() {
    uint16_t joystate = ~0, oldstate;

    while (1) {

        random();

        JOY_update();

        oldstate = joystate;
        joystate = JOY_readJoypad(JOY_1);

        if ((joystate & BUTTON_UP) && !(oldstate & BUTTON_UP)) {
            if(selected_item > 0) {
                selected_item --;
                sound_play_ui_click();
                menu_draw();
            }
        }

        if ((joystate & BUTTON_DOWN) && !(oldstate & BUTTON_DOWN)) {
            if(selected_item < num_items - 1) {
                selected_item ++;
                sound_play_ui_click();
                menu_draw();
            }
        }

        if ((joystate & BUTTON_START) && !(oldstate & BUTTON_START)) {
            break;
        }

        if ((joystate & BUTTON_A) && !(oldstate & BUTTON_A)) {
            break;
        }

        if ((joystate & BUTTON_C) && !(oldstate & BUTTON_C)) {
            break;
        }

        SYS_doVBlankProcess();
    }

    sound_play_ping();
    return selected_item;
}
