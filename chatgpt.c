#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

// 用於存儲來自API響應的數據
struct string {
  char *ptr;
  size_t len;
};

// 初始化string結構
void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

// curl寫數據的回調函數
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
  size_t new_len = s->len + size * nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size * nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size * nmemb;
}

// 讀取API密鑰和輸入文本，並執行API請求
int main(void) {
    CURL *curl;
    CURLcode res;
    char *api_key;
    FILE *file;
    char *input_text;
    long input_size;
    struct string s;
    init_string(&s);
    
    // 讀取API密鑰
    printf("Please enter your API key: ");
    scanf("%ms", &api_key);  // %ms 會自動分配記憶體
    
    // 讀取input.txt
    file = fopen("input.txt", "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        input_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        input_text = malloc(input_size + 1);
        fread(input_text, 1, input_size, file);
        fclose(file);
        input_text[input_size] = 0;
    }

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
        char post_data[2048];
        sprintf(post_data, "{\"model\":\"gpt-3.5-turbo\",\"messages\":[{\"role\":\"user\",\"content\":\"%s\"}],\"stream\":true}", input_text);
        
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.chatanywhere.tech/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
        
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        char auth_header[256];
        sprintf(auth_header, "Authorization: Bearer %s", api_key);
        headers = curl_slist_append(headers, auth_header);
        
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        
        res = curl_easy_perform(curl);
        
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        curl_global_cleanup();

        // 處理JSON回應並寫入檔案
        cJSON *root = cJSON_Parse(s.ptr);
        cJSON *choices = cJSON_GetObjectItem(root, "choices");
        if (choices) {
            cJSON *choice = cJSON_GetArrayItem(choices, 0);
            if (choice) {
                cJSON *message = cJSON_GetObjectItem(choice, "message");
                if (message) {
                    cJSON *content = cJSON_GetObjectItem(message, "content");
                    if (content) {
                        file = fopen("message.txt", "w");
                        if (file) {
                            fprintf(file, "%s", content->valuestring);
                            fclose(file);
                        }
                    }
                }
            }
        }
        
        // 釋放記憶體
        cJSON_Delete(root);
        free(s.ptr);
    }
    
    free(api_key);
    free(input_text);

    return 0;
}
