#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "toml.h"
#include "fiction.h"

char *get_background(char *event, char *STORY_FILE_NAME)
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
    return get_background_image(background.u.s, STORY_FILE_NAME);
}

char *event_description(char *event, char *STORY_FILE_NAME)
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

char *background_character(char *event, char *STORY_FILE_NAME)
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
    return get_character_image(character.u.s, STORY_FILE_NAME);
}

toml_array_t *get_dialogue_array(char *event, char *STORY_FILE_NAME)
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

toml_array_t *get_choices_array(char *event, char *STORY_FILE_NAME)
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

char *get_background_image(char *scene, char *STORY_FILE_NAME)
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

char *get_character_image(char *character, char *STORY_FILE_NAME)
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

char *check_endding(char *event, char *STORY_FILE_NAME)
{
    FILE *fp = fopen(STORY_FILE_NAME, "r");
    char errbuf[200];
    toml_table_t* index = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (index == 0)
    {
        // printf("Error: %s\n", errbuf);
        return NULL;
    }
    toml_table_t* event_table = toml_table_in(index, "events");
    if (event == 0)
    {
        // printf("Error: %s\n", "events not found");
        return NULL;
    }
    toml_table_t* target_event = toml_table_in(event_table, event);
    if (target_event == 0)
    {
        // printf("Error: %s\n", "event not found");
        return NULL;
    }
    toml_datum_t endding = toml_string_in(target_event, "is_ending");
    if (endding.ok)
    {
        fclose(fp);
        return endding.u.s;
    }
    fclose(fp);
    return NULL;
}

char *check_item(char *event, char *STORY_FILE_NAME)
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
    toml_datum_t item = toml_string_in(target_event, "item");
    if (item.ok)
    {
        toml_table_t *item_table = toml_table_in(index, "items");
        if (item_table == 0)
        {
            printf("Error: %s\n", "items not found");
            return NULL;
        }
        toml_table_t *target_item = toml_table_in(item_table, item.u.s);
        if (target_item == 0)
        {
            printf("Error: %s\n", "item not found");
            return NULL;
        }
        toml_datum_t name = toml_string_in(target_item, "name");
        if (!name.ok)
        {
            // printf("Error: %s\n", "name not found");
            return NULL;
        }
        fclose(fp);
        return name.u.s;
    }
    fclose(fp);
    return NULL;
}

void get_table_name_list(char **table_name_list ,char *table, char *STORY_FILE_NAME)
{
    FILE *fp = fopen(STORY_FILE_NAME, "r");
    char errbuf[200];
    toml_table_t* index = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (index == 0)
    {
        printf("Error: %s\n", errbuf);
        return;
    }
    toml_table_t *target_table = toml_table_in(index, table);
    if (target_table == 0)
    {
        printf("Error: %s\n", table);
        return;
    }
    toml_table_t *tmp_table = NULL;
    char key_str[30] = {0};
    strcpy(key_str, table);
    key_str[strlen(key_str) - 1] = '\0';
    strcat(key_str, "_");
    for(int i = 0; i < toml_table_ntab(target_table); i++)
    {
        snprintf(key_str + strlen(key_str), 20 - strlen(key_str), "%d", i+1);
        tmp_table = toml_table_in(target_table, key_str);
        toml_datum_t name = toml_string_in(tmp_table, "name");
        if (!name.ok)
        {
            // printf("Error: %s\n", "name not found");
            return;
        }
        table_name_list[i] = name.u.s;
        memset(key_str, 0, 30);
        strcpy(key_str, table);
        key_str[strlen(key_str) - 1] = '\0';
        strcat(key_str, "_");
    }
}

int32_t get_table_size(char *table, char *STORY_FILE_NAME)
{
    FILE *fp = fopen(STORY_FILE_NAME, "r");
    char errbuf[200];
    toml_table_t* index = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (index == 0)
    {
        printf("Error: %s\n", errbuf);
        return -1;
    }
    toml_table_t* target_table = toml_table_in(index, table);
    if (target_table == 0)
    {
        printf("Error: %s\n", table);
        return -1;
    }
    fclose(fp);
    return toml_table_ntab(target_table);
}

char *get_item_description(char *item, char *STORY_FILE_NAME)
{
    FILE *fp = fopen(STORY_FILE_NAME, "r");
    char errbuf[200];
    toml_table_t* index = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (index == 0)
    {
        printf("Error: %s\n", errbuf);
        return NULL;
    }
    toml_table_t *item_table = toml_table_in(index, "items");
    if (item == 0)
    {
        printf("Error: %s\n", "items not found");
        return NULL;
    }
    toml_table_t *target_item = toml_table_in(item_table, item);
    if (target_item == 0)
    {
        printf("Error: %s\n", "item not found");
        return NULL;
    }
    toml_datum_t description = toml_string_in(target_item, "description");
    if (!description.ok)
    {
        // printf("Error: %s\n", "description not found");
        return NULL;
    }
    fclose(fp);
    return description.u.s;
}

char *get_item_image(char *item, char *STORY_FILE_NAME)
{
    FILE *fp = fopen(STORY_FILE_NAME, "r");
    char errbuf[200];
    toml_table_t* index = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (index == 0)
    {
        printf("Error: %s\n", errbuf);
        return NULL;
    }
    toml_table_t *item_table = toml_table_in(index, "items");
    if (item == 0)
    {
        printf("Error: %s\n", "items not found");
        return NULL;
    }
    toml_table_t *target_item = toml_table_in(item_table, item);
    if (target_item == 0)
    {
        printf("Error: %s\n", "item not found");
        return NULL;
    }
    toml_datum_t image = toml_string_in(target_item, "icon_image");
    if (!image.ok)
    {
        // printf("Error: %s\n", "image not found");
        return NULL;
    }
    fclose(fp);
    return image.u.s;
}

int32_t get_choice_table_size(char *event, char *STORY_FILE_NAME)
{
    FILE *fp = fopen(STORY_FILE_NAME, "r");
    char errbuf[200];
    toml_table_t *index = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (index == 0)
    {
        printf("Error: %s\n", errbuf);
        return -1;
    }
    toml_table_t *event_table = toml_table_in(index, "events");
    if (event == 0)
    {
        printf("Error: %s\n", "events not found");
        return -1;
    }
    toml_table_t *target_event = toml_table_in(event_table, event);
    if (target_event == 0)
    {
        printf("Error: %s\n", "event not found");
        return -1;
    }
    toml_array_t *choices = toml_array_in(target_event, "choices");
    if (choices == 0)
    {
        // printf("Error: %s\n", "choices not found");
        return -1;
    }
    fclose(fp);
    return toml_array_nelem(choices);
}

char *get_item_id(char *event, char *STORY_FILE_NAME)
{
    FILE *fp = fopen(STORY_FILE_NAME, "r");
    char errbuf[200];
    toml_table_t *index = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (index == 0)
    {
        printf("Error: %s\n", errbuf);
        return NULL;
    }
    toml_table_t *event_table = toml_table_in(index, "events");
    if (event == 0)
    {
        printf("Error: %s\n", "events not found");
        return NULL;
    }
    toml_table_t *target_event = toml_table_in(event_table, event);
    if (target_event == 0)
    {
        printf("Error: %s\n", "event not found");
        return NULL;
    }
    toml_datum_t item = toml_string_in(target_event, "item");
    if (!item.ok)
    {
        // printf("Error: %s\n", "item not found");
        return NULL;
    }
    fclose(fp);
    return item.u.s;
}
