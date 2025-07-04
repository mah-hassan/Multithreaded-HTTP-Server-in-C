#include <string.h>
#include <stdio.h>
#include "request.h"

int parse_http_req(const char *raw_request, struct http_request *request)
{
    if (!raw_request || !request) {
        printf("❌ Invalid input: NULL pointer\n");
        return -1;
    }

    printf("📩 RAW REQUEST BEGIN >>>>>>>>>>>\n%s\n<<<<<<<<<< END RAW REQUEST\n", raw_request);

    // STEP 1: Parse the request line
    const char *line_end = strstr(raw_request, "\r\n");
    size_t line_offset = 2;
    if (!line_end) {
        printf("⚠️ \\r\\n not found, trying fallback to \\n...\n");
        line_end = strchr(raw_request, '\n');
        if (!line_end) {
            printf("❌ Could not find end of request line\n");
            return -1;
        }
        line_offset = 1;
    }

    size_t line_len = line_end - raw_request;
    if (line_len >= sizeof(request->method) + sizeof(request->path) + sizeof(request->version)) {
        printf("❌ First line too long\n");
        return -1;
    }

    char first_line[600];
    strncpy(first_line, raw_request, line_len);
    first_line[line_len] = '\0';

    printf("✅ Parsed request line: \"%s\"\n", first_line);

    int parsed = sscanf(first_line, "%7s %511s %15s", request->method, request->path, request->version);
    if (parsed != 3) {
        printf("❌ Failed to parse request line. sscanf matched %d fields\n", parsed);
        return -1;
    }

    printf("📌 Method: %s\n📌 Path: %s\n📌 Version: %s\n", request->method, request->path, request->version);

    // STEP 2: Parse headers
    const char *headers_start = line_end + line_offset;
    const char *headers_end = strstr(headers_start, "\r\n\r\n");
    size_t headers_offset = 4;

    if (!headers_end) {
        printf("⚠️ \\r\\n\\r\\n not found, trying fallback to \\n\\n...\n");
        headers_end = strstr(headers_start, "\n\n");
        if (!headers_end) {
            printf("❌ Could not find end of headers\n");
            return -1;
        }
        headers_offset = 2;
    }

    size_t headers_len = headers_end - headers_start;
    if (headers_len >= sizeof(request->headers)) {
        printf("❌ Headers too long (limit: %zu bytes)\n", sizeof(request->headers));
        return -1;
    }

    strncpy(request->headers, headers_start, headers_len);
    request->headers[headers_len] = '\0';

    printf("✅ Parsed headers:\n%s\n", request->headers);

    // STEP 3: Parse body
    const char *body_start = headers_end + headers_offset;
    strncpy(request->body, body_start, sizeof(request->body) - 1);
    request->body[sizeof(request->body) - 1] = '\0';

    if (strlen(request->body) > 0) {
        printf("✅ Parsed body:\n%s\n", request->body);
    } else {
        printf("✅ No body found\n");
    }

    return 0;
}
