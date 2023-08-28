// Laurynas Gailius 2023
/*
References:
    - Rendering pixel array by keeping it in RAM and then just copying it to a texture :
            https://discourse.libsdl.org/t/most-efficient-way-of-getting-render-pixels/27581/4
    - KD-Tree and nearest neighbour search pseudocode:
            https://en.wikipedia.org/wiki/K-d_tree
    - KD-Tree implementation:
            https://www.geeksforgeeks.org/k-dimensional-tree/
    
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
#define MAX_POINTS 10000
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

// Dump pixel data to an SDL_Texture
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

// Draw voronoi diagram on screen
void display(SDL_Texture *texture, voronoi_t *v, bool draw_points)
{
    voronoi_draw(v, draw_points);
    v_texturize_pixels(texture, v->pixels, 0);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    voronoi_t properties = voronoi_create(WIDTH, HEIGHT, MAX_POINTS, POINT_RADIUS, POINT_COLOR, color_palette, palette_size);

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Create window
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

    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        return 1;
    }

    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

    // Create texture
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

    bool should_quit = false, points = true;
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
                display(texture, &properties, points);
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                    case SDLK_p:
                    {
                        // Toggle point visibility
                        points = !points;
                        display(texture, &properties, points);
                        break;
                    }
                    case SDLK_g:
                    {
                        // Generate random points
                        voronoi_generate_random_points(&properties, 10);
                        display(texture, &properties, points);
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
        }
        // Render on screen
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