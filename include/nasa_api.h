#ifndef NASA_API_H
#define NASA_API_H

char *fetch_apod_data(const char *api_key);
char *fetch_donki_data(const char *type, const char *start_date, const char *end_date, const char *api_key);

#endif