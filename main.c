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
#include "toml.h"

int32_t initialize_window();
void setup();
void render_background(char *event);
void render_description(char *event);
void render_background_character(char *event);
void destroy_window();
void render_dialogue(char *character_id, char *text, char *event);
void render_character(char *character_image);
void render_text(char *text);
void render_choice(toml_array_t *choices, int32_t *choice);
void process_input_space();
int32_t detect_user_input_number();

int main(int argc, char *argv[]){
    printf("Please enter the name of the story file: ");
    fscanf(stdin, "%s", STORY_FILE_NAME);
    game_is_running = initialize_window();
    if(!game_is_running){
        return 1;
    }
    setup();
    play_music();
    char event[128] = "event_1"; // start from event_1
    // game loop
    while(game_is_running){
        // part 1 (背景、描述、背景人物)
        render_background(event);
        if(background_character(event) != NULL)
            render_background_character(event);
        currentWordIndex = 0;
        totalWords = 0;
        while(1){
            render_description(event);
            if(currentWordIndex >= totalWords){
                process_input_space();
                break;
            }
        }
        // part 2 (對話)
        toml_array_t *dialogue = get_dialogue_array(event);
        if (dialogue != NULL){
            for(int32_t i=0;;i++){
                toml_table_t *dialogue_table = toml_table_at(dialogue, i);
                if (dialogue_table == 0){
                    break;
                }
                toml_datum_t character_id = toml_string_in(dialogue_table, "character_id");
                toml_datum_t text = toml_string_in(dialogue_table, "text");
                if (!character_id.ok || !text.ok){
                    // printf("Error: %s\n", "character_id or text not found");
                    return 0;
                }
                render_dialogue(character_id.u.s, text.u.s, event);
                free(character_id.u.s);
                free(text.u.s);
                free (dialogue_table);
            }
        }
        free(dialogue);
        if(check_endding(event)){
            game_is_running = FALSE;
            break;
        }
        // part 3 (選項)
        int32_t choice = 0;
        toml_array_t *choices = get_choices_array(event);
        if (choices != NULL){
            render_choice(choices, &choice);
            free(choices);
        }
        // part 4 (選擇後的事件)
        toml_table_t *choice_table = toml_table_at(choices, choice-1);
        if (choice_table != NULL){
            toml_datum_t next_event_datum = toml_string_in(choice_table, "next_event_id");
            if (!next_event_datum.ok){
                // printf("Error: %s\n", "next_event not found");
                return 0;
            }
            strncpy(event, next_event_datum.u.s, sizeof(event));
        }
        else{
            game_is_running = FALSE;
            break;
        }
        free(choice_table);
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
void process_input_space(){
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
    render_text(event_description(event));
}
// render character
void render_background_character(char *event){
    render_character(background_character(event));
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

void render_dialogue(char *character_id, char *text, char *event){
    // 蓋掉角色和對話框
    render_background(event);
    // render character
    render_character(get_character_image(character_id));
    // render text
    currentWordIndex = 0;
    totalWords = 0;
    while (1)
    {
        render_text(text);
        if(currentWordIndex >= totalWords){
            process_input_space();
            break;
        }
    }
}

void render_character(char *character_image){
    char filePath[128];
    snprintf(filePath, sizeof(filePath), "img/%s", character_image);
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
        50*VH
    };
    SDL_RenderCopy(renderer, characterTexture, NULL, &characterRect);
    SDL_RenderPresent(renderer);
    SDL_FreeSurface(characterSurface);
}

void render_text(char *text){
    SDL_SetRenderDrawColor(renderer, 110, 120, 170, 0.8 * 255); // light blue color
    SDL_RenderFillRect(renderer, &rect_dialog_bg);
    // Update logic integrated here
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    last_frame_time = SDL_GetTicks();
    // Adjust text display speed, every 30 milliseconds display a new character
    if (SDL_GetTicks() - start_time >= 30) {
        start_time = SDL_GetTicks();
        if (currentWordIndex < totalWords) {
            currentWordIndex++;
        }
    }
    if (words != NULL) {
        for (int i = 0; i < totalWords; i++) {
            free(words[i]);
        }
        free(words);
    }
    words = split_utf8_string(text, &totalWords);
    
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
    for(int i=0; i<totalWords; i++){
        free(words[i]);
    }
    free(words);
    words = NULL;
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

void render_choice(toml_array_t *choices, int32_t *choice){
    // 把選項連結起來，並且加上選項編號和換行符號
    char choice_string[1000]={0};
    char index[10]={0};
    for(int32_t i=0;;i++){
        toml_table_t *choice_table = toml_table_at(choices, i);
        if (choice_table == 0){
            break;
        }
        toml_datum_t choice = toml_string_in(choice_table, "text");
        if (!choice.ok){
            // printf("Error: %s\n", "choice not found");
            return;
        }
        snprintf(index, sizeof(index), "(%d)", i+1);
        strcat(choice_string, index);
        strcat(choice_string, choice.u.s);
        strcat(choice_string, "    ");
    }
    currentWordIndex = 0;
    totalWords = 0;
    while (1)
    {
        render_text(choice_string);
        if(currentWordIndex >= totalWords){
            *choice = detect_user_input_number();
            break;
        }
    }
}

int32_t detect_user_input_number(){
    SDL_Event event;
    int number = 0;
    int isNumberEntered = 0;

    while (!isNumberEntered) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) {
                    number = event.key.keysym.sym - SDLK_0;
                    isNumberEntered = 1;
                }
            }
        }
    }

    return number;
}
