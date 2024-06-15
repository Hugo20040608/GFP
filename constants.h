#pragma once
#define TRUE 1
#define FALSE 0
#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)
double WINDOW_WIDTH = 1920;
double WINDOW_HEIGHT = 1080;
#define VW WINDOW_WIDTH / 100
#define VH WINDOW_HEIGHT / 100

SDL_Rect rect_dialog_bg;
SDL_Rect textRect;
SDL_Rect rect_background;
SDL_Rect r_btn_postion;
SDL_Rect d_btn_postion;
int game_is_running = FALSE; 
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Renderer *renderer_text = NULL;
SDL_Texture *texture = NULL;
TTF_Font* font = NULL;
SDL_Texture* textTexture = NULL;

int start_time = 0;
int last_frame_time = 0;
int currentWordIndex = 0;
int totalWords = 0;
char **words = NULL;
char STORY_FILE_NAME[128];
