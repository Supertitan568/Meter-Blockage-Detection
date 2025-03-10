#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <event2/buffer.h>
#include <event2/event.h>
#include <event2/http.h>

static struct table_entry {
  const char* file_extention;
  const char* mime_type;
} mime_type_table[] = {
  {"txt", "text/plain"},
  {"js", "text/javascript"},
  {"css", "text/css"},
  {"html", "text/html"},
  {NULL, NULL}
};


static const char* get_mime_type(const char* uri){
  // Get last thing in path then compare against table above
  const char* file_ext = strrchr(uri, '.');
  if(file_ext || !strchr(file_ext, '/')){
    file_ext++;
    for(struct table_entry* entry = mime_type_table; entry->mime_type; entry++){
      if(strcmp(entry->file_extention, file_ext) == 0){
        return entry->mime_type;
      }
    }
  }
  
  // Assuming that we already checked for our endpoints before this
  return "application/stream";
}

static const char* get_full_file_path(const char* static_dir, const char* path){
  // Prob not too important but maybe replace strcpy with strncpy later
  char* full_path = (char*) malloc(strlen(static_dir) + strlen(path) + 1);
  strcpy(full_path, static_dir);
  strcpy(full_path + strlen(static_dir), path);

  return full_path;
}

static void add_content_length(struct evhttp_request* req, unsigned req_size){
  char buffer[64];
  snprintf(buffer, sizeof(buffer), "%u", req_size);
  evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Length", buffer);
}


static const char* get_real_file(struct evbuffer* reply, const char* path){
  FILE* f;
  char buffer[256];
  if((f = fopen(path, "r"))){
    while(fgets(buffer, sizeof(buffer), f)){
      evbuffer_add_printf(reply, buffer);  
    }
    fclose(f);
  }
  return NULL;
}


static void handle_http_request(struct evhttp_request *req, void *ctx){
  // Setting up local vars
  const char* static_dir = ".";

  // Checking http method
  // Maybe I'll handle POSTS too depeding on what we decide lol
  enum evhttp_cmd_type cmd = evhttp_request_get_command(req);
  if (cmd != EVHTTP_REQ_GET){
    return;
  }
  
  // Decoding the URI
  struct evhttp_uri* decoded = evhttp_uri_parse(evhttp_request_get_uri(req));
  if(!decoded){
    evhttp_send_error(req, HTTP_BADREQUEST, 0);
    return;
  }

  /* Let's see what path the user asked for. */
  const char *path = evhttp_uri_get_path(decoded);
  if (!path){
    path = "/";
  }

  // Getting the path
  // Gotta check to see if the client is trying to get endpoint or document
  char* decoded_path = evhttp_uridecode(path, 0, NULL);
  if(decoded_path == NULL){
    evhttp_send_error(req, HTTP_NOTFOUND, NULL);
  }

  // If this is out on the open internet make sure to sanitize the URI
  // Oh well I'll do it later
          
  printf("Requested %s\n", decoded_path);  
  struct evbuffer* reply = evbuffer_new();
  if(strcmp(decoded_path, "/sensor") == 0){
    // We really just need to send a bool here
    evbuffer_add_printf(reply, "1"); 
    evhttp_send_reply(req, HTTP_OK, NULL, reply);
  }
  else{
    const char* full_file_path = get_full_file_path(static_dir, decoded_path); 
     
    if(access(full_file_path, F_OK) != -1){
      get_real_file(reply, full_file_path);
      evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", get_mime_type(decoded_path));
      add_content_length(req, evbuffer_get_length(reply));
      evhttp_send_reply(req, HTTP_OK, NULL, reply);
    }
    else {
      evhttp_send_error(req, HTTP_NOTFOUND, NULL);
    }
    free((void*)full_file_path);
  }

  evbuffer_free(reply);

  if(decoded)
    evhttp_uri_free(decoded);
  if (decoded_path) 
    free(decoded_path);

}

static void signal_cb(evutil_socket_t fd, short event, void *arg){
  printf("%s signal received\n", strsignal(fd));
  event_base_loopbreak(arg);
}

int main() {
  ev_uint16_t http_port = 8080;
  char* http_addr = "0.0.0.0";
  struct event_base *base;
  struct evhttp *http_server;
  struct event *sig_int;

  base = event_base_new();

  http_server = evhttp_new(base);
  evhttp_bind_socket(http_server, http_addr, http_port);
  evhttp_set_gencb(http_server, handle_http_request, NULL);

  sig_int = evsignal_new(base, SIGINT, signal_cb, base);
  event_add(sig_int, NULL);

  printf("Listening requests on http://%s:%d\n", http_addr, http_port);

  event_base_dispatch(base);

  evhttp_free(http_server);
  event_free(sig_int);
  event_base_free(base);
}
