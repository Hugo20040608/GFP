#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "toml.h"
#include "fiction.h"

char *get_background(char *event, char *STORY_FILE_NAME);
char *event_description(char *event, char *STORY_FILE_NAME);
char *background_character(char *event, char *STORY_FILE_NAME);
toml_array_t *get_dialogue_array(char *event, char *STORY_FILE_NAME);
toml_array_t *get_choices_array(char *event, char *STORY_FILE_NAME);
char *get_background_image(char *scene, char *STORY_FILE_NAME);
char *get_character_image(char *character, char *STORY_FILE_NAME);
char *check_endding(char *event, char *STORY_FILE_NAME);
char *check_item(char *event, char *STORY_FILE_NAME);
void get_table_name_list(char **table_name_list ,char *table, char *STORY_FILE_NAME);
int32_t get_table_size(char *table, char *STORY_FILE_NAME);
char *get_item_description(char *item, char *STORY_FILE_NAME);
char *get_item_image(char *item, char *STORY_FILE_NAME);
int32_t get_choice_table_size(char *event, char *STORY_FILE_NAME);
char *get_item_id(char event, char *STORY_FILE_NAME);
