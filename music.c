#include "music.h"
Mix_Music *music;
void play_music(char *song_name){
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }
    char path[100] = "music/";
    strcat(path, song_name);
    music = Mix_LoadMUS(path);
    if(!music) {
        printf("Failed to load music: %s\n", Mix_GetError());
    }
    Mix_PlayMusic(music, -1);
}
void free_music(){
    Mix_FreeMusic(music);
    Mix_CloseAudio();
}