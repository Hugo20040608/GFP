#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "fiction.h"
#include "utf8split.h"
#include "constants.h"
#include "music.h"

int32_t initialize_window();
void setup();
void render_background(char *event);
void render_description(char *event);
void render_background_character(char *event);
void process_input();
void destroy_window();

int main(int argc, char *argv[]){
    game_is_running = initialize_window();
    if(!game_is_running){
        return 1;
    }
    setup();
    play_music();
    char event[128] = "event_1";
    while(game_is_running){
        render_background(event);
        if(background_character(event) != NULL)
            render_background_character(event);
        while(1){
            render_description(event);
            if(currentWordIndex >= totalWords)
            {
                process_input();
                break;
            }
        }
        // if (diaolog_is_visible(event))
        //     render_dialog(event);
        // if(endding)
        //     game_is_running = FALSE;
        // render_choice();
        // process_input();
        break;
    }
    destroy_window();
    free_music();
    return 0;
}

// initialize window
int32_t initialize_window(){
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return FALSE;
    }
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        SDL_Log("Could not get display mode: %s", SDL_GetError());
        // handle error here
    } else {
        WINDOW_WIDTH = displayMode.w;
        WINDOW_HEIGHT = displayMode.h;
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
// setup window
void setup_cursor(){
    SDL_Surface* cursorSurface = SDL_LoadBMP("img/ancientbook.bmp");
    if (!cursorSurface) {
        printf("Unable to load cursor image: %s\n", SDL_GetError());
    }
    else {
        SDL_Cursor* cursor = SDL_CreateColorCursor(cursorSurface, 0, 0);
        if (!cursor) {
            printf("Unable to create cursor: %s\n", SDL_GetError());
        } else {
            SDL_SetCursor(cursor);
        }
        SDL_FreeSurface(cursorSurface);
    }
}
// setup window
void setup(){
    // setup_cursor();
    rect_dialog_bg = (SDL_Rect){
        5 * VW,
        70 * VH,
        90 * VW,
        20 * VH
    };
    rect_background = (SDL_Rect){
        0,
        0,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    };
}
// process input
void process_input(){
    SDL_Event event; // occur in the application, such as a key press or a mouse movement
    int32_t condition=0;
    while(1)
    {
        while(SDL_PollEvent(&event)) // 持續檢查事件
        {
            switch(event.type){
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_SPACE)
                        condition = 1;
                    break;
            }
            if(condition)
            {
                break;
            }
        }
        if(condition)
        {
            break;
        }
    }
}
// render window
void render_background(char *event){
    char *backgroundString = get_background(event);
    char filePath[128];
    snprintf(filePath, sizeof(filePath), "img/%s", backgroundString);
    SDL_Surface* surface = IMG_Load(filePath); 
    if (!surface) {
        printf("Error creating surface: %s\n", IMG_GetError());
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Error creating texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface); // Make sure to free the surface before returning
        return;
    }
    SDL_RenderCopy(renderer, texture, NULL, &rect_background); 
    SDL_RenderPresent(renderer);
    SDL_FreeSurface(surface);
}
// render description
void render_description(char *event){
    SDL_SetRenderDrawColor(renderer, 110, 120, 170, 0.8 * 255); // light blue color
    SDL_RenderFillRect(renderer, &rect_dialog_bg);
    // Update logic integrated here
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    last_frame_time = SDL_GetTicks();
    // Adjust text display speed, every 100 milliseconds display a new character
    if (SDL_GetTicks() - start_time >= 100) {
        start_time = SDL_GetTicks();
        if (currentWordIndex < totalWords) {
            currentWordIndex++;
        }
    }
    char *textString = event_description(event);
    int len = strlen(textString);
    if (words != NULL) {
        for (int i = 0; i < totalWords; i++) {
            free(words[i]);
        }
        free(words);
    }
    words = split_utf8_string(textString, &totalWords);
    
    TTF_Init();
    font = TTF_OpenFont("NotoSansTC.ttf", 24);
    if (!font) {
        printf("Error loading font: %s\n", TTF_GetError());
        return;
    }
    char *textToRender = calloc(1000, sizeof(char));
    for (int i = 0; i <= currentWordIndex && i < totalWords; i++) {
        strcat(textToRender, words[i]);
    }
    SDL_Color color = {255, 255, 255, 255};  // white color
    SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, textToRender, color);
    
    if (!textSurface) {
        printf("Error creating text surface: %s\n", TTF_GetError());
        return;
    }
    int textWidth = 0;
    int textHeight = 0;
    if (TTF_SizeUTF8(font, textToRender, &textWidth, &textHeight) != 0) {
        printf("Error getting text size: %s\n", TTF_GetError());
        return;
    }
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        printf("Error creating text texture: %s\n", SDL_GetError());
        return;
    }
    textRect = (SDL_Rect){
        5*VW,
        70*VH,
        textWidth,
        textHeight
    };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_RenderPresent(renderer);
    free(textToRender);
    SDL_FreeSurface(textSurface);
}
// render character
void render_background_character(char *event){
    char *characterString = background_character(event);
    char filePath[128];
    snprintf(filePath, sizeof(filePath), "img/%s", characterString);
    SDL_Surface* characterSurface = IMG_Load(filePath);
    if (!characterSurface) {
        printf("Error creating character surface: %s\n", IMG_GetError());
        return;
    }
    SDL_Texture* characterTexture = SDL_CreateTextureFromSurface(renderer, characterSurface);
    if (!characterTexture) {
        printf("Error creating character texture: %s\n", SDL_GetError());
        SDL_FreeSurface(characterSurface); // Make sure to free the surface before returning
        return;
    }
    SDL_Rect characterRect = (SDL_Rect){
        40*VW,
        20*VH,
        20*VW,
        20*VH
    };
    SDL_RenderCopy(renderer, characterTexture, NULL, &characterRect);
    SDL_RenderPresent(renderer);
    SDL_FreeSurface(characterSurface);
}
// destroy window
void destroy_window(){
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
    TTF_Quit();
}