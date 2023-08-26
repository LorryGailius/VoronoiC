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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_POINTS 100
#define POINT_RADIUS 4
#define POINT_COLOR 0xFF000000

typedef struct
{
    int x;
    int y;
    int velocity_x;
    int velocity_y;
    uint32_t color;
} point_t;

uint32_t pixels[HEIGHT][WIDTH] = {0};
point_t points[MAX_POINTS];
int point_count = 0;

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

void fill_screen(uint32_t color)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            pixels[y][x] = color;
        }
    }
}

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

void add_point(int x, int y)
{
    point_t p = {
        .x = x,
        .y = y,
        .color = color_palette[point_count % palette_size],
        .velocity_x = rand() % 5 - 2,
        .velocity_y = rand() % 5 - 2};

    points[point_count++] = p;
}

void generate_random_points(int count)
{
    for (int i = 0; i < count; i++)
    {
        add_point(rand() % WIDTH, rand() % HEIGHT);
    }
}

void draw_all_points()
{
    for (int i = 0; i < point_count; i++)
    {
        draw_circle(points[i].x, points[i].y, POINT_RADIUS, POINT_COLOR);
    }
}

void move_points()
{
    for (int i = 0; i < point_count; i++)
    {
        points[i].x += points[i].velocity_x;
        points[i].y += points[i].velocity_y;

        if (points[i].x < 0 || points[i].x >= WIDTH)
        {
            points[i].velocity_x *= -1;
        }

        if (points[i].y < 0 || points[i].y >= HEIGHT)
        {
            points[i].velocity_y *= -1;
        }
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

int sqr_dist(int x1, int y1, int x2, int y2)
{
    int dx = x1 - x2;
    int dy = y1 - y2;

    return dx * dx + dy * dy;
}

void brute_force_voronoi()
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            int prev = 0;
            for (int i = 0; i < point_count; i++)
            {
                if (sqr_dist(x, y, points[i].x, points[i].y) < sqr_dist(x, y, points[prev].x, points[prev].y))
                {
                    prev = i;
                }
            }
            pixels[y][x] = points[prev].color;
        }
    }
}

void draw_voronoi(SDL_Texture *texture)
{
    brute_force_voronoi();
    draw_all_points();
    texturize_pixels(texture, 0);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

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

    generate_random_points(5);
    draw_voronoi(texture);

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
                if (e.button.button == SDL_BUTTON_LEFT && point_count < MAX_POINTS)
                {
                    add_point(e.button.x, e.button.y);
                }
                draw_voronoi(texture);
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_m)
            {
                animate = !animate;
            }
        }

        if (animate)
        {
            move_points();
            draw_voronoi(texture);
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