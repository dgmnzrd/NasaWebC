#ifndef NASA_API_H
#define NASA_API_H

char *fetch_apod_data(const char *api_key);
char *fetch_donki_data(const char *type, const char *start_date, const char *end_date, const char *api_key);
char *fetch_mars_photos(const char *earth_date, const char *api_key);
char *fetch_video_info(const char *nasa_id);
char *search_nasa_api(const char *query, const char *media_type, int page);
char *search_nasa_media(const char *query, const char *media_type, int page);

#endif