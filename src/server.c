#define MG_ENABLE_CUSTOM_MG_STR
#include "mongoose.h"
#include "server.h"
#include "nasa_api.h"
#include "cache_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h> // mkdir
#include <sys/types.h>

#define PORT "8080"
#define CACHE_DIR "cache"

// Verifica si la carpeta de caché existe, si no, la crea
static void ensure_cache_dir()
{
    struct stat st = {0};
    if (stat(CACHE_DIR, &st) == -1)
    {
        mkdir(CACHE_DIR, 0700);
    }
}

static void handle_request(struct mg_connection *c, int ev, void *ev_data)
{
    if (ev != MG_EV_HTTP_MSG)
        return;

    ensure_cache_dir();

    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    const char *api_key = getenv("API_KEY");
    if (!api_key)
    {
        mg_http_reply(c, 500, "Content-Type: text/plain\r\n", "API_KEY not configured");
        return;
    }

    // === /apod endpoint ===
    if (mg_strcmp(hm->uri, mg_str("/apod")) == 0)
    {
        const char *cache_file = CACHE_DIR "/cache_apod.json";
        const char *ts_file = CACHE_DIR "/cache_apod.ts";
        char *response = NULL;

        if (has_day_changed(ts_file))
        {
            response = fetch_apod_data(api_key);
            if (response && strchr(response, '{'))
            {
                save_to_cache(cache_file, response);
                update_timestamp(ts_file);
            }
            else
            {
                free(response);
                response = load_from_cache(cache_file);
            }
        }
        else
        {
            response = load_from_cache(cache_file);
        }

        if (response)
        {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", response);
            free(response);
        }
        else
        {
            mg_http_reply(c, 500, "Content-Type: text/plain\r\n", "Failed to load APOD data");
        }
        return;
    }

    // === /donki endpoint ===
    if (mg_strcmp(hm->uri, mg_str("/donki")) == 0)
    {
        char type[32], start[16], end[16];
        mg_http_get_var(&hm->query, "type", type, sizeof(type));
        mg_http_get_var(&hm->query, "startDate", start, sizeof(start));
        mg_http_get_var(&hm->query, "endDate", end, sizeof(end));

        if (!*type || !*start || !*end)
        {
            mg_http_reply(c, 400, "Content-Type: text/plain\r\n", "Missing required parameters");
            return;
        }

        char cache_file[256], ts_file[256];
        snprintf(cache_file, sizeof(cache_file), CACHE_DIR "/cache_donki_%s.json", type);
        snprintf(ts_file, sizeof(ts_file), CACHE_DIR "/cache_donki_%s.ts", type);

        char *response = NULL;
        if (has_day_changed(ts_file))
        {
            response = fetch_donki_data(type, start, end, api_key);
            if (response && strchr(response, '['))
            {
                save_to_cache(cache_file, response);
                update_timestamp(ts_file);
            }
            else
            {
                free(response);
                response = load_from_cache(cache_file);
            }
        }
        else
        {
            response = load_from_cache(cache_file);
        }

        if (response)
        {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", response);
            free(response);
        }
        else
        {
            mg_http_reply(c, 500, "Content-Type: text/plain\r\n", "Failed to load DONKI data");
        }
        return;
    }

    // === /mars endpoint ===
    if (mg_strcmp(hm->uri, mg_str("/mars")) == 0)
    {
        char date[16];
        mg_http_get_var(&hm->query, "earth_date", date, sizeof(date));
        if (!*date)
        {
            mg_http_reply(c, 400, "Content-Type: text/plain\r\n", "Missing earth_date parameter");
            return;
        }

        char cache_file[256], ts_file[256];
        snprintf(cache_file, sizeof(cache_file), CACHE_DIR "/cache_mars_%s.json", date);
        snprintf(ts_file, sizeof(ts_file), CACHE_DIR "/cache_mars_%s.ts", date);

        char *response = NULL;
        if (has_day_changed(ts_file))
        {
            response = fetch_mars_photos(date, api_key);
            if (response && strchr(response, '{'))
            {
                save_to_cache(cache_file, response);
                update_timestamp(ts_file);
            }
            else
            {
                free(response);
                response = load_from_cache(cache_file);
            }
        }
        else
        {
            response = load_from_cache(cache_file);
        }

        if (response)
        {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", response);
            free(response);
        }
        else
        {
            mg_http_reply(c, 500, "Content-Type: text/plain\r\n", "Failed to load Mars Rover data");
        }
        return;
    }

    if (mg_strcmp(hm->uri, mg_str("/video")) == 0)
    {
        char id[256];
        mg_http_get_var(&hm->query, "id", id, sizeof(id));
        if (strlen(id) == 0)
        {
            mg_http_reply(c, 400, "Content-Type: text/plain\r\n", "Missing video ID");
            return;
        }

        // Sanitizar para nombre de archivo
        char id_safe[256];
        snprintf(id_safe, sizeof(id_safe), "%s", id);
        for (int i = 0; id_safe[i]; i++)
        {
            if (id_safe[i] == ' ')
                id_safe[i] = '_';
        }

        char cache_file[256], ts_file[256];
        snprintf(cache_file, sizeof(cache_file), CACHE_DIR "/cache_video_%s.json", id_safe);
        snprintf(ts_file, sizeof(ts_file), CACHE_DIR "/cache_video_%s.ts", id_safe);

        char *response = NULL;
        if (has_day_changed(ts_file))
        {
            response = fetch_video_info(id); // usar id original
            if (response && strchr(response, '{'))
            {
                save_to_cache(cache_file, response);
                update_timestamp(ts_file);
            }
            else
            {
                free(response);
                response = load_from_cache(cache_file);
            }
        }
        else
        {
            response = load_from_cache(cache_file);
        }

        if (response)
        {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", response);
            free(response);
        }
        else
        {
            mg_http_reply(c, 500, "Content-Type: text/plain\r\n", "Failed to load video info");
        }
        return;
    }

    if (mg_strcmp(hm->uri, mg_str("/search")) == 0)
    {
        char query[128] = "nasa";
        char media[32] = "video"; // Valor por defecto
        char tmp_media[32] = "";
        char page_str[8] = "1";
        int page = 1;

        mg_http_get_var(&hm->query, "q", query, sizeof(query));
        mg_http_get_var(&hm->query, "media_type", tmp_media, sizeof(tmp_media));
        mg_http_get_var(&hm->query, "page", page_str, sizeof(page_str));

        if (strlen(tmp_media) > 0)
        {
            snprintf(media, sizeof(media), "%s", tmp_media);
        }

        page = atoi(page_str);
        if (page < 1)
            page = 1;

        char query_safe[128];
        snprintf(query_safe, sizeof(query_safe), "%s", query);
        for (int i = 0; query_safe[i]; i++)
        {
            if (query_safe[i] == ' ')
                query_safe[i] = '_';
        }

        char cache_file[256], ts_file[256];
        snprintf(cache_file, sizeof(cache_file), CACHE_DIR "/cache_search_%s_%s_p%d.json", query_safe, media, page);
        snprintf(ts_file, sizeof(ts_file), CACHE_DIR "/cache_search_%s_%s_p%d.ts", query_safe, media, page);

        char *response = NULL;
        if (has_day_changed(ts_file))
        {
            response = search_nasa_api(query, media, page);
            if (response && strchr(response, '{'))
            {
                save_to_cache(cache_file, response);
                update_timestamp(ts_file);
            }
            else
            {
                free(response);
                response = load_from_cache(cache_file);
            }
        }
        else
        {
            response = load_from_cache(cache_file);
        }

        if (response)
        {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", response);
            free(response);
        }
        else
        {
            mg_http_reply(c, 500, "Content-Type: text/plain\r\n", "Failed to load search results");
        }
        return;
    }

    // === Static files (html, css, js) ===
    struct mg_http_serve_opts opts = {.root_dir = "public"};
    mg_http_serve_dir(c, hm, &opts);
}

void start_server()
{
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    printf("Servidor iniciado en http://localhost:%s\n", PORT);
    mg_http_listen(&mgr, "http://localhost:" PORT, handle_request, NULL);
    for (;;)
        mg_mgr_poll(&mgr, 1000);
    mg_mgr_free(&mgr);
}