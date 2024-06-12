#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "toml.h"
#include "fiction.h"

char *get_background(char *event);
char *background_description(char *event);
char *background_character(char *event);
toml_array_t *get_dialogue_array(char *event);
toml_array_t *get_choices_array(char *event);