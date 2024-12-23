#pragma once
#include <SDL3/SDL.h>
#include <stdbool.h>

typedef struct {
    int x;
    int y;
} Vector2;
Vector2 random_vector2(Vector2 bounds);
// returns size of the output, doesn't check if the output will fit
int get_neighbours(Vector2 input, Vector2 bounds, Vector2* output);

typedef struct {
    bool has_mine;
    bool has_flag;
    bool is_revealed;
    // -1 means it hasn't been calculated
    char revealed_value;
} Cell;
Cell new_cell();
// returns true if there isn't a mine, cells with flags can't be revealed

typedef struct {
    // x, y
    Cell** cells;
    Vector2 bounds;
    Vector2 hover;
    bool game_over;
    bool game_started;
} Board;
Board generate_board(Vector2 bounds, int mine_count);
Cell* get_cell(Board* board, Vector2* position);
void shift_board(Board* board, Vector2* direction);
void destroy_board(Board);
void right_click(Board*);
void left_click(Board*);
