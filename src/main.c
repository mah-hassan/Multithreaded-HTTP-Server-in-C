#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> // For sockaddr_in
#include <string.h>
#include <unistd.h> 
#include <pthread.h>
#include <stdlib.h>

// include defined http struct
#include "lib/request.h"
#include "lib/response.h"
#include "lib/queue.h"

#define PORT 3000
#define THREAD_POOL_SIZE 150

pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t conditional_var = PTHREAD_COND_INITIALIZER;
queue *connections_queue;
// function prototypes
void *thread_worker();
void process(const struct http_request *req, int client_socket_fd);
void *handle_connection(void *arg);
// define functions for supported routes
void home(int client_socket_fd);
void hello(int client_socket_fd, const char *name);
int accepted_connections = 0;
int main() 
{
     // Create a socket
     int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
     if(socket_fd == -1)
     {
        perror("socket creation failed");
        return 1;
     }
     // bind the socket to server address
     struct sockaddr_in server_addr;
     server_addr.sin_family = AF_INET; // IPv4
     server_addr.sin_port = htons(PORT); // Convert port number to network byte order
     server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address

     int bind_status = bind(socket_fd,
          (struct sockaddr*)&server_addr,
          sizeof(server_addr));

     if(bind_status < 0)
     {
        perror("failed binding socket to address");
        return 1;
     }
     // Listen for incoming connections
     int listen_status = listen(socket_fd, 200);
     if(listen_status == -1)
     {
        perror("failed to listen for incoming connections");
        return 1;
     }
     printf("Server is listening on port %d\n", PORT);
     
     connections_queue = create_queue();

     // Intialize the thread pool
     for(int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&thread_pool[i], NULL, thread_worker, NULL);
        pthread_detach(thread_pool[i]); // Detach the thread to allow it to run independently (non-blocking)
     }

     // Accept incoming connections
     while(1) {
        int client_socket_fd = accept(socket_fd, NULL, NULL);
        accepted_connections++;
        if (client_socket_fd == -1) {
            perror("accept failed");
            continue;
        }
        // Create a new thread to handle the client connection
        int *client_socket_fd_ptr = malloc(sizeof(int));
        *client_socket_fd_ptr = client_socket_fd;
        
        pthread_mutex_lock(&mutex);
        enqueue(connections_queue, client_socket_fd_ptr);
        pthread_cond_signal(&conditional_var); 
        pthread_mutex_unlock(&mutex);

     }
    
}

void *thread_worker() {
    while (1) {
        pthread_mutex_lock(&mutex);
        
        int *client_socket_fd_ptr = dequeue(connections_queue);
        if(client_socket_fd_ptr == NULL) {
            // If the queue is empty, wait for a signal
            pthread_cond_wait(&conditional_var, &mutex);
            client_socket_fd_ptr = dequeue(connections_queue);
        }
        pthread_mutex_unlock(&mutex);
        if(client_socket_fd_ptr != NULL) {
            handle_connection(client_socket_fd_ptr);
        }
    }
}


void *handle_connection(void *arg) {
    int client_socket_fd = *(int *)arg;
    free(arg);  // Free memory now that we’ve copied the value

    char buffer[1024];
    size_t received_bytes = recv(client_socket_fd, buffer, sizeof(buffer) - 1, 0);
    buffer[received_bytes] = '\0';

    struct http_request req;
    if (parse_http_req(buffer, &req) == 0) {
        printf("request parsed successfully:\n");
    } else {
        printf("plain request %s\n", buffer);
        send_bad_request(client_socket_fd, "Failed to parse HTTP request");
        printf("Failed to parse request\n");
        close(client_socket_fd);
        return NULL;
    }

    process(&req, client_socket_fd);
    close(client_socket_fd);
    return NULL;
}



void process(const struct http_request *req, int client_socket_fd)
{
    if (strcmp(req->method, "GET") != 0 && strcmp(req->method, "POST") != 0) {
        send_bad_request(client_socket_fd, "Unsupported HTTP method");
        return;
    }

    if (strcmp(req->path, "/") == 0) {
        home(client_socket_fd);
    } else if (strcmp(req->path, "/hello") == 0) {
        const char *name = req->body;
        hello(client_socket_fd, name);
    } else {
        send_not_found(client_socket_fd, "Route not found");
    }
}



void home(int client_socket_fd) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("❌ getcwd failed");
        send_internal_error(client_socket_fd, "Internal server error");
        return;
    }
    // Construct the full file path: "<cwd>/public/index.html"
    char file_path[2048];
    snprintf(file_path, sizeof(file_path), "%s/public/index.html", cwd);

    const char *content_type = "text/html";
    if (send_file_response(client_socket_fd, file_path, content_type) == -1) {
        send_internal_error(client_socket_fd, "Failed to send index.html");
    }
}

void hello(int client_socket_fd, const char *name) {
     if (!name || strlen(name) == 0) {
          send_bad_request(client_socket_fd, "Name parameter is required");
          return;
     }
     char response[512];
     snprintf(response, sizeof(response), "Hello, %s!", name);
     send_ok(client_socket_fd, response);
}