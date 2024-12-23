#pragma once
#include "board.h"
#include <SDL3/SDL.h>

void draw_circle(SDL_Renderer* target, Vector2 position, int radius);
void draw_grid(SDL_Renderer* target, Vector2 size);
void draw_mines(SDL_Renderer* target, Board board);
void draw_flags(SDL_Renderer* target, Board board);
void draw_hover(SDL_Renderer* targer, Board board);
void draw_revealed(SDL_Renderer* target, Board board);
void draw_numbers(SDL_Renderer* target, Board* board);
Vector2 get_resolution(SDL_Renderer* renderer, Vector2 size);
