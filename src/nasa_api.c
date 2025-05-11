#include "nasa_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct MemoryStruct
{
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL)
        return 0;

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

static char *perform_request(const char *url)
{
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct chunk = {malloc(1), 0};

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    if (curl_handle)
    {
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        res = curl_easy_perform(curl_handle);

        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            free(chunk.memory);
            chunk.memory = NULL;
        }

        curl_easy_cleanup(curl_handle);
    }

    curl_global_cleanup();
    return chunk.memory;
}

char *fetch_apod_data(const char *api_key)
{
    char url[256];
    snprintf(url, sizeof(url), "https://api.nasa.gov/planetary/apod?api_key=%s", api_key);
    return perform_request(url);
}

char *fetch_donki_data(const char *type, const char *start_date, const char *end_date, const char *api_key)
{
    char url[512];
    snprintf(url, sizeof(url), "https://api.nasa.gov/DONKI/%s?startDate=%s&endDate=%s&api_key=%s", type, start_date, end_date, api_key);
    return perform_request(url);
}

char *fetch_mars_photos(const char *earth_date, const char *api_key)
{
    char url[512];
    snprintf(
        url, sizeof(url),
        "https://api.nasa.gov/mars-photos/api/v1/rovers/curiosity/photos?earth_date=%s&api_key=%s",
        earth_date, api_key);
    return perform_request(url);
}

char *fetch_video_info(const char *nasa_id)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return NULL;

    char *escaped_id = curl_easy_escape(curl, nasa_id, 0);
    if (!escaped_id)
    {
        curl_easy_cleanup(curl);
        return NULL;
    }

    char url_asset[512], url_meta[512];
    snprintf(url_asset, sizeof(url_asset), "https://images-api.nasa.gov/asset/%s", escaped_id);
    snprintf(url_meta, sizeof(url_meta), "https://images-api.nasa.gov/metadata/%s", escaped_id);

    curl_free(escaped_id);
    curl_easy_cleanup(curl);

    char *json_asset = perform_request(url_asset);
    char *json_meta_location = perform_request(url_meta);

    if (!json_asset || !json_meta_location)
    {
        free(json_asset);
        free(json_meta_location);
        return NULL;
    }

    // Extraer location
    const char *loc = strstr(json_meta_location, "\"location\"");
    if (!loc)
    {
        free(json_asset);
        free(json_meta_location);
        return NULL;
    }

    const char *start = strchr(loc, ':');
    if (!start)
    {
        free(json_asset);
        free(json_meta_location);
        return NULL;
    }

    start = strchr(start, '\"');
    const char *end = start ? strchr(start + 1, '\"') : NULL;
    if (!start || !end)
    {
        free(json_asset);
        free(json_meta_location);
        return NULL;
    }

    char location_url[512] = {0};
    strncpy(location_url, start + 1, end - start - 1);
    free(json_meta_location);

    char *json_metadata = perform_request(location_url);
    if (!json_metadata)
    {
        free(json_asset);
        return NULL;
    }

    // Buscar primer .mp4
    const char *mp4 = strstr(json_asset, ".mp4");
    const char *href_start = mp4;
    while (href_start && href_start > json_asset && *href_start != '\"')
        href_start--;
    if (!mp4 || !href_start || *href_start != '\"')
    {
        free(json_asset);
        free(json_metadata);
        return NULL;
    }

    char video_url[512] = {0};
    strncpy(video_url, href_start + 1, mp4 - href_start + 3);

    // Título y descripción
    char title[256] = "NASA Video";
    char desc[1024] = "No description";

    const char *t1 = strstr(json_metadata, "\"title\"");
    if (t1)
    {
        const char *t2 = strchr(t1 + 7, '\"');
        const char *t3 = t2 ? strchr(t2 + 1, '\"') : NULL;
        if (t2 && t3 && t3 - t2 - 1 < sizeof(title))
        {
            strncpy(title, t2 + 1, t3 - t2 - 1);
            title[t3 - t2 - 1] = '\0';
        }
    }

    const char *d1 = strstr(json_metadata, "\"description\"");
    if (d1)
    {
        const char *d2 = strchr(d1 + 12, '\"');
        const char *d3 = d2 ? strchr(d2 + 1, '\"') : NULL;
        if (d2 && d3 && d3 - d2 - 1 < sizeof(desc))
        {
            strncpy(desc, d2 + 1, d3 - d2 - 1);
            desc[d3 - d2 - 1] = '\0';
        }
    }

    free(json_asset);
    free(json_metadata);

    for (char *p = title; *p; ++p)
        if (*p == '\"')
            *p = '\'';
    for (char *p = desc; *p; ++p)
        if (*p == '\"')
            *p = '\'';

    char *json = malloc(2048);
    snprintf(json, 2048,
             "{ \"video_url\": \"%s\", \"title\": \"%s\", \"description\": \"%s\" }",
             video_url, title, desc);
    return json;
}

char *search_nasa_api(const char *query, const char *media_type, int page)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return NULL;

    char *q_esc = curl_easy_escape(curl, query, 0);
    char *t_esc = curl_easy_escape(curl, media_type ? media_type : "", 0);

    if (!q_esc || !t_esc)
    {
        if (q_esc)
            curl_free(q_esc);
        if (t_esc)
            curl_free(t_esc);
        curl_easy_cleanup(curl);
        return NULL;
    }

    char url[512];
    snprintf(url, sizeof(url), "https://images-api.nasa.gov/search?q=%s&media_type=%s&page=%d", q_esc, t_esc, page);

    curl_free(q_esc);
    curl_free(t_esc);
    curl_easy_cleanup(curl);

    char *result = perform_request(url);

    if (!result || !strstr(result, "\"items\""))
    {
        free(result);
        return NULL;
    }

    return result;
}

char *search_nasa_media(const char *query, const char *media_type, int page)
{
    char url[512];
    snprintf(
        url, sizeof(url),
        "https://images-api.nasa.gov/search?q=%s&media_type=%s&page=%d",
        query, media_type, page);
    return perform_request(url);
}