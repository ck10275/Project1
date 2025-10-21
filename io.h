#ifndef IO_H
#define IO_H

#include "game.h"

// parse user input like "e3-d4" or "12-21" or "save filename" or "load filename"
int parse_move_string(const char *s, int *from, int *to);
void prompt_loop(GameState *g);

#endif // IO_H
