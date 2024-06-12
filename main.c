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

// initialize window
int initialize_window(){
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
// update window
void update_words(){
    
}
// render window
void render_background(){
    char *backgroundString = get_background("event_1");
    // char *backgroundString = "img/manor_gate.jpg";
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
    SDL_SetRenderDrawColor(renderer, 110, 120, 170, 0.8 * 255);
    
    SDL_RenderPresent(renderer);

    SDL_FreeSurface(surface);
}
// render window
void render_description_bg(){

}
void render_description(){
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
    char *textString = background_description("event_1");
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
// void render_character(){
//     char *characterString = character_switch("event_1");
//     // char *characterString = "img/character.png";
//     snprintf(filePath, sizeof(filePath), "img/%s", characterString);
//     SDL_Surface* characterSurface = IMG_Load(filePath);
//     if (!characterSurface) {
//         printf("Error creating character surface: %s\n", IMG_GetError());
//         return;
//     }
// }
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
// main function
int main(int argc, char *argv[]){
    game_is_running = initialize_window();
    if(!game_is_running){
        return 1;
    }
    setup();
    char event[128] = "event_1";
    while(game_is_running){
        render_background(event); // check
        render_description(event); // check
        // if(character_is_visible(event))
        //     render_character(event); // update();
        // if (diaolog_is_visible(event))
        //     render_dialog(event);
        // if(endding)
        //     game_is_running = FALSE;
        // render_choice();
        process_input();
        // 找下一個event
        update_words();
    }
    destroy_window();
    return 0;
}
