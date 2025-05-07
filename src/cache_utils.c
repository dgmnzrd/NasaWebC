#include "cache_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int has_day_changed(const char *timestamp_file) {
    FILE *f = fopen(timestamp_file, "r");
    if (!f) return 1; // Si no hay timestamp, forzar actualización

    time_t last_time;
    fscanf(f, "%ld", &last_time);
    fclose(f);

    struct tm last_tm, now_tm;
    localtime_r(&last_time, &last_tm);
    time_t now = time(NULL);
    localtime_r(&now, &now_tm);

    // Comparamos año, mes y día
    if (now_tm.tm_year != last_tm.tm_year ||
        now_tm.tm_mon  != last_tm.tm_mon  ||
        now_tm.tm_mday != last_tm.tm_mday) {
        return 1; // Cambió el día
    }

    return 0; // Mismo día
}

void update_timestamp(const char *timestamp_file) {
    FILE *f = fopen(timestamp_file, "w");
    if (!f) return;
    fprintf(f, "%ld", time(NULL));
    fclose(f);
}

char *load_from_cache(const char *cache_file) {
    FILE *f = fopen(cache_file, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);

    char *buffer = malloc(len + 1);
    fread(buffer, 1, len, f);
    buffer[len] = '\0';

    fclose(f);
    return buffer;
}

int save_to_cache(const char *cache_file, const char *data) {
    FILE *f = fopen(cache_file, "w");
    if (!f) return 1;
    fputs(data, f);
    fclose(f);
    return 0;
}