#pragma once
char** split_utf8_string(char* str, int* count) {
    int length = strlen(str);
    char** result = NULL;
    int num_chars = 0;
    int i = 0;

    while (i < length) {
        int char_len = 1;
        // 檢查UTF-8字元的長度
        if ((str[i] & 0x80) == 0x00) { // 0xxxxxxx
            // 1-byte character
            char_len = 1;
        } else if ((str[i] & 0xE0) == 0xC0) { // 110xxxxx
            // 2-byte character
            char_len = 2;
        } else if ((str[i] & 0xF0) == 0xE0) { // 1110xxxx
            // 3-byte character
            char_len = 3;
        } else if ((str[i] & 0xF8) == 0xF0) { // 11110xxx
            // 4-byte character
            char_len = 4;
        }

        // 分配記憶體給新的字元
        result = realloc(result, sizeof(char*) * (num_chars + 1));
        result[num_chars] = calloc(char_len + 1, sizeof(char));
        memcpy(result[num_chars], &str[i], char_len);
        num_chars++;
        i += char_len;
    }

    *count = num_chars;
    return result;
}