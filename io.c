#include "io.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int parse_coord_or_index(const char *tok) {
    // try a1..h8
    if (strlen(tok) == 2 && isalpha((unsigned char)tok[0]) && isdigit((unsigned char)tok[1])) {
        char t0 = tolower((unsigned char)tok[0]);
        char t1 = tok[1];
        char s[3];
        s[0] = t0; s[1] = t1; s[2] = '\0';
        return coord_to_index(s);
    }
    // try integer index
    char *end;
    long v = strtol(tok, &end, 10);
    if (end != tok && *end == '\0' && v >= 0 && v < 64) return (int)v;
    return -1;
}

int parse_move_string(const char *s, int *from, int *to) {
    // trim
    while (*s && isspace((unsigned char)*s)) s++;
    char buf[128];
    strncpy(buf, s, sizeof(buf)-1); buf[sizeof(buf)-1] = '\0';
    // find separator '-'
    char *dash = strchr(buf, '-');
    if (!dash) return 0;
    *dash = '\0';
    char *a = buf;
    char *b = dash+1;
    // trim spaces
    while (*a && isspace((unsigned char)*a)) a++;
    char *ae = a + strlen(a) - 1;
    while (ae >= a && isspace((unsigned char)*ae)) { *ae = '\0'; ae--; }
    while (*b && isspace((unsigned char)*b)) b++;
    char *be = b + strlen(b) - 1;
    while (be >= b && isspace((unsigned char)*be)) { *be = '\0'; be--; }
    int ai = parse_coord_or_index(a);
    int bi = parse_coord_or_index(b);
    if (ai < 0 || bi < 0) return 0;
    *from = ai; *to = bi;
    return 1;
}

static void print_help() {
    puts("Commands:");
    puts("  move: e3-d4  or  12-21  (from-to)");
    puts("  save <file>  - save game");
    puts("  load <file>  - load game");
    puts("  quit         - exit");
    puts("  help         - this message");
}

void prompt_loop(GameState *g) {
    char line[256];
    while (1) {
        print_board(g);
        int winner = check_winner(g);
        if (winner != -1) {
            printf("Game over: %s wins!\n", winner==0? "Red":"Black");
            break;
        }
        printf("Enter command (help for commands): ");
        if (!fgets(line, sizeof(line), stdin)) break;
        // strip newline
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;
        if (strncmp(line, "quit", 4) == 0) break;
        if (strncmp(line, "help", 4) == 0) { print_help(); continue; }
        if (strncmp(line, "save ", 5) == 0) {
            char *fn = line + 5;
            if (save_game(fn, g)) printf("Saved to %s\n", fn);
            else printf("Failed to save to %s\n", fn);
            continue;
        }
        if (strncmp(line, "load ", 5) == 0) {
            char *fn = line + 5;
            if (load_game(fn, g)) printf("Loaded from %s\n", fn);
            else printf("Failed to load from %s\n", fn);
            continue;
        }
        int from, to;
        if (!parse_move_string(line, &from, &to)) {
            printf("Invalid command or move format.\n");
            continue;
        }
        int captured;
        if (apply_move(g, from, to, &captured)) {
            if (captured >= 0) {
                char buf[8];
                index_to_coord(captured, buf);
                printf("Captured piece at %s\n", buf);
            }
            // if capture and there is additional capture, allow chain moves: handled in apply_move (it keeps turn)
            continue;
        } else {
            printf("Illegal move.\n");
            continue;
        }
    }
}
