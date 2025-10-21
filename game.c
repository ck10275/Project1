#include "game.h"
#include "bitops.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Coordinate mapping:
   index = row*8 + col
   row 0 at top, row 7 at bottom.
   For user convenience we allow "a1".. "h8" where a1 is bottom-left:
     a1 => row 7, col 0 => index = 7*8 + 0 = 56
*/

static inline int in_bounds(int r, int c) { return r >= 0 && r < 8 && c >= 0 && c < 8; }
static void square_rc(int idx, int *r, int *c) { *r = idx / 8; *c = idx % 8; }

void init_standard(GameState *g) {
    g->red = 0ULL;
    g->black = 0ULL;
    g->kings = 0ULL;
    g->turn = 0; // red starts
    // place red on rows 0..2 (top three rows) on dark squares
    for (int r = 0; r <= 2; ++r) {
        for (int c = 0; c < 8; ++c) {
            if ((r + c) % 2 == 1) {
                int idx = r*8 + c;
                g->red = SetBit64(g->red, idx);
            }
        }
    }
    // place black on rows 5..7 (bottom three rows)
    for (int r = 5; r <= 7; ++r) {
        for (int c = 0; c < 8; ++c) {
            if ((r + c) % 2 == 1) {
                int idx = r*8 + c;
                g->black = SetBit64(g->black, idx);
            }
        }
    }
}

void print_board(const GameState *g) {
    puts("  a b c d e f g h");
    for (int r = 0; r < 8; ++r) {
        printf("%d ", 8 - r);
        for (int c = 0; c < 8; ++c) {
            int idx = r*8 + c;
            char ch = '.';
            if ((r + c) % 2 == 0) { // light square
                ch = ' ';
            } else {
                int rbit = GetBit64(g->red, idx);
                int bbit = GetBit64(g->black, idx);
                int kbit = GetBit64(g->kings, idx);
                if (rbit) ch = kbit ? 'R' : 'r';
                else if (bbit) ch = kbit ? 'B' : 'b';
                else ch = '.';
            }
            printf("%c ", ch);
        }
        printf("%d\n", 8 - r);
    }
    puts("  a b c d e f g h");
    printf("Turn: %s\n", g->turn == 0 ? "Red (r/R)" : "Black (b/B)");
}

int coord_to_index(const char *coord) {
    if (!coord || strlen(coord) < 2) return -1;
    char file = coord[0];
    char rank = coord[1];
    if (file < 'a' || file > 'h') return -1;
    if (rank < '1' || rank > '8') return -1;
    int col = file - 'a';
    int row_from_bottom = rank - '1'; // 0..7 bottom to top
    int row = 7 - row_from_bottom; // convert to row 0..7 top to bottom
    return row*8 + col;
}

void index_to_coord(int idx, char *out) {
    int row = idx / 8;
    int col = idx % 8;
    int row_from_bottom = 7 - row;
    out[0] = 'a' + col;
    out[1] = '1' + row_from_bottom;
    out[2] = '\0';
}

static int occupied(const GameState *g, int idx) {
    return GetBit64(g->red, idx) || GetBit64(g->black, idx);
}

static int is_player_piece(const GameState *g, int idx, int player) {
    if (player == 0) return GetBit64(g->red, idx);
    else return GetBit64(g->black, idx);
}

static int is_opponent_piece(const GameState *g, int idx, int player) {
    if (player == 0) return GetBit64(g->black, idx);
    else return GetBit64(g->red, idx);
}

static void remove_piece(GameState *g, int idx) {
    g->red = ClearBit64(g->red, idx);
    g->black = ClearBit64(g->black, idx);
    g->kings = ClearBit64(g->kings, idx);
}

static void set_piece(GameState *g, int idx, int player, int king) {
    remove_piece(g, idx);
    if (player == 0) g->red = SetBit64(g->red, idx);
    else g->black = SetBit64(g->black, idx);
    if (king) g->kings = SetBit64(g->kings, idx);
}

int apply_move(GameState *g, int from, int to, int *captured_idx_out) {
    if (from < 0 || from >= 64 || to < 0 || to >= 64) return 0;
    if (!is_player_piece(g, from, g->turn)) return 0;
    if (occupied(g, to)) return 0;
    // must be diagonal
    int fr = from/8, fc = from%8;
    int tr = to/8, tc = to%8;
    int dr = tr - fr, dc = tc - fc;
    if (abs(dc) != abs(dr)) return 0;
    int king = GetBit64(g->kings, from);
    int dir = (g->turn == 0) ? 1 : -1; // red down (+1 row), black up (-1 row) given row 0 top
    // normal step
    if (abs(dr) == 1) {
        if (!king) {
            if (dr != dir) return 0; // non-king moves only forward
        }
        int player = g->turn;
        // move piece
        int was_king = GetBit64(g->kings, from);
        remove_piece(g, from);
        set_piece(g, to, player, was_king);
        // promotion
        int tr_bot = tr;
        if (player == 0 && tr == 7) g->kings = SetBit64(g->kings, to);
        if (player == 1 && tr == 0) g->kings = SetBit64(g->kings, to);
        if (captured_idx_out) *captured_idx_out = -1;
        g->turn = 1 - g->turn;
        return 1;
    } else if (abs(dr) == 2) {
        // capture
        int mr = (fr + tr)/2, mc = (fc + tc)/2;
        int mid = mr*8 + mc;
        if (!is_opponent_piece(g, mid, g->turn)) return 0;
        if (!king) {
            if ((dr/2) != dir) return 0;
        }
        int player = g->turn;
        int was_king = GetBit64(g->kings, from);
        remove_piece(g, from);
        remove_piece(g, mid);
        set_piece(g, to, player, was_king);
        // promotion
        if (player == 0 && tr == 7) g->kings = SetBit64(g->kings, to);
        if (player == 1 && tr == 0) g->kings = SetBit64(g->kings, to);
        if (captured_idx_out) *captured_idx_out = mid;
        // After a capture, keep same player's turn if further captures possible from 'to' (mandatory chain).
        // We'll let the caller check; for simplicity, if further capture available we keep player's turn,
        // otherwise flip turn.
        // Check further captures:
        int more = 0;
        int trc = to/8, tcc = to%8;
        int dirs[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
        for (int i=0;i<4;i++) {
            int rr = trc + dirs[i][0]*1, cc = tcc + dirs[i][1]*1;
            int rrr = trc + dirs[i][0]*2, ccc = tcc + dirs[i][1]*2;
            if (!in_bounds(rr,cc) || !in_bounds(rrr,ccc)) continue;
            int midx = rr*8 + cc, toidx = rrr*8 + ccc;
            if (occupied(g, toidx)) continue;
            if (is_opponent_piece(g, midx, g->turn)) {
                if (was_king) { more = 1; break; }
                else {
                    // non-king can only capture forward
                    int forward = (g->turn==0)?1:-1;
                    if (dirs[i][0] == forward) { more = 1; break; }
                }
            }
        }
        if (!more) g->turn = 1 - g->turn;
        return 1;
    }
    return 0;
}

int any_capture_available(const GameState *g) {
    int player = g->turn;
    uint64_t pieces = (player == 0) ? g->red : g->black;
    for (int idx = 0; idx < 64; ++idx) {
        if (!GetBit64(pieces, idx)) continue;
        int r = idx/8, c = idx%8;
        int king = GetBit64(g->kings, idx);
        int dirs[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
        for (int i=0;i<4;i++) {
            int dr = dirs[i][0], dc = dirs[i][1];
            int r1 = r + dr, c1 = c + dc;
            int r2 = r + 2*dr, c2 = c + 2*dc;
            if (!in_bounds(r1,c1) || !in_bounds(r2,c2)) continue;
            int mid = r1*8 + c1, to = r2*8 + c2;
            if (!is_opponent_piece(g, mid, player)) continue;
            if (occupied(g, to)) continue;
            if (king) return 1;
            int forward = (player == 0) ? 1 : -1;
            if (dr == forward) return 1;
        }
    }
    return 0;
}

int check_winner(const GameState *g) {
    if (g->red == 0ULL) return 1; // black wins
    if (g->black == 0ULL) return 0; // red wins
    // also if current player has no moves: simplistic detection (no pieces or no legal moves)
    // For brevity we skip enumerating all moves; this is a basic check only.
    return -1;
}

int save_game(const char *filename, const GameState *g) {
    FILE *f = fopen(filename, "w");
    if (!f) return 0;
    fprintf(f, "%llu\n%llu\n%llu\n%d\n", (unsigned long long)g->red, (unsigned long long)g->black, (unsigned long long)g->kings, g->turn);
    fclose(f);
    return 1;
}

int load_game(const char *filename, GameState *g) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;
    unsigned long long r,b,k;
    int t;
    if (fscanf(f, "%llu%llu%llu%d", &r, &b, &k, &t) < 4) { fclose(f); return 0; }
    g->red = (uint64_t)r;
    g->black = (uint64_t)b;
    g->kings = (uint64_t)k;
    g->turn = t;
    fclose(f);
    return 1;
}
