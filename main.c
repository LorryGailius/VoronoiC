#include <SDL2/SDL.h>
#include "voronoi.h"
#include <assert.h>
#include <time.h>

const int screen_width = 800;
const int screen_height = 600;

int main(int argc, char *argv[]) {
    srand(time(NULL));

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Event event;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }

    if (SDL_CreateWindowAndRenderer(screen_width, screen_height, SDL_WINDOW_SHOWN, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        return 3;
    }

    int mouse_x, mouse_y, pointNum = 0;

    pixel_t **pixels = (pixel_t**)malloc(screen_height * sizeof(pixel_t*));

    for (size_t i = 0; i < screen_height; i++) {

        pixels[i] = (pixel_t*)malloc(screen_width * sizeof(pixel_t));
    }

    point_t *points = malloc(MAX_POINTS * sizeof(point_t));

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderPresent(renderer);

    while (1) {

        SDL_PollEvent(&event);

        if (event.type == SDL_QUIT) {
            break;
        }
        else if(event.type == SDL_MOUSEBUTTONDOWN) {
            if(event.button.button == SDL_BUTTON_LEFT) { 

                SDL_GetMouseState(&mouse_x, &mouse_y);
                add_point(points, &pointNum, mouse_x, mouse_y); 
                voronoi(points, pointNum, screen_width, screen_height, pixels);
                
                for (size_t i = 0; i < screen_height; i++) {
                    for (size_t j = 0; j < screen_width; j++) {
                        SDL_SetRenderDrawColor(renderer, pixels[i][j].r, pixels[i][j].g, pixels[i][j].b, 255);
                        SDL_RenderDrawPoint(renderer, j, i);
                    }
                }
                SDL_RenderPresent(renderer);
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    for(size_t i = 0; i < screen_height; i++) { free(pixels[i]); }
    
    free(pixels);
    free(points);

    return 0;
}