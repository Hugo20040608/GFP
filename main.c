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

int32_t initialize_window(); // 初始化視窗
void setup(); // 設定視窗
void destroy_window(); // 關閉視窗
// --------------------------------------------------------------------------
void render_background(char *event); // 渲染背景
void render_description(char *event); // 渲染描述
void render_background_character(char *event); // 渲染背景人物
void render_dialogue(char *character_id, char *text, char *event); // 渲染對話
void render_choice(toml_array_t *choices, int32_t *choice, char *event); // 渲染選項
// --------------------------------------------------------------------------
void render_character(char *character_image); // 渲染角色
void render_text(char *text); // 渲染文字
void render_item(char *item_image); // 渲染物品
// --------------------------------------------------------------------------
void process_input_space(); // 處理空白鍵
int32_t detect_user_input_number(int32_t range); // 檢測用戶輸入數字
void detect_user_input_RD(char *key_in); // 檢測用戶輸入R或D
// --------------------------------------------------------------------------
void read_database_start(char *database, char *event); // 讀取數據庫開始
void save_event_to_database(char *database, char *event); // 保存事件到數據庫
void save_item_to_database(char *database, char *item); // 更新存檔的物品
void create_database(char *database, char *STORY_FILE_NAME); // 創建數據庫
// --------------------------------------------------------------------------
void open_screen(char *key_in); // 開場畫面
void end_screen_fail(); // 失敗畫面
void end_screen_success(); // 成功畫面

int main(int argc, char *argv[]){
    printf("Please enter the name of the story file: ");
    fscanf(stdin, "%s", STORY_FILE_NAME);
    game_is_running = initialize_window();
    if(!game_is_running){
        return 1;
    }
    setup();
    char key_in[2] = {0};
    open_screen(key_in);
    if(!game_is_running){
        destroy_window();
        free_music();
        return 0;
    }
    if(strcmp(key_in, "r") == 0){
        create_database("database.txt", STORY_FILE_NAME);
    }
    play_music("raining_village.mp3");
    char event[100] = {0}; // start from database save_event
    read_database_start("database.txt", event);
    // 遊戲迴圈，直到遊戲結束 (兩種結束方式: 1. 遊戲結束 2. 用戶按下ESC)
    while(game_is_running){
        // part 1 (背景、描述、背景人物)
        SDL_RenderClear(renderer);
        render_background(event);
        if(background_character(event, STORY_FILE_NAME) != NULL)
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
        // 這是 "1. 遊戲結束" 的條件
        if(check_endding(event, STORY_FILE_NAME) != NULL){
            game_is_running = FALSE;
            break;
        }
        if(!game_is_running){
            save_event_to_database("database.txt", event);
            destroy_window();
            free_music();
            return 0;
        }
        // part 2 (對話)
        toml_array_t *dialogue = get_dialogue_array(event, STORY_FILE_NAME);
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
                if(!game_is_running){
                    break;
                }
                free(character_id.u.s);
                free(text.u.s);
                free (dialogue_table);
            }
        }
        if(!game_is_running){
            free(dialogue);
            save_event_to_database("database.txt", event);
            destroy_window();
            free_music();
            return 0;
        }
        free(dialogue);
        // small part (檢查物品)
        char *item = check_item(event, STORY_FILE_NAME);
        char *item_id = get_item_id(event, STORY_FILE_NAME);
        if(item != NULL){
            char *message = get_item_description(item_id, STORY_FILE_NAME);
            char *item_image = get_item_image(item_id, STORY_FILE_NAME);
            render_item(item_image);
            free(item_image);
            currentWordIndex = 0;
            totalWords = 0;
            while(1){
                render_text(message);
                if(currentWordIndex >= totalWords){
                    process_input_space();
                    break;
                }
            }
            save_item_to_database("database.txt", item);
        }
        if(!game_is_running){
            save_event_to_database("database.txt", event);
            destroy_window();
            free_music();
            return 0;
        }
        free(item);
        // part 3 (選項)
        int32_t choice = 0;
        toml_array_t *choices = get_choices_array(event, STORY_FILE_NAME);
        if (choices != NULL){
            SDL_RenderClear(renderer);
            render_background(event);
            render_choice(choices, &choice, event);
            free(choices);
        }
        if(!game_is_running){
            save_event_to_database("database.txt", event);
            destroy_window();
            free_music();
            return 0;
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
        free(choice_table);
    }
    free_music();
    if(strcmp(check_endding(event, STORY_FILE_NAME), "success") == 0){
        end_screen_success();
        create_database("database.txt", STORY_FILE_NAME);
    }
    else{
        end_screen_fail();
        create_database("database.txt", STORY_FILE_NAME);
    }
    destroy_window();
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
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
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
    r_btn_postion = (SDL_Rect){
        113,
        594,
        636,
        144
    };
    d_btn_postion = (SDL_Rect){
        113,
        848,
        847,
        144
    };
}
// open screen
void open_screen(char *key_in){
    play_music("op.mp3");
    SDL_Surface* surface = IMG_Load("img/game_start.png");
    if (!surface) {
        printf("Error creating surface: %s\n", IMG_GetError());
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Error creating texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface); // Make sure to free the surface before returning
        return;
    }
    SDL_RenderCopy(renderer, texture, NULL, &rect_background);
    // render r button
    surface = IMG_Load("img/game_start_r_btn.png");
    if (!surface) {
        printf("Error creating surface: %s\n", IMG_GetError());
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Error creating texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface); // Make sure to free the surface before returning
        return;
    }
    SDL_RenderCopy(renderer, texture, NULL, &r_btn_postion);
    // render d button
    surface = IMG_Load("img/game_start_d_btn.png");
    if (!surface) {
        printf("Error creating surface: %s\n", IMG_GetError());
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Error creating texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface); // Make sure to free the surface before returning
        return;
    }
    SDL_RenderCopy(renderer, texture, NULL, &d_btn_postion);
    SDL_RenderPresent(renderer);
    detect_user_input_RD(key_in);
    free_music();
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
                    else if(event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        condition = 1;
                        game_is_running = FALSE;
                    }
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
    char *backgroundString = get_background(event, STORY_FILE_NAME);
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
    render_text(event_description(event, STORY_FILE_NAME));
}
// render character
void render_background_character(char *event){
    render_character(background_character(event, STORY_FILE_NAME));
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
    SDL_RenderClear(renderer);
    // 蓋掉角色和對話框
    render_background(event);
    // render character
    render_character(get_character_image(character_id, STORY_FILE_NAME));
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
    int maxWidth = 90 * VW - 10;
    SDL_Surface *textSurface = TTF_RenderUTF8_Blended_Wrapped(font, textToRender, color, maxWidth);
    
    if (!textSurface) {
        printf("Error creating text surface: %s\n", TTF_GetError());
        return;
    }
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        printf("Error creating text texture: %s\n", SDL_GetError());
        return;
    }
    textRect = (SDL_Rect) {
        5 * VW + 10,
        70 * VH + 10,
        textSurface->w,
        textSurface->h
    };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_RenderPresent(renderer);
    free(textToRender);
    SDL_FreeSurface(textSurface);
}

void render_choice(toml_array_t *choices, int32_t *choice, char *event){
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
            *choice = detect_user_input_number(get_choice_table_size(event, STORY_FILE_NAME));
            break;
        }
    }
}

int32_t detect_user_input_number(int32_t range){
    SDL_Event event;
    int number = 0;
    int isNumberEntered = 0;

    while (!isNumberEntered) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9 && event.key.keysym.sym - SDLK_0 <= range && event.key.keysym.sym - SDLK_0 > 0) {
                    number = event.key.keysym.sym - SDLK_0;
                    isNumberEntered = 1;
                }
                else if(event.key.keysym.sym == SDLK_ESCAPE){
                    game_is_running = FALSE;
                    isNumberEntered = 1;
                }
            }
        }
    }

    return number;
}

void read_database_start(char *database, char *event)
{
    FILE *fp = fopen(database, "r");
    if (fp == NULL) {
        printf("Error opening database file\n");
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if(strstr(line, "save_event") != NULL){
            break;
        }
    }
    fgets(line, sizeof(line), fp);
    for(int32_t i=0; i<strlen(line); i++){
        if(line[i] == '\n'){
            line[i] = '\0';
        }
    }
    strcpy(event, line);
    fclose(fp);
}

void save_event_to_database(char *database, char *event)
{
    FILE *fp = fopen(database, "r");
    if (fp == NULL) {
        printf("Error opening database file\n");
        return;
    }
    char line[256];
    int32_t counter = 0;
    while (fgets(line, sizeof(line), fp)) {
        counter++;
    }
    char **database_content = calloc(counter, sizeof(char *));
    for(int32_t i=0; i<counter; i++){
        database_content[i] = calloc(100, sizeof(char));
    }
    fseek(fp, 0, SEEK_SET);
    for(int32_t i=0; i<counter; i++){
        fgets(line, sizeof(line), fp);
        strcpy(database_content[i], line);
    }
    fclose(fp);
    strcat(event, "\n");
    for(int32_t i=0; i<counter; i++){
        if(strstr(database_content[i], "save_event") != NULL){
            strcpy(database_content[i+1], event);
            break;
        }
    }
    fp = fopen(database, "w");
    for(int32_t i=0; i<counter; i++){
        fprintf(fp, "%s", database_content[i]);
    }
    fclose(fp);
    for(int32_t i=0; i<counter; i++){
        free(database_content[i]);
    }
    free(database_content);
    return;
}
void end_screen_fail(){
    play_music("game_over.mp3");
    SDL_Surface* surface = IMG_Load("img/game_end_fail.jpg");
    if (!surface) {
        printf("Error creating surface: %s\n", IMG_GetError());
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Error creating texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface); // Make sure to free the surface before returning
        return;
    }
    SDL_RenderCopy(renderer, texture, NULL, &rect_background);
    SDL_RenderPresent(renderer);
    process_input_space();
    free_music();
}
void end_screen_success(){
    play_music("win.mp3");
    SDL_Surface* surface = IMG_Load("img/game_end_success.jpg");
    if (!surface) {
        printf("Error creating surface: %s\n", IMG_GetError());
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Error creating texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface); // Make sure to free the surface before returning
        return;
    }
    SDL_RenderCopy(renderer, texture, NULL, &rect_background);
    SDL_RenderPresent(renderer);
    process_input_space();
    free_music();
}
// 更新存檔的物品
void save_item_to_database(char *database, char *item)
{
    FILE *fp = fopen(database, "r");
    if (fp == NULL) {
        printf("Error opening database file\n");
        return;
    }
    char line[256];
    int32_t counter = 0;
    while (fgets(line, sizeof(line), fp)) {
        counter++;
    }
    char **database_content = calloc(counter, sizeof(char *));
    for(int32_t i=0; i<counter; i++){
        database_content[i] = calloc(100, sizeof(char));
    }
    fseek(fp, 0, SEEK_SET);
    for(int32_t i=0; i<counter; i++){
        fgets(line, sizeof(line), fp);
        strcpy(database_content[i], line);
    }
    fclose(fp);
    for(int32_t i=0; i<counter; i++){
        if(strstr(database_content[i], item) != NULL){
            strcpy(database_content[i+1], "1\n");
            break;
        }
    }
    fp = fopen(database, "w");
    for(int32_t i=0; i<counter; i++){
        fprintf(fp, "%s", database_content[i]);
    }
    fclose(fp);
    for(int32_t i=0; i<counter; i++){
        free(database_content[i]);
    }
    free(database_content);
    return;
}

void detect_user_input_RD(char *key_in){
    SDL_Event event;
    int32_t condition=0;
    while(1)
    {
        while(SDL_PollEvent(&event)) // 持續檢查事件
        {
            switch(event.type){
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_r)
                    {
                        key_in[0] = 'r';
                        condition = 1;
                    }
                    else if(event.key.keysym.sym == SDLK_d)
                    {
                        key_in[0] = 'd';
                        condition = 1;
                    }
                    else if(event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        condition = 1;
                        game_is_running = FALSE;
                    }
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

void create_database(char *database, char *STORY_FILE_NAME)
{
    FILE *fp = fopen(database, "w");
    if (fp == NULL) {
        printf("Error opening database file\n");
        return;
    }
    fprintf(fp, "save_event\nevent_1\n");
    char **scene_name_list = calloc(get_table_size("scenes", STORY_FILE_NAME), sizeof(char *));
    for(int32_t i=0; i<get_table_size("scenes", STORY_FILE_NAME); i++){
        scene_name_list[i] = calloc(100, sizeof(char));
    }
    get_table_name_list(scene_name_list, "scenes", STORY_FILE_NAME);
    for(int32_t i=0; i<get_table_size("scenes", STORY_FILE_NAME); i++){
        fprintf(fp, "%s\n0\n", scene_name_list[i]);
    }
    char **character_name_list = calloc(get_table_size("characters", STORY_FILE_NAME), sizeof(char *));
    for(int32_t i=0; i<get_table_size("characters", STORY_FILE_NAME); i++){
        character_name_list[i] = calloc(100, sizeof(char));
    }
    get_table_name_list(character_name_list, "characters", STORY_FILE_NAME);
    for(int32_t i=0; i<get_table_size("characters", STORY_FILE_NAME); i++){
        fprintf(fp, "%s\n0\n", character_name_list[i]);
    }
    char **item_name_list = calloc(get_table_size("items", STORY_FILE_NAME), sizeof(char *));
    for(int32_t i=0; i<get_table_size("items", STORY_FILE_NAME); i++){
        item_name_list[i] = calloc(100, sizeof(char));
    }
    get_table_name_list(item_name_list, "items", STORY_FILE_NAME);
    for(int32_t i=0; i<get_table_size("items", STORY_FILE_NAME); i++){
        fprintf(fp, "%s\n0\n", item_name_list[i]);
    }
    fclose(fp);
}

void render_item(char *item_image){
    char filePath[128];
    snprintf(filePath, sizeof(filePath), "img/%s", item_image);
    SDL_Surface* itemSurface = IMG_Load(filePath);
    if (!itemSurface) {
        printf("Error creating item surface: %s\n", IMG_GetError());
        return;
    }
    SDL_Texture* itemTexture = SDL_CreateTextureFromSurface(renderer, itemSurface);
    if (!itemTexture) {
        printf("Error creating item texture: %s\n", SDL_GetError());
        SDL_FreeSurface(itemSurface); // Make sure to free the surface before returning
        return;
    }
    SDL_Rect itemRect = (SDL_Rect){
        70*VW,
        20*VH,
        10*VW,
        10*VH
    };
    SDL_RenderCopy(renderer, itemTexture, NULL, &itemRect);
    SDL_RenderPresent(renderer);
    SDL_FreeSurface(itemSurface);
}
