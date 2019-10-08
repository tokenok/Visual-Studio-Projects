#ifndef update_header_guard
#define update_header_guard

#include <curl\curl.h>
#include <string>

#include "zip/unzip.h"
#include "zip/iowin32.h"

bool update();

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userp);
size_t file_write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream);
int progress_func(void* /*ptr*/, double total_to_download, double total_already_downloaded, double /*TotalToUpload*/, double /*NowUploaded*/);

BOOL check_for_updates(std::string *update_version, std::string *current_version, CURLcode *result);

CURLcode download_update();

std::string get_status_info();

int extract_folder(string zipfile, string dest = "");


#endif

