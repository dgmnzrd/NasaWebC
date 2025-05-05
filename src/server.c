#include "mongoose.h"
#include "server.h"
#include "nasa_api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PORT "8080"

static void handle_request(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;

        if (mg_strcmp(hm->uri, mg_str("/apod")) == 0) {
            const char *api_key = getenv("API_KEY");
            if (!api_key) {
                mg_http_reply(c, 500, "Content-Type: text/plain\r\n", "API_KEY not configured");
                return;
            }

            char *apod_data = fetch_apod_data(api_key);
            if (apod_data && strchr(apod_data, '{')) {
                // debug opcional
                printf("APOD JSON:\n%s\n", apod_data);

                mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", apod_data);
                free(apod_data);
            } else {
                mg_http_reply(c, 500, "Content-Type: text/plain\r\n", "Error getting APOD data");
                if (apod_data) free(apod_data);
            }

        } else {
            struct mg_http_serve_opts opts = {.root_dir = "public"};
            mg_http_serve_dir(c, hm, &opts);
        }
    }
}

void start_server() {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    printf("Servidor iniciado en http://localhost:%s\n", PORT);
    mg_http_listen(&mgr, "http://localhost:" PORT, handle_request, NULL);
    for (;;) mg_mgr_poll(&mgr, 1000);
    mg_mgr_free(&mgr);
}