#ifndef CACHE_UTILS_H
#define CACHE_UTILS_H

int has_day_changed(const char *timestamp_file);
void update_timestamp(const char *timestamp_file);
char *load_from_cache(const char *cache_file);
int save_to_cache(const char *cache_file, const char *data);

#endif