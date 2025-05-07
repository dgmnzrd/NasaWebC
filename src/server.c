#include "mongoose.h"
#include "server.h"
#include "nasa_api.h"
#include "cache_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>  // mkdir
#include <sys/types.h>

#define PORT "8080"
#define CACHE_DIR "cache"

// Verifica si la carpeta de caché existe, si no, la crea
static void ensure_cache_dir() {
    struct stat st = {0};
    if (stat(CACHE_DIR, &st) == -1) {
        mkdir(CACHE_DIR, 0700);
    }
}

static void handle_request(struct mg_connection *c, int ev, void *ev_data) {
    if (ev != MG_EV_HTTP_MSG) return;

    ensure_cache_dir();  // Asegura que el directorio exista antes de cada solicitud

    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    const char *api_key = getenv("API_KEY");
    if (!api_key) {
        mg_http_reply(c, 500, "Content-Type: text/plain\r\n", "API_KEY not configured");
        return;
    }

    // === /apod endpoint ===
    if (mg_strcmp(hm->uri, mg_str("/apod")) == 0) {
        const char *cache_file = CACHE_DIR "/cache_apod.json";
        const char *ts_file = CACHE_DIR "/cache_apod.ts";
        char *response = NULL;

        if (has_day_changed(ts_file)) {
            response = fetch_apod_data(api_key);
            if (response && strchr(response, '{')) {
                save_to_cache(cache_file, response);
                update_timestamp(ts_file);
            } else {
                free(response);
                response = load_from_cache(cache_file);
            }
        } else {
            response = load_from_cache(cache_file);
        }

        if (response) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", response);
            free(response);
        } else {
            mg_http_reply(c, 500, "Content-Type: text/plain\r\n", "Failed to load APOD data");
        }

    // === /donki endpoint ===
    } else if (mg_strcmp(hm->uri, mg_str("/donki")) == 0) {
        char type[32], start[16], end[16];
        mg_http_get_var(&hm->query, "type", type, sizeof(type));
        mg_http_get_var(&hm->query, "startDate", start, sizeof(start));
        mg_http_get_var(&hm->query, "endDate", end, sizeof(end));

        if (!*type || !*start || !*end) {
            mg_http_reply(c, 400, "Content-Type: text/plain\r\n", "Missing required parameters");
            return;
        }

        char cache_file[256], ts_file[256];
        snprintf(cache_file, sizeof(cache_file), CACHE_DIR "/cache_donki_%s.json", type);
        snprintf(ts_file, sizeof(ts_file), CACHE_DIR "/cache_donki_%s.ts", type);

        char *response = NULL;

        if (has_day_changed(ts_file)) {
            response = fetch_donki_data(type, start, end, api_key);
            if (response && strchr(response, '[')) {
                save_to_cache(cache_file, response);
                update_timestamp(ts_file);
            } else {
                free(response);
                response = load_from_cache(cache_file);
            }
        } else {
            response = load_from_cache(cache_file);
        }

        if (response) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", response);
            free(response);
        } else {
            mg_http_reply(c, 500, "Content-Type: text/plain\r\n", "Failed to load DONKI data");
        }

    // === Static files ===
    } else {
        struct mg_http_serve_opts opts = {.root_dir = "public"};
        mg_http_serve_dir(c, hm, &opts);
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