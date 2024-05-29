#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define TRUE 1
#define FALSE 0
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
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
SDL_Texture *texture = NULL;
TTF_Font* font = NULL;
SDL_Texture* textTexture = NULL;

int last_frame_time = 0;

int initialize_window(){
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return FALSE;
    }
    window = SDL_CreateWindow(
        "Game Window", 
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
    ball.x = 0.05 * WINDOW_WIDTH;
    ball.y = 0.7 * WINDOW_HEIGHT;
    ball.width = 0.9 * WINDOW_WIDTH;
    ball.height = 0.2 * WINDOW_HEIGHT;

    SDL_Surface* surface = IMG_Load("img/manor_gate.jpg");
    if (!surface) {
        printf("Error creating surface: %s\n", IMG_GetError());
        return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Error creating texture: %s\n", SDL_GetError());
        return;
    }
    TTF_Init();
    font = TTF_OpenFont("OpenSans-Regular.ttf", 24);
    if (!font) {
        printf("Error loading font: %s\n", TTF_GetError());
        return;
    }

    SDL_Color color = {255, 255, 255, 255};  // white color
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "my name is ...", color);
    if (!textSurface) {
        printf("Error creating text surface: %s\n", TTF_GetError());
        return;
    }

    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        printf("Error creating text texture: %s\n", SDL_GetError());
        return;
    }

    SDL_FreeSurface(textSurface);
    SDL_FreeSurface(surface);
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
    // while(!SDL_TICKS_PASSED(SDL_GetTicks(), last_frame_time + FRAME_TARGET_TIME));

    // last_frame_time = SDL_GetTicks();
    
    // if(ball.x >= WINDOW_WIDTH - ball.width || ball.x <= 0){
    //     change_x = -change_x;
    // }

    // if(ball.y >= WINDOW_HEIGHT - ball.height || ball.y <= 0){
    //     change_y = -change_y;
    // }
    // ball.x += change_x;
    // ball.y += change_y;
}
void render(){
    // SDL_SetRenderDrawColor(renderer, 110, 120, 170, 165);
    // SDL_RenderClear(renderer);

    SDL_Rect rect_img = {
        0,
        0,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    };
    // Draw before present
    SDL_Rect rect_ball = {
        ball.x,
        ball.y,
        ball.width,
        ball.height
    };
    SDL_RenderCopy(renderer, texture, NULL, &rect_img);
    SDL_SetRenderDrawColor(renderer, 110, 120, 170, 255);
    SDL_RenderFillRect(renderer, &rect_ball);
    SDL_Rect textRect = {100, 800, 200, 50};  // position and size of the text
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_RenderPresent(renderer);
}
void destroy_window(){
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
    TTF_Quit();
}
int main(int argc, char *argv[]){
    game_is_running = initialize_window();
    if(!game_is_running){
        return 1;
    }

    setup();

    while(game_is_running){
        process_input();
        update();
        render();
    }

    destroy_window();
    return 0;
}
