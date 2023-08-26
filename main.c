// Laurynas Gailius 2023
/*
References:
    - Rendering pixel array by keeping it in RAM and then just copying it to a texture :
            https://discourse.libsdl.org/t/most-efficient-way-of-getting-render-pixels/27581/4

Color is represented in RGBA format, where each channel is 8 bits long.
    - 0xAABBGGRR
    - AA - alpha channel
    - BB - blue channel
    - GG - green channel
    - RR - red channel
*/

#include <SDL2/SDL.h>
#include "voronoi.h"

#define WIDTH 800
#define HEIGHT 600
#define MAX_POINTS 100
#define POINT_RADIUS 4
#define POINT_COLOR 0xFF000000

uint32_t color_palette[] = {
    0xFFA7794E,
    0xFF2B8EF2,
    0xFF5957E1,
    0xFFB2B776,
    0xFF4FA159,
    0xFF48C9ED,
    0xFFA17AB0,
    0xFF7CBE01};

int palette_size = sizeof(color_palette) / sizeof(uint32_t);

void v_texturize_pixels(SDL_Texture *texture, uint32_t *pixels, int texture_pitch)
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

void display(SDL_Texture *texture, voronoi_t *v)
{
    voronoi_draw(v);
    v_texturize_pixels(texture, v->pixels, 0);
}

int main(int argc, char *argv[])
{
    voronoi_t properties = voronoi_create(WIDTH, HEIGHT, MAX_POINTS, POINT_RADIUS, POINT_COLOR, color_palette, palette_size);

    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // create window
    SDL_Window *window = SDL_CreateWindow("VoronoiC",
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

    bool should_quit = false, animate = false;
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
                    voronoi_add_point(&properties, e.button.x, e.button.y);
                }
                display(texture, &properties);
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_m)
            {
                animate = !animate;
            }
        }

        if (animate)
        {
            voronoi_move_points(&properties);
            display(texture, &properties);
        }

        // render on screen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    voronoi_destroy(&properties);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}