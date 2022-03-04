#include <genesis.h>
#define TILESET_CURSOR 0
#define TILESET_WORD_TILES_FILLED 1
#define TILESET_WORD_TILES_OUTLINE 2
#define TILESET_MISC 3
#define TILESET_LARGE_FONT 4

            typedef struct tileset_s tileset_t;
            
            struct tileset_s {
                int start_tile;
                int num_tiles;
            };
            
            extern tileset_t tile_sets[5];
        
