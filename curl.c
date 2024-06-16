#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "escape_json_string.h"
#include "APIKEY.h"
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
    FILE *fp = fopen("test.toml", "rb");
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

    char prompt[1500] = "這個故事的大綱是這樣的：勇者接受了一個極限任務，要在一個神秘森林中尋找能改變命運的魔法石。在他的旅程中，勇者遇到了一位知道魔法石秘密的神秘老人，並被告知需要前往一座古老城堡尋找線索。勇者在城堡中發現了一本古書，書中記載著找到魔法石的關鍵是兩把鑰匙。勇者必須前往北村和南村尋找這兩把鑰匙，並在村莊的長老那裡得到了它們。最終，勇者回到城堡，使用這兩把鑰匙打開了藏有魔法石的寶箱，完成了任務。然而，這個故事中也充滿了危險和選擇。如果勇者做出錯誤的決定，比如跟隨錯誤的線索或選擇錯誤的路徑，他可能會遇到致命的危險，如被熊追殺或被侍衛殺死，導致任務失敗。整個故事是一系列選擇和後果的連鎖，勇者的每一個決定都可能導致不同的結局。這是一個關於勇氣、智慧和命運的冒險故事。生出範例中的RANDOM_DESCRIPTION，不要加```，務必遵守以下範例格式輸出：";
    size_t newdata_size = strlen(prompt) + strlen(data) + 50; // 50 為額外字符和安全邊際
    char *newdata = calloc(newdata_size, 1);
    if (newdata == NULL) {
        fprintf(stderr, "Failed to allocate memory for newdata.\n");
        return 1;
    }
    snprintf(newdata, newdata_size, "%s%s", prompt, data);
    // 轉義 data 中的特殊 JSON 字符
    char *escaped_data = escape_json_string(newdata);
    free(data);  // 釋放原始 data，使用轉義後的字符串

    // 使用轉義後的字符串格式化 JSON
    size_t post_data_size = strlen(escaped_data) + 1024;
    char *post_data = calloc(post_data_size, 1);
    snprintf(post_data, post_data_size, "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"user\", \"content\": \"%s\"}]}", escaped_data);
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