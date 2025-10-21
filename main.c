#include <stdio.h>
#include <string.h>
#include "game.h"
#include "io.h"

int main(int argc, char **argv) {
    GameState g;
    init_standard(&g);

    if (argc >= 2) {
        if (strcmp(argv[1], "--load") == 0 && argc >= 3) {
            if (!load_game(argv[2], &g)) {
                printf("Failed to load %s\n", argv[2]);
            } else {
                printf("Loaded %s\n", argv[2]);
            }
        }
    }

    printf("Bitboard Checkers (console)\n");
    printf("Enter moves like: e3-d4  or  12-21\n");
    prompt_loop(&g);

    return 0;
}
