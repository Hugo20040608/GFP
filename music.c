#include "music.h"

void play_music(){
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }
    Mix_Music *music = Mix_LoadMUS("raining_village.mp3");
    if(!music) {
        printf("Failed to load music: %s\n", Mix_GetError());
    }
    Mix_PlayMusic(music, -1);
}
void free_music(){
    Mix_FreeMusic(music);
    Mix_CloseAudio();
}