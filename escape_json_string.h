char* escape_json_string(const char* input) {
    const char* p = input;
    int extra_chars = 0;

    // 首先計算需要額外添加多少字符來存放轉義字符
    while (*p) {
        switch (*p) {
            case '\n': case '\r': case '\t': case '\b': case '\f': case '"': case '\\':
                extra_chars++;
                break;
        }
        p++;
    }

    // 分配新的記憶體來存放處理後的字符串
    char* output = malloc(strlen(input) + extra_chars + 1);
    char* d = output;
    p = input;

    while (*p) {
        switch (*p) {
            case '\n': *d++ = '\\'; *d++ = 'n'; break;
            case '\r': *d++ = '\\'; *d++ = 'r'; break;
            case '\t': *d++ = '\\'; *d++ = 't'; break;
            case '\b': *d++ = '\\'; *d++ = 'b'; break;
            case '\f': *d++ = '\\'; *d++ = 'f'; break;
            case '"':  *d++ = '\\'; *d++ = '"';  break;
            case '\\': *d++ = '\\'; *d++ = '\\'; break;
            default:   *d++ = *p; break;
        }
        p++;
    }
    *d = '\0';
    return output;
}