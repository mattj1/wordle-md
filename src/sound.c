#include "common.h"
#include "../res/audio.h"

void sound_init() {
    XGM_setPCM(65, pcm_tinyclick, sizeof(pcm_tinyclick));
    XGM_setPCM(66, pcm_flip, sizeof(pcm_flip));
    XGM_setPCM(67, pcm_menuclick, sizeof(pcm_menuclick));
    XGM_setPCM(68, pcm_ping, sizeof(pcm_ping));
}

void sound_play_click() {
    XGM_startPlayPCM(65, 1, SOUND_PCM_CH3);
}

void sound_play_flip() {
    XGM_startPlayPCM(66, 1, SOUND_PCM_CH3);
}

void sound_play_ui_click() {
//    XGM_startPlayPCM(67, 1, SOUND_PCM_CH3);
}

void sound_play_ping() {
//    XGM_startPlayPCM(68, 1, SOUND_PCM_CH3);
}