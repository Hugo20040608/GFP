#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
// #include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
#include "toml.h"
#include "fiction.h"

char *background_switch(char *event);
char *event_description(char *event);
toml_array_t *get_array_type(char *event);