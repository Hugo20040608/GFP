#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "escape_json_string.h"
#include "APIKEY.h"
int next_scene = 0;
struct string {
    char *ptr;
    size_t len;
};
void init_string(struct string *s);
size_t write_data(void *ptr, size_t size, size_t nmemb, struct string *s);
int main(void) {
    CURL *curl;
    CURLcode res;
    struct string s;
    init_string(&s);
    // 打開文件
    FILE *fp = fopen("story.toml", "rb");
    if (fp == NULL) {
        perror("Cannot open file.");
        return 1;
    }
    // get file line
    int fLine = 0;
    char line[1024] = {0};
    while(fgets(line, sizeof(line), fp) != NULL) {
        fLine++;
    }
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    // 讀取文件內容
    char *data = calloc(fsize + 1, 1);
    fread(data, 1, fsize, fp);
    data[fsize] = 0; // 確保字符串結尾
    fclose(fp);

    char *request_str = "請參考story.toml的內容，改寫";
    char *newdata = calloc(strlen(request_str) + strlen(data) + 1, 1);

    if(fLine <= 4){
        snprintf(newdata, sizeof(newdata), "%s%s%s", request_str, "角色一的說話方式", data);
    }
    else{
        snprintf(newdata, sizeof(newdata), "%s%s%s", request_str, "角色二的說話方式", data);
        next_scene = 1;
    }
    // fp = fopen("input.txt", "w");
    // fwrite(newdata, 1, strlen(newdata), fp);
    // fclose(fp);
    // 轉義 data 中的特殊 JSON 字符
    char *escaped_data = escape_json_string(newdata);
    free(data);  // 釋放原始 data，使用轉義後的字符串

    // 使用轉義後的字符串格式化 JSON
    char *post_data = calloc(strlen(escaped_data) + 1024, 1);
    int required = snprintf(NULL, 0, "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"user\", \"content\": \"%s\"}]}", escaped_data);
    if (required >= sizeof(post_data)) {
        fprintf(stderr, "Buffer size is too small for the content.\n");
        free(escaped_data);
        return 1;
    }
    snprintf(post_data, sizeof(post_data), "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"user\", \"content\": \"%s\"}]}", escaped_data);
    // printf("%s\n", post_data);
    free(escaped_data); // 釋放轉義後的 data
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
        if(res == CURLE_OK) {
            fwrite(s.ptr, 1, s.len, fp);
        }
        else {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
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
size_t write_data(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size * nmemb;
    char *new_ptr = realloc(s->ptr, new_len + 1);
    if (new_ptr == NULL) {
        return 0; // 通知 libcurl 處理失敗
    }
    s->ptr = new_ptr;
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    return size * nmemb;
}