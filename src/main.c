#include "board.h"
#include "graphics.h"

#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

const int GRID_WIDTH = 18;
const int GRID_HEIGHT = 14;
const int CELL_RESOLUTION = 30;
const Vector2 BOARD_SIZE = { 18, 14 };
const int MINE_COUNT = 40;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static Board board;
static Vector2 resolution = { -1, -1 };
static bool debug_draw_mines = false;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_CreateWindowAndRenderer("minesweeper", GRID_WIDTH * CELL_RESOLUTION, GRID_HEIGHT * CELL_RESOLUTION, 0, &window, &renderer);

    SDL_Log("created renderer");
    SDL_Rect vp;
    bool status = SDL_GetRenderViewport(renderer, &vp);
    if (status)
        SDL_Log("x: %d, y: %d", vp.w, vp.h);
    else
        SDL_Log("couldn't get vp");

    Vector2 grid_bounds = { GRID_WIDTH, GRID_HEIGHT };
    SDL_Log("generating board");
    board = generate_board(grid_bounds, MINE_COUNT);
    SDL_Log("generated board");

    resolution = get_resolution(renderer, board.bounds);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    switch (event->type) {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_MOUSE_MOTION:
        board.hover.x = event->motion.x / resolution.x;
        board.hover.y = event->motion.y / resolution.y;
        // SDL_Log("%d %d", mouse_position.x, mouse_position.y);
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event->button.button == 3) {
            right_click(&board);
        } else if (event->button.button == 1) {
            left_click(&board);
        }
        break;
    case SDL_EVENT_KEY_DOWN:
        if (event->key.key == SDLK_H) {
            // debug_draw_mines = !debug_draw_mines;
        }
        if (event->key.key == SDLK_UP) {
            Vector2 position = { 0, -1 };
            shift_board(&board, &position);
        }
        if (event->key.key == SDLK_DOWN) {
            Vector2 position = { 0, 1 };
            shift_board(&board, &position);
        }
        if (event->key.key == SDLK_LEFT) {
            Vector2 position = { -1, 0 };
            shift_board(&board, &position);
        }
        if (event->key.key == SDLK_RIGHT) {
            Vector2 position = { 1, 0 };
            shift_board(&board, &position);
        }
        if (event->key.key == SDLK_R) {
            destroy_board(board);
            Vector2 grid_bounds = { GRID_WIDTH, GRID_HEIGHT };
            board = generate_board(grid_bounds, MINE_COUNT);
        }
        break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    draw_grid(renderer, BOARD_SIZE);
    draw_hover(renderer, board);
    draw_revealed(renderer, board);
    draw_flags(renderer, board);
    if (board.game_over || debug_draw_mines) {
        draw_mines(renderer, board);
    }
    draw_numbers(renderer, &board);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    destroy_board(board);
}
