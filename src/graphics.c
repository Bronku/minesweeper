#include "graphics.h"
#include "board.h"

const SDL_Color light_green = { 170, 215, 81, SDL_ALPHA_OPAQUE };
const SDL_Color dark_green = { 162, 209, 73, SDL_ALPHA_OPAQUE };
const SDL_Color light_yellow = { 229, 194, 159, SDL_ALPHA_OPAQUE };
const SDL_Color dark_yellow = { 215, 184, 153, SDL_ALPHA_OPAQUE };
const SDL_Color hover_dark_green = { 185, 221, 119, SDL_ALPHA_OPAQUE };
const SDL_Color black = { 0, 0, 0, SDL_ALPHA_OPAQUE };
const SDL_Color red = { 255, 0, 0, SDL_ALPHA_OPAQUE };

void set_color(SDL_Renderer* target, SDL_Color color)
{
    SDL_SetRenderDrawColor(target, color.r, color.g, color.b, color.a);
}

void draw_circle(SDL_Renderer* target, Vector2 position, int radius)
{
    int x = radius;
    int y = 0;
    int radius_error = 1 - x;

    while (x >= y) {
        for (int i = position.x - x; i < position.x + x; i++) {
            SDL_RenderPoint(target, i, position.y + y);
            SDL_RenderPoint(target, i, position.y - y);
        }
        for (int i = position.x - y; i < position.x + y; i++) {
            SDL_RenderPoint(target, i, position.y + x);
            SDL_RenderPoint(target, i, position.y - x);
        }
        y++;
        if (radius_error < 0) {
            radius_error += 2 * y + 1;
        } else {
            x--;
            radius_error += 2 * (y - x + 1);
        }
    }
}

Vector2 get_resolution(SDL_Renderer* renderer, Vector2 size)
{
    SDL_Rect viewport;
    Vector2 resolution;
    bool darker_color = false;
    SDL_GetRenderViewport(renderer, &viewport);
    resolution.x = viewport.w / size.x;
    resolution.y = viewport.h / size.y;
    return resolution;
}

void draw_grid(SDL_Renderer* target, Vector2 size)
{
    Vector2 resolution = get_resolution(target, size);
    bool darker_color = false;
    for (int i = 0; i < size.x; i++) {
        for (int j = 0; j < size.y; j++) {
            SDL_FRect rectangle;
            rectangle.x = resolution.x * i;
            rectangle.y = resolution.y * j;
            rectangle.w = resolution.x;
            rectangle.h = resolution.y;
            if (darker_color) {
                set_color(target, dark_green);
            } else {
                set_color(target, light_green);
            }
            SDL_RenderFillRect(target, &rectangle);
            darker_color = !darker_color;
        }
        darker_color = !darker_color;
    }
}

void draw_mines(SDL_Renderer* target, Board board)
{
    set_color(target, black);
    Vector2 resolution = get_resolution(target, board.bounds);
    int radius = resolution.x / 4;
    for (int i = 0; i < board.bounds.x; i++) {
        // SDL_Log("i: %d", i);
        for (int j = 0; j < board.bounds.y; j++) {
            // SDL_Log("j: %d", j);
            if (!board.cells[i][j].has_mine) {
                continue;
            }
            Vector2 position = {
                resolution.x * i + resolution.x / 2,
                resolution.y * j + resolution.y / 2
            };
            draw_circle(target, position, radius);
        }
    }
}

void draw_flags(SDL_Renderer* target, Board board)
{
    set_color(target, red);
    Vector2 resolution = get_resolution(target, board.bounds);
    int radius = resolution.x / 4;
    for (int i = 0; i < board.bounds.x; i++) {
        // SDL_Log("i: %d", i);
        for (int j = 0; j < board.bounds.y; j++) {
            // SDL_Log("j: %d", j);
            if (!board.cells[i][j].has_flag) {
                continue;
            }
            Vector2 position = {
                resolution.x * i + resolution.x / 2,
                resolution.y * j + resolution.y / 2
            };
            draw_circle(target, position, radius);
        }
    }
}

void draw_hover(SDL_Renderer* target, Board board)
{
    Vector2 resolution = get_resolution(target, board.bounds);
    set_color(target, hover_dark_green);
    SDL_FRect rectangle;
    rectangle.x = resolution.x * board.hover.x;
    rectangle.y = resolution.y * board.hover.y;
    rectangle.w = resolution.x;
    rectangle.h = resolution.y;
    SDL_RenderFillRect(target, &rectangle);
}

void draw_revealed(SDL_Renderer* target, Board board)
{
    set_color(target, red);
    Vector2 resolution = get_resolution(target, board.bounds);
    int radius = resolution.x / 4;
    bool darker_color = true;
    for (int i = 0; i < board.bounds.x; i++) {
        // SDL_Log("i: %d", i);
        for (int j = 0; j < board.bounds.y; j++) {
            // SDL_Log("j: %d", j);
            darker_color = !darker_color;
            if (!board.cells[i][j].is_revealed) {
                continue;
            }
            SDL_FRect rectangle;
            rectangle.x = resolution.x * i;
            rectangle.y = resolution.y * j;
            rectangle.w = resolution.x;
            rectangle.h = resolution.y;
            if (darker_color) {
                set_color(target, dark_yellow);
            } else {
                set_color(target, light_yellow);
            }
            SDL_RenderFillRect(target, &rectangle);
        }
        darker_color = !darker_color;
    }
}

void draw_numbers(SDL_Renderer* target, Board* board)
{
    set_color(target, black);
    Vector2 resolution = get_resolution(target, board->bounds);
    int radius = resolution.x / 4;
    for (int i = 0; i < board->bounds.x; i++) {
        for (int j = 0; j < board->bounds.y; j++) {
            Vector2 position = { i, j };
            char text[2] = { get_cell(board, &position)->revealed_value, '\0' };
            if (text[0] < 1) {
                continue;
            }
            text[0] += '0';
            int x = resolution.x * i + resolution.x / 2;
            int y = resolution.y * j + resolution.y / 2;
            SDL_RenderDebugText(target, x, y, text);
        }
    }
}
