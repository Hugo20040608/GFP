#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "toml.h"
#include "fiction.h"

char *get_background(char *event)
{
    FILE *fp = fopen("new.toml", "r");
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
    return background.u.s;
}

char *background_description(char *event)
{
    FILE *fp = fopen("new.toml", "r");
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
    FILE *fp = fopen("new.toml", "r");
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
    return character.u.s;
}

toml_array_t *get_dialogue_array(char *event)
{
    FILE *fp = fopen("new.toml", "r");
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
    FILE *fp = fopen("new.toml", "r");
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