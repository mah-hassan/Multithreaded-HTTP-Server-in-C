#ifndef RESPONSE_H
#define RESPONSE_H
#include <stdio.h>
// Builds an HTTP response (used only internally)
int build_response(int status_code,
                   const char *status_text,
                   const char *content_type,
                   const char *body,
                   size_t body_length,
                   char *output_buffer,
                   size_t output_buffer_size);

// Public functions
void send_ok(int client_socket_fd, const char *message);
void send_not_found(int client_socket_fd, const char *message);
void send_internal_error(int client_socket_fd, const char *message);
void send_bad_request(int client_socket_fd, const char *message);

// File streaming response
int send_file_response(int client_socket_fd, const char *file_path, const char *content_type);

#endif
