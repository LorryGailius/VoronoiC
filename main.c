// Laurynas Gailius 2023
/*
References: 
    - Rendering pixel array by keeping it in RAM and then just copying it to a texture : 
            https://discourse.libsdl.org/t/most-efficient-way-of-getting-render-pixels/27581/4 
*/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_POINTS 10
#define POINT_RADIUS 4

typedef struct
{
    int x;
    int y;
} point_t;

uint32_t pixels[HEIGHT][WIDTH] = {0};
point_t points[MAX_POINTS];

void draw_circle(int cx, int cy, int radius, uint32_t color)
{
    int x0 = cx - radius;
    int y0 = cy - radius;
    int x1 = cx + radius;
    int y1 = cy + radius;

    for (int x = x0; x < x1; x++)
    {
        if (x >= 0 && x < WIDTH)
        {
            for (int y = y0; y < y1; y++)
            {
                if (y >= 0 && y < HEIGHT)
                {
                    int dx = x - cx;
                    int dy = y - cy;

                    if ((dx * dx + dy * dy) <= (radius * radius))
                    {
                        pixels[y][x] = color;
                    }
                }
            }
        }
    }
}

void generate_random_points()
{
    for (int i = 0; i < MAX_POINTS; i++)
    {
        points[i].x = rand() % WIDTH;
        points[i].y = rand() % HEIGHT;
    }
}

void texturize_pixels(SDL_Texture *texture, int texture_pitch)
{
    void *texture_pixels = NULL;

    if (SDL_LockTexture(texture, NULL, &texture_pixels, &texture_pitch) != 0)
    {
        SDL_Log("Unable to lock texture: %s", SDL_GetError());
    }
    else
    {
        memcpy(texture_pixels, pixels, texture_pitch * HEIGHT);
    }

    SDL_UnlockTexture(texture);
}

int main(int argc, char *argv[])
{
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // create window
    SDL_Window *window = SDL_CreateWindow("sdl2_pixelbuffer",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        return 1;
    }

    // create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        return 1;
    }

    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

    // create texture
    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT);
    if (texture == NULL)
    {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        return 1;
    }

    generate_random_points();
    for (int i = 0; i < MAX_POINTS; i++)
    {
        draw_circle(points[i].x, points[i].y, POINT_RADIUS, 0xFFFFFFFF);
    }

    texturize_pixels(texture, 0);

    bool should_quit = false;
    SDL_Event e;
    while (!should_quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                should_quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    int x = e.button.x;
                    int y = e.button.y;
                    draw_circle(x, y, POINT_RADIUS, 0xFFFFFFFF);
                }

                texturize_pixels(texture, 0);
            }
        }

        // render on screen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}