#ifndef REQUEST_H
#define REQUEST_H
// constants for HTTP request parsing
#define MAX_METHOD_LEN 8
#define MAX_PATH_LEN 512
#define MAX_VERSION_LEN 16
#define MAX_HEADERS_LEN 4096
#define MAX_BODY_LEN 4096

struct http_request {
    char method[MAX_METHOD_LEN];
    char path[MAX_PATH_LEN];
    char version[MAX_VERSION_LEN];
    char headers[MAX_HEADERS_LEN];
    char body[MAX_BODY_LEN];
};


// parsing function prototype
int parse_http_req(const char *raw_request, struct http_request *request);
#endif 