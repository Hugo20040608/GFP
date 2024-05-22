#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "toml.h"

int main()
{
    FILE *fp = fopen("test.toml", "r");
    char errbuf[200];
    toml_table_t* test = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (test == 0)
    {
        printf("Error: %s\n", errbuf);
        return 1;
    }
    toml_table_t* characters = toml_table_in(test, "characters");
    if (characters == 0)
    {
        printf("Error: %s\n", "characters not found");
        return 1;
    }
    toml_table_t* heros = toml_table_in(characters, "hero");
    if (heros == 0)
    {
        printf("Error: %s\n", "hero not found");
        return 1;
    }
    toml_datum_t name= toml_string_in(heros, "name");
    if (!name.ok)
    {
        printf("Error: %s\n", "name not found");
        return 1;
    }
    printf("name: %s\n", name.u.s);
    toml_datum_t description = toml_string_in(heros, "description");
    if (!description.ok)
    {
        printf("Error: %s\n", "description not found");
        return 1;
    }
    printf("description: %s\n", description.u.s);
    puts("------------------------------------------------------");
    toml_table_t* scene = toml_table_in(test, "scenes");
    if (scene == 0)
    {
        printf("Error: %s\n", "scene not found");
        return 1;
    }
    toml_table_t* scene1 = toml_table_in(scene, "intro");
    if (scene1 == 0)
    {
        printf("Error: %s\n", "intro not found");
        return 1;
    }
    toml_datum_t scene1_background = toml_string_in(scene1, "background");
    if (!scene1_background.ok)
    {
        printf("Error: %s\n", "background not found");
        return 1;
    }
    printf("scene1_background: %s\n", scene1_background.u.s);
    
    return 0;
}