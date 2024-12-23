#include "board.h"
#include <stdlib.h>

Vector2 random_vector2(Vector2 bounds)
{
    Vector2 output = { -1, -1 };
    output.x = SDL_rand(bounds.x);
    output.y = SDL_rand(bounds.y);
    return output;
}

int get_neighbours(Vector2 input, Vector2 bounds, Vector2* output)
{
    int length = 0;
    Vector2 tmp = { -1, -1 };
    if (input.x > 0 && input.y > 0) {
        tmp.x = input.x - 1;
        tmp.y = input.y - 1;
        output[length] = tmp;
        length++;
    }
    if (input.x > 0 && input.y < bounds.y - 1) {
        tmp.x = input.x - 1;
        tmp.y = input.y + 1;
        output[length] = tmp;
        length++;
    }
    if (input.x > 0) {
        tmp.x = input.x - 1;
        tmp.y = input.y;
        output[length] = tmp;
        length++;
    }
    if (input.x < bounds.x - 1 && input.y > 0) {
        tmp.x = input.x + 1;
        tmp.y = input.y - 1;
        output[length] = tmp;
        length++;
    }
    if (input.x < bounds.x - 1 && input.y < bounds.y - 1) {
        tmp.x = input.x + 1;
        tmp.y = input.y + 1;
        output[length] = tmp;
        length++;
    }
    if (input.x < bounds.x - 1) {
        tmp.x = input.x + 1;
        tmp.y = input.y;
        output[length] = tmp;
        length++;
    }
    if (input.y > 0) {
        tmp.x = input.x;
        tmp.y = input.y - 1;
        output[length] = tmp;
        length++;
    }
    if (input.y < bounds.y - 1) {
        tmp.x = input.x;
        tmp.y = input.y + 1;
        output[length] = tmp;
        length++;
    }
    return length;
}

Cell new_cell()
{
    Cell output;
    output.has_mine = false;
    output.has_flag = false;
    output.is_revealed = false;
    output.revealed_value = -1;
    return output;
}

char cell_value(Board* board, Vector2 position)
{
    char out = 0;
    Vector2 neighbours[8];
    if (get_cell(board, &position)->has_mine) {
        return -1;
    }
    int n = get_neighbours(position, board->bounds, neighbours);
    for (int i = 0; i < n; i++) {
        if (get_cell(board, &neighbours[i])->has_mine) {
            out++;
        }
    }
    return out;
}

Vector2 find_empty_cell(Board* board)
{
    for (int i = 1; i < board->bounds.x - 1; i++) {
        for (int j = 1; j < board->bounds.y - 1; j++) {
            Vector2 position = { i, j };
            if (cell_value(board, position) == 0) {
                return position;
            }
        }
    }
    Vector2 out = { -1, -1 };
    return out;
}
Board generate_board(Vector2 bounds, int mine_count)
{
    Cell** cells = malloc(sizeof(Cell*) * bounds.x);
    for (int i = 0; i < bounds.x; i++) {
        cells[i] = malloc(sizeof(Cell) * bounds.y);
        for (int j = 0; j < bounds.y; j++) {
            cells[i][j] = new_cell();
        }
    }
    Board output = { cells, bounds };
    output.hover.x = -1;
    output.hover.y = -1;
    output.game_over = false;
    output.game_started = false;
    while (mine_count > 0) {
        const Vector2 mine_position = random_vector2(bounds);
        if (cells[mine_position.x][mine_position.y].has_mine) {
            continue;
        }
        mine_count--;
        cells[mine_position.x][mine_position.y].has_mine = true;
    }
    return output;
}

Cell* get_cell(Board* board, Vector2* position)
{
    return &board->cells[position->x][position->y];
}

void destroy_board(Board board)
{
    for (int i = 0; i < board.bounds.x; i++) {
        free(board.cells[i]);
    }
    free(board.cells);
}

void reveal_cell(Board* board, Vector2* position)
{
    Cell* cell = get_cell(board, position);
    if (cell->is_revealed == true) {
        return;
    }
    cell->is_revealed = true;
    cell->revealed_value = cell_value(board, *position);

    if (cell->revealed_value != 0) {
        return;
    }
    Vector2 neighbours[8];
    int n = get_neighbours(*position, board->bounds, neighbours);
    for (int i = 0; i < n; i++) {
        Cell* neighbour = get_cell(board, &neighbours[i]);
        if (neighbour->has_mine) {
            continue;
        }
        reveal_cell(board, &neighbours[i]);
    }
}

void right_click(Board* board)
{
    board->cells[board->hover.x][board->hover.y].has_flag
        = !board->cells[board->hover.x][board->hover.y].has_flag;
}

void left_click(Board* board)
{
    if (board->cells[board->hover.x][board->hover.y].has_flag) {
        return;
    }
    if (!board->game_started) {
        board->game_started = true;
        if (cell_value(board, board->hover) == 0) {
            reveal_cell(board, &board->hover);
            return;
        }
        Vector2 anchor_position = find_empty_cell(board);
        anchor_position.x = board->hover.x - anchor_position.x;
        anchor_position.y = board->hover.y - anchor_position.y;
        shift_board(board, &anchor_position);
        reveal_cell(board, &board->hover);
        return;
    }
    if (board->cells[board->hover.x][board->hover.y].has_mine) {
        board->game_over = true;
        return;
    }
    reveal_cell(board, &board->hover);
}

Vector2 sub_modulo(Vector2 a, Vector2 b, Vector2 bounds)
{
    Vector2 out = {
        (bounds.x + a.x - b.x) % bounds.x,
        (bounds.y + a.y - b.y) % bounds.y
    };
    return out;
}

void shift_board(Board* board, Vector2* direction)
{
    Cell** new_board = malloc(sizeof(Cell*) * board->bounds.x);
    for (int i = 0; i < board->bounds.x; i++) {
        new_board[i] = malloc(sizeof(Cell) * board->bounds.y);
        for (int j = 0; j < board->bounds.y; j++) {
            Vector2 position = { i, j };
            position = sub_modulo(position, *direction, board->bounds);
            new_board[i][j] = *get_cell(board, &position);
        }
    }

    for (int i = 0; i < board->bounds.x; i++) {
        for (int j = 0; j < board->bounds.y; j++) {
            board->cells[i][j] = new_board[i][j];
        }
        free(new_board[i]);
    }
    free(new_board);
}
