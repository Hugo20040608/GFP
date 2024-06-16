#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "APIKEY.h"
struct string {
    char *ptr;
    size_t len;
};
void init_string(struct string *s);
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
int main(void) {
    CURL *curl;
    CURLcode res;
    struct string s;
    init_string(&s);
    // 打開文件
    FILE *fp = fopen("input.txt", "rb");
    if (fp == NULL) {
        perror("Cannot open file.");
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    // 讀取文件內容
    char *data = calloc(fsize + 1, 1);
    fread(data, 1, fsize, fp);
    data[fsize] = 0;
    // 關閉文件
    fclose(fp);
    // 檢查並替換換行符
    char *p = data;
    while ((p = strchr(p, '\n')) != NULL) {
        *p = '\\';
        memmove(p + 2, p + 1, strlen(p));
        *(p + 1) = 'n';
    }
    // 將文件內容用於 POST 數據
    char post_data[1024] = {0};
    snprintf(post_data, sizeof(post_data), "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"user\", \"content\": \"%s\"}]}", data);
    printf("%s\n", post_data);
    // 釋放記憶體
    free(data);
    // 初始化 CURL
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        // 設置 CURL 選項
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.chatanywhere.tech/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
        // 設置 HTTP Header
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        char auth_header[256] = {0};
        snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", APIKEY);
        headers = curl_slist_append(headers, auth_header);curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        // 設置接收數據的回調函數和文件指針
        fp = fopen("message.txt", "wb");
        if (fp == NULL) {
            perror("Cannot open file.");
            return 1;
        }
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        // 執行 POST 請求
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Parse JSON
            cJSON *json = cJSON_Parse(s.ptr);
            if (json == NULL) {
                const char *error_ptr = cJSON_GetErrorPtr();
                if (error_ptr != NULL) {
                    fprintf(stderr, "Error before: %s\n", error_ptr);
                }
            } else {
                // Navigate to the nested structure
                cJSON *choices = cJSON_GetObjectItemCaseSensitive(json, "choices");
                cJSON *first_choice = cJSON_GetArrayItem(choices, 0);
                cJSON *message = cJSON_GetObjectItemCaseSensitive(first_choice, "message");
                cJSON *content = cJSON_GetObjectItemCaseSensitive(message, "content");

                if (cJSON_IsString(content) && (content->valuestring != NULL)) {
                    fwrite(content->valuestring, 1, strlen(content->valuestring), fp);
                    printf("Received: %s\n", content->valuestring);
                }
                cJSON_Delete(json);
            }
        }
        fclose(fp);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return 0;
}

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}
// 回應數據的處理函數
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}