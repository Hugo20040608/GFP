#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

void find_response_by_cjson() {
    FILE *pFile;
    long lSize;
    char *my_json_string;
    size_t result;
    pFile = fopen("message.txt", "r");
    if (pFile == NULL) {
        fprintf(stderr, "錯誤：無法打開 message.txt\n");
        return;
    }
    fseek(pFile, 0, SEEK_END);
    lSize = ftell(pFile);
    rewind(pFile);
    my_json_string = (char *)malloc(sizeof(char) * (lSize + 1));
    if (my_json_string == NULL) {
        fputs("記憶體錯誤", stderr);
        fclose(pFile);
        return;
    }
    result = fread(my_json_string, 1, lSize, pFile);
    if (result != lSize) {
        fputs("讀取錯誤", stderr);
        fclose(pFile);
        free(my_json_string);
        return;
    }
    my_json_string[lSize] = '\0';
    fclose(pFile);
    printf("檔案內容：%s\n", my_json_string);
    // 解析 JSON 字串
    cJSON *json = cJSON_Parse(my_json_string);
    free(my_json_string);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "錯誤位置：%s\n", error_ptr);
        }
        return;
    }
    // 提取 "choices" 陣列
    cJSON *choices = cJSON_GetObjectItemCaseSensitive(json, "choices");
    if (!cJSON_IsArray(choices)) {
        fprintf(stderr, "錯誤：choices 不是一個陣列\n");
        cJSON_Delete(json);
        return;
    }
    // 取得陣列的第一個元素
    cJSON *first_choice = cJSON_GetArrayItem(choices, 0);
    if (first_choice == NULL) {
        fprintf(stderr, "錯誤：無法取得 choices 中的第一個項目\n");
        cJSON_Delete(json);
        return;
    }
    // 從第一個元素中提取 "message" 物件
    cJSON *message = cJSON_GetObjectItemCaseSensitive(first_choice, "message");
    if (message == NULL) {
        fprintf(stderr, "錯誤：無法從第一個選項中取得 message\n");
        cJSON_Delete(json);
        return;
    }
    // 從 "message" 物件中提取 "content" 欄位
    cJSON *content = cJSON_GetObjectItemCaseSensitive(message, "content");
    if (!cJSON_IsString(content)) {
        fprintf(stderr, "錯誤：content 不是一個字串\n");
        cJSON_Delete(json);
        return;
    }
    // 印出 content 欄位的值
    printf("內容：%s\n", content->valuestring);
    pFile = fopen("input.txt", "ab");
    fwrite(content->valuestring, 1, strlen(content->valuestring), pFile);
    fclose(pFile);
    printf("內容已寫入 input.txt\n");
    // 清理 cJSON 物件
    cJSON_Delete(json);
    return;
}
