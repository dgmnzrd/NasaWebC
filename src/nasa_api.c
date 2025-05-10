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

char *fetch_mars_photos(const char *earth_date, const char *api_key) {
    char url[512];
    snprintf(
        url, sizeof(url),
        "https://api.nasa.gov/mars-photos/api/v1/rovers/curiosity/photos?earth_date=%s&api_key=%s",
        earth_date, api_key
    );
    return perform_request(url);
}