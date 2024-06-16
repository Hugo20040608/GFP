#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

int main() {
    // 創建 JSON 對象
    cJSON *json = cJSON_CreateObject();

    // 添加數據到 JSON
    cJSON_AddStringToObject(json, "name", "John");
    cJSON_AddNumberToObject(json, "age", 30);

    // 將 JSON 對象轉換成字符串
    char *json_data = cJSON_Print(json);
    printf("%s\n", json_data);

    // 釋放 JSON 對象記憶體
    cJSON_Delete(json);
    free(json_data);

    return 0;
}
