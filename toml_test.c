#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "fiction.h"
#include "toml.h"

int main()
{
    puts("---This is a example of how to print array ( get_array_type )---");
    char *event = "event_2";
    toml_array_t *dialogue = get_dialogue_array(event);
    if (dialogue == 0)
    {
        printf("Error: %s\n", "dialogue not found");
        return 1;
    }
    for(int32_t i = 0;; i++)
    {
        toml_table_t *dialogue_table = toml_table_at(dialogue, i);
        if (dialogue_table == 0)
        {
            break;
        }
        toml_datum_t character_id = toml_string_in(dialogue_table, "character_id");
        toml_datum_t text = toml_string_in(dialogue_table, "text");
        if (!character_id.ok || !text.ok)
        {
            printf("Error: %s\n", "character_id or text not found");
            return 1;
        }
        printf("%s: %s\n", character_id.u.s, text.u.s);
    }
    puts("----------------------------------------------------------------");
    return 0;
}