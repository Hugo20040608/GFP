#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "toml.h"

void background_switch(char *event)
{
    FILE *fp = fopen("new.toml", "r");
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
    toml_datum_t background = toml_string_in(target_event, "scene_id");
    if (!background.ok)
    {
        printf("Error: %s\n", "scene_id not found");
        return 1;
    }
    SDL_Surface* surface = IMG_Load("img/%s", background.u.s);

    return;
}