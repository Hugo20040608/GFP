#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "toml.h"
#include "fiction.h"
#include "constants.h"

char *get_background(char *event)
{
    FILE *fp = fopen(STORY_FILE_NAME, "r");
    char errbuf[200];
    toml_table_t* index = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (index == 0)
    {
        printf("Error: %s\n", errbuf);
        return NULL;
    }
    toml_table_t* event_table = toml_table_in(index, "events");
    if (event == 0)
    {
        printf("Error: %s\n", "events not found");
        return NULL;
    }
    toml_table_t* target_event = toml_table_in(event_table, event);
    if (target_event == 0)
    {
        printf("Error: %s\n", "event not found");
        return NULL;
    }
    toml_datum_t background = toml_string_in(target_event, "scene_id");
    if (!background.ok)
    {
        // printf("Error: %s\n", "scene_id not found");
        return NULL;
    }
    fclose(fp);
    return get_background_image(background.u.s);
}

char *event_description(char *event)
{
    FILE *fp = fopen(STORY_FILE_NAME, "r");
    char errbuf[200];
    toml_table_t* index = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (index == 0)
    {
        printf("Error: %s\n", errbuf);
        return NULL;
    }
    toml_table_t* event_table = toml_table_in(index, "events");
    if (event == 0)
    {
        printf("Error: %s\n", "events not found");
        return NULL;
    }
    toml_table_t* target_event = toml_table_in(event_table, event);
    if (target_event == 0)
    {
        printf("Error: %s\n", "event not found");
        return NULL;
    }
    toml_datum_t description = toml_string_in(target_event, "description");
    if (!description.ok)
    {
        // printf("Error: %s\n", "description not found");
        return NULL;
    }
    fclose(fp);
    return description.u.s;
}

char *background_character(char *event)
{
    FILE *fp = fopen(STORY_FILE_NAME, "r");
    char errbuf[200];
    toml_table_t* index = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (index == 0)
    {
        printf("Error: %s\n", errbuf);
        return NULL;
    }
    toml_table_t* event_table = toml_table_in(index, "events");
    if (event == 0)
    {
        printf("Error: %s\n", "events not found");
        return NULL;
    }
    toml_table_t* target_event = toml_table_in(event_table, event);
    if (target_event == 0)
    {
        printf("Error: %s\n", "event not found");
        return NULL;
    }
    toml_datum_t character = toml_string_in(target_event, "character_id");
    if (!character.ok)
    {
        // printf("Error: %s\n", "character_id not found");
        return NULL;
    }
    fclose(fp);
    return get_character_image(character.u.s);
}

toml_array_t *get_dialogue_array(char *event)
{
    FILE *fp = fopen(STORY_FILE_NAME, "r");
    char errbuf[200];
    toml_table_t* index = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (index == 0)
    {
        printf("Error: %s\n", errbuf);
        return NULL;
    }
    toml_table_t* event_table = toml_table_in(index, "events");
    if (event == 0)
    {
        printf("Error: %s\n", "events not found");
        return NULL;
    }
    toml_table_t* target_event = toml_table_in(event_table, event);
    if (target_event == 0)
    {
        printf("Error: %s\n", "event not found");
        return NULL;
    }
    toml_array_t* dialogue = toml_array_in(target_event, "dialogue");
    if (dialogue == 0)
    {
        // printf("Error: %s\n", "dialogue not found");
        return NULL;
    }
    fclose(fp);
    return dialogue;
}

toml_array_t *get_choices_array(char *event)
{
    FILE *fp = fopen(STORY_FILE_NAME, "r");
    char errbuf[200];
    toml_table_t* index = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (index == 0)
    {
        printf("Error: %s\n", errbuf);
        return NULL;
    }
    toml_table_t* event_table = toml_table_in(index, "events");
    if (event == 0)
    {
        printf("Error: %s\n", "events not found");
        return NULL;
    }
    toml_table_t* target_event = toml_table_in(event_table, event);
    if (target_event == 0)
    {
        printf("Error: %s\n", "event not found");
        return NULL;
    }
    toml_array_t* choices = toml_array_in(target_event, "choices");
    if (choices == 0)
    {
        // printf("Error: %s\n", "choices not found");
        return NULL;
    }
    fclose(fp);
    return choices;
}

char *get_background_image(char *scene)
{
    FILE *fp = fopen(STORY_FILE_NAME, "r");
    char errbuf[200];
    toml_table_t* index = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (index == 0)
    {
        printf("Error: %s\n", errbuf);
        return NULL;
    }
    toml_table_t* scene_table = toml_table_in(index, "scenes");
    if (scene == 0)
    {
        printf("Error: %s\n", "scenes not found");
        return NULL;
    }
    toml_table_t* target_scene = toml_table_in(scene_table, scene);
    if (target_scene == 0)
    {
        printf("Error: %s\n", "scene not found");
        return NULL;
    }
    toml_datum_t background = toml_string_in(target_scene, "background_image");
    if (!background.ok)
    {
        // printf("Error: %s\n", "background not found");
        return NULL;
    }
    fclose(fp);
    return background.u.s;
}

char *get_character_image(char *character)
{
    FILE *fp = fopen(STORY_FILE_NAME, "r");
    char errbuf[200];
    toml_table_t* index = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (index == 0)
    {
        printf("Error: %s\n", errbuf);
        return NULL;
    }
    toml_table_t* character_table = toml_table_in(index, "characters");
    if (character == 0)
    {
        printf("Error: %s\n", "characters not found");
        return NULL;
    }
    toml_table_t* target_character = toml_table_in(character_table, character);
    if (target_character == 0)
    {
        printf("Error: %s\n", "character not found");
        return NULL;
    }
    toml_datum_t image = toml_string_in(target_character, "portrait_image");
    if (!image.ok)
    {
        // printf("Error: %s\n", "image not found");
        return NULL;
    }
    fclose(fp);
    return image.u.s;
}

int32_t check_endding(char *event)
{
    FILE *fp = fopen(STORY_FILE_NAME, "r");
    char errbuf[200];
    toml_table_t* index = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (index == 0)
    {
        printf("Error: %s\n", errbuf);
        return 1;
    }
    toml_table_t* event_table = toml_table_in(index, "events");
    if (event == 0)
    {
        printf("Error: %s\n", "events not found");
        return 1;
    }
    toml_table_t* target_event = toml_table_in(event_table, event);
    if (target_event == 0)
    {
        printf("Error: %s\n", "event not found");
        return 1;
    }
    toml_datum_t endding = toml_int_in(target_event, "endding");
    if (!endding.ok)
    {
        // printf("Error: %s\n", "endding not found");
        return 0;
    }
    fclose(fp);
    return endding.u.i;
}
