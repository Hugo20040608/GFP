#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <SDL2/SDL.h>
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main(int argc, char *argv[]){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = NULL;
    SDL_Event e;
    while (1){
        if (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                break;
            }
        }
    }
    SDL_Renderer *renderer = NULL;
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    if (window == NULL || renderer == NULL){
        printf("Window or Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_RenderSetScale(renderer, 4, 4);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoint(renderer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    SDL_RenderPresent(renderer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
