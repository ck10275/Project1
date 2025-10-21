#ifndef GAME_H
#define GAME_H

#include <stdint.h>

typedef struct {
    uint64_t red;    // red pieces (player 1)
    uint64_t black;  // black pieces (player 2)
    uint64_t kings;  // kings (bit==1 means that square holds a king of whichever color)
    int turn;        // 0 = red to move, 1 = black to move
} GameState;

// initialize standard starting positions
void init_standard(GameState *g);

// print board to stdout
void print_board(const GameState *g);

// convert coordinate strings to indices and back
int coord_to_index(const char *coord); // "a1" .. "h8" -> index 0..63 (a1 bottom-left)
void index_to_coord(int idx, char *out);

// validate and apply a move (returns 1 if move applied, 0 on invalid)
int apply_move(GameState *g, int from, int to, int *captured_idx_out);

// check if any captures exist for current player
int any_capture_available(const GameState *g);

// check winner: -1 none, 0 red wins, 1 black wins
int check_winner(const GameState *g);

// save/load
int save_game(const char *filename, const GameState *g);
int load_game(const char *filename, GameState *g);

#endif // GAME_H
