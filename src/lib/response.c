#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include "response.h"

#define MAX_RESPONSE_SIZE 8192

int build_response(int status_code,
                   const char *status_text,
                   const char *content_type,
                   const char *body,
                   size_t body_length,
                   char *output_buffer,
                   size_t output_buffer_size)
{
    if (!body || !status_text || !content_type || !output_buffer)
        return -1;

    int written = snprintf(output_buffer, output_buffer_size,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        status_code, status_text, content_type, body_length);

    if ((size_t)written >= output_buffer_size) return -1;
    if (written + body_length >= output_buffer_size) return -1;

    memcpy(output_buffer + written, body, body_length);
    return written + body_length;
}

void send_ok(int client_socket_fd, const char *message) {
    if (!message) message = "OK";

    char buffer[MAX_RESPONSE_SIZE];
    int len = build_response(200, "OK", "text/plain", message, strlen(message), buffer, sizeof(buffer));

    if (len > 0) {
        send(client_socket_fd, buffer, len, 0);
    }
}

void send_not_found(int client_socket_fd, const char *message) {
    if (!message) message = "404 Not Found";

    char buffer[MAX_RESPONSE_SIZE];
    int len = build_response(404, "Not Found", "text/plain", message, strlen(message), buffer, sizeof(buffer));

    if (len > 0) {
        send(client_socket_fd, buffer, len, 0);
    }
}

void send_internal_error(int client_socket_fd, const char *message) {
    if (!message) message = "500 Internal Server Error";

    char buffer[MAX_RESPONSE_SIZE];
    int len = build_response(500, "Internal Server Error", "text/plain", message, strlen(message), buffer, sizeof(buffer));

    if (len > 0) {
        send(client_socket_fd, buffer, len, 0);
    }
}

void send_bad_request(int client_socket_fd, const char *message) {
    if (!message) message = "400 Bad Request";

    char buffer[MAX_RESPONSE_SIZE];
    int len = build_response(400, "Bad Request", "text/plain", message, strlen(message), buffer, sizeof(buffer));

    if (len > 0) {
        send(client_socket_fd, buffer, len, 0);
    }
}

int send_file_response(int client_socket_fd, const char *file_path, const char *content_type)
{
    if (!file_path || !content_type) return -1;

    int fd = open(file_path, O_RDONLY);
    if (fd == -1) return -1;

    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        return -1;
    }

    size_t file_size = st.st_size;

    char header[1024];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        content_type, file_size);

    if (header_len < 0 || header_len >= (int)sizeof(header)) {
        close(fd);
        return -1;
    }

    send(client_socket_fd, header, header_len, 0);

    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        send(client_socket_fd, buffer, bytes_read, 0);
    }

    close(fd);
    return 0;
}
