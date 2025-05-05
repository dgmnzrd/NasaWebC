#include "mongoose.h"
#include "server.h"
#include "nasa_api.h"
#include <stdio.h>
#include <string.h>

#define PORT "8080"

static void handle_request(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;

        // Comparación segura de URI usando mg_strcmp
        if (mg_strcmp(hm->uri, mg_str("/apod")) == 0) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"message\": \"Aquí irá el APOD\"}");
        } else {
            struct mg_http_serve_opts opts = {.root_dir = "public/html"};
            mg_http_serve_dir(c, hm, &opts);
        }
    }
}

void start_server() {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);

    printf("Servidor iniciado en http://localhost:%s\n", PORT);
    mg_http_listen(&mgr, "http://localhost:" PORT, handle_request, NULL);

    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }

    mg_mgr_free(&mgr);
}