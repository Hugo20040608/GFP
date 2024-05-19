#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <SDL2/SDL.h>

#define TRUE 1
#define FALSE 0
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

struct ball{
    int x;
    int y;
    int width;
    int height;
} ball;

int game_is_running = FALSE; 
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int last_frame_time = 0;

int initialize_window(){
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return FALSE;
    }
    window = SDL_CreateWindow(
        NULL, 
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );
    if(!window){
        printf("Error creating window: %s\n", SDL_GetError());
        return FALSE;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer){
        printf("Error creating renderer: %s\n", SDL_GetError());
        return FALSE;
    }
    return TRUE;
}
void setup(){
    ball.x = 200;
    ball.y = 200;
    ball.width = 100;
    ball.height = 100;
}
void process_input(){
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type){
        case SDL_QUIT: // X button
            game_is_running = FALSE;
            break;
        case SDL_KEYDOWN: // key press
            if(event.key.keysym.sym == SDLK_ESCAPE)
                game_is_running = FALSE;
            break;
    }
}
int change_x = 5;
int change_y = 5;
void update(){
    // wait some time 
    while(!SDL_TICKS_PASSED(SDL_GetTicks(), last_frame_time + FRAME_TARGET_TIME));

    last_frame_time = SDL_GetTicks();
    
    if(ball.x >= WINDOW_WIDTH - ball.width || ball.x <= 0){
        change_x = -change_x;
    }

    if(ball.y >= WINDOW_HEIGHT - ball.height || ball.y <= 0){
        change_y = -change_y;
    }
    ball.x += change_x;
    ball.y += change_y;
}
void render(){
    SDL_SetRenderDrawColor(renderer, 110, 120, 170, 165);
    SDL_RenderClear(renderer);
    
    // Draw before present
    SDL_Rect rect_ball = {
        ball.x,
        ball.y,
        ball.width,
        ball.height
    };
    SDL_SetRenderDrawColor(renderer,189, 254, 37, 255);
    SDL_RenderFillRect(renderer, &rect_ball);

    SDL_RenderPresent(renderer);
}
void destroy_window(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
int main(int argc, char *argv[]){
    game_is_running = initialize_window();
    
    setup();

    while(game_is_running){
        process_input();
        update();
        render();
    }

    destroy_window();
    return 0;
}
