#pragma once
#define TRUE 1
#define FALSE 0
#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)
double WINDOW_WIDTH = 1920;
double WINDOW_HEIGHT = 1080;
#define VW WINDOW_WIDTH / 100
#define VH WINDOW_HEIGHT / 100
struct ball{
    int x;
    int y;
    int width;
    int height;
} ball;
SDL_Rect rect_ball;
SDL_Rect textRect;
SDL_Rect rect_img;
int game_is_running = FALSE; 
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
TTF_Font* font = NULL;
SDL_Texture* textTexture = NULL;

char **words;
int currentWordIndex = 0;
int totalWords = 0;
int start_time = 0;

int last_frame_time = 0;
