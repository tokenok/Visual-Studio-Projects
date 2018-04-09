#include <Windows.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <regex>
#include "Shlwapi.h"

#include "Registry.h"
//#include "zlib.h"

#include "update.h"
#include "common.h"
#include "console.h"
#include "include.h"

#pragma warning(disable: 4129)

std::string VERSION_URL = "";
std::string DOWNLOAD_URL = "http://annihilus.net/downloads/Annihilus.zip";

using namespace std;
using namespace console_class;

bool update() {
	string cur_ver, new_ver;
	CURLcode result = CURLE_OK;
	console << "checking for updates...\n";
	if (check_for_updates(&new_ver, &cur_ver, &result) || result != CURLE_OK) {
		if (result != CURLE_OK) {
			console.setConsoleColor(t::red + b::black);
			console << "check failed\n";
			console.restorePreviousAttributes();
			MessageBoxA(NULL, curl_easy_strerror(result), "ERROR", MB_OK);
			return false;
		}
		else {
			//save new version to registry
			CRegistry hregkey;
			if (hregkey.Open(CREG_PATH, CREG_ROOTKEY))
				hregkey[CREG_KEY_VERSION] = str_to_wstr(new_ver).c_str();
			hregkey.Close();
			console.setConsoleColor(t::green + b::black);
			console << "update found!\n";
			console.restorePreviousAttributes();
			console << "downloading update...\n";
			result = download_update();
			if (result != CURLE_OK) {
				console.setConsoleColor(t::red + b::black);
				console << "download failed\n";
				console.restorePreviousAttributes();
				MessageBoxA(NULL, curl_easy_strerror(result), "ERROR", MB_OK);
				return false;
			}
			else {
				console.setConsoleColor(t::green + b::black);
				console << "download successful\n";
				console.restorePreviousAttributes();

				//extract files
				TCHAR wpath[MAX_PATH];
				GetModuleFileName(NULL, wpath, MAX_PATH);
				string path = wstr_to_str(wpath);
				path = path.substr(0, path.rfind('\\') + 1);

				string zipfile = DOWNLOAD_URL.substr(DOWNLOAD_URL.rfind('/') + 1, DOWNLOAD_URL.length());
				zipfile = path + zipfile;

				console << "extracting update..." << endl;

				extract_folder(zipfile, path);
			}
		}
	}
	else {
		console << "no update found\n";
		cout << curl_easy_strerror(result) << '\n';
		if (result != CURLE_OK) {
			console.setConsoleColor(t::red + b::black);
			cout << curl_easy_strerror(result) << '\n';
			console.restorePreviousAttributes();
		}
		return false;
	}
	return true;
}

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userp) {
	((std::string*)userp)->append((char*)ptr, size * nmemb);
	return size * nmemb;
}
size_t file_write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}
int progress_func(void* /*ptr*/, double total_to_download, double total_already_downloaded, double /*TotalToUpload*/, double /*NowUploaded*/) {
	static double prev_downloaded = 0;
	static int count = 0;
	static int first = 1;

	double current_download_speed;
	double avg_dl_speed = 0;
	double total_time;
	double time_left;
	double elapsed_time = 0;

	static time_t prevtime;
	static time_t starttime;
	time_t currtime;
	double dif;
	if (first) {
		time(&starttime);
		time(&prevtime);
		first = 0;
	}
	time(&currtime);
	dif = difftime(currtime, prevtime);
	/*if (dif < .5)
	return 0;*/
	prevtime = currtime;
	count++;

	current_download_speed = total_already_downloaded - prev_downloaded;
	elapsed_time = difftime(currtime, starttime);
	total_time = total_to_download / current_download_speed;
	time_left = total_time - elapsed_time;
	avg_dl_speed = total_already_downloaded / count;
	int percent_downloaded = (int)(total_already_downloaded / total_to_download * 100);

	console.setConsoleColor(t::cyan | b::black);
	cout << setw(3) << percent_downloaded << "% "
		<< setw(5) << (int)(total_already_downloaded / 1024)
		<< " /"
		<< setw(5) << (int)(total_to_download / 1024)
		<< " kb\t\t\t\t\r";
	console.restorePreviousAttributes();
	cout.flush();

	//last
	prev_downloaded = total_already_downloaded;

	return 0;
}

BOOL check_for_updates(string *update_version, string *current_version, CURLcode *result) {
	CURL* curl;

	curl = curl_easy_init();
	//curl_easy_setopt(curl, CURLOPT_URL, "http://tokenworld.webs.com/VERSION.txt");
	curl_easy_setopt(curl, CURLOPT_URL, VERSION_URL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &*update_version);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 3500);

	//get update version
	*result = curl_easy_perform(curl);

	curl_easy_cleanup(curl);

	cout << *update_version << '\n';

	if (!regex_match(*update_version, regex("^\s*(?:[0-9]+\.*)+"))) {
		console.setConsoleColor(t::red + b::black);
		console << "invalid version info: " << *update_version << '\n';
		console.restorePreviousAttributes();
		return false;
	}

	//check current version
	CRegistry hregkey;
	hregkey.Open(CREG_PATH, CREG_ROOTKEY);
	*current_version = wstr_to_str(hregkey[CREG_KEY_VERSION]);
	hregkey.Close();

	if (update_version->size() > 0 && *update_version != *current_version) {
		return true;
	}

	return false;
}

CURLcode download_update() {
	CURL* curl;
	CURLcode result;
	FILE *fp;

	string urlfile = DOWNLOAD_URL.substr(DOWNLOAD_URL.rfind('/') + 1, DOWNLOAD_URL.length());

	TCHAR wpath[MAX_PATH];
	GetModuleFileName(NULL, wpath, MAX_PATH);
	string path = wstr_to_str(wpath);
	path = path.substr(0, path.rfind('\\') + 1);
	path += urlfile;

	fp = fopen(path.c_str(), "wb");
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, DOWNLOAD_URL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
	result = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	fclose(fp);
	cout << '\n';

	return result;
}

std::string get_status_info() {
	CURL* curl;
	string php;

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, "http://annihilus.net/status/index.php");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &php);

	curl_easy_perform(curl);

	curl_easy_cleanup(curl);

	return php;
}

void change_file_date(const char *filename, uLong dosdate, tm_unz /*tmu_date*/) {
#ifdef WIN32
	HANDLE hFile;
	FILETIME ftm, ftLocal, ftCreate, ftLastAcc, ftLastWrite;

	hFile = CreateFile(str_to_wstr(filename).c_str(), GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);
	GetFileTime(hFile, &ftCreate, &ftLastAcc, &ftLastWrite);
	DosDateTimeToFileTime((WORD)(dosdate >> 16), (WORD)dosdate, &ftLocal);
	LocalFileTimeToFileTime(&ftLocal, &ftm);
	SetFileTime(hFile, &ftm, &ftLastAcc, &ftm);
	CloseHandle(hFile);
#else
#ifdef unix
	struct utimbuf ut;
	struct tm newdate;
	newdate.tm_sec = tmu_date.tm_sec;
	newdate.tm_min = tmu_date.tm_min;
	newdate.tm_hour = tmu_date.tm_hour;
	newdate.tm_mday = tmu_date.tm_mday;
	newdate.tm_mon = tmu_date.tm_mon;
	if (tmu_date.tm_year > 1900)
		newdate.tm_year = tmu_date.tm_year - 1900;
	else
		newdate.tm_year = tmu_date.tm_year;
	newdate.tm_isdst = -1;

	ut.actime = ut.modtime = mktime(&newdate);
	utime(filename, &ut);
#endif
#endif
}
int extract_currentfile(unzFile uf, string dest) {
	char filename_inzip[256];
	int err = UNZ_OK;
	FILE *fout = NULL;
	void* buf;
	uInt size_buf;

	unz_file_info file_info;
	err = unzGetCurrentFileInfo(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);

	if (err != UNZ_OK) {
		printf("error %d with zipfile in unzGetCurrentFileInfo\n", err);
		return err;
	}

	size_buf = 8192;
	buf = (void*)malloc(size_buf);
	if (buf == NULL) {
		printf("Error allocating memory\n");
		return UNZ_INTERNALERROR;
	}
	else {
		const char* write_filename;

		char fullpath[256];
		strcpy(fullpath, dest.c_str());
		strcat(fullpath, filename_inzip);

		write_filename = fullpath;


		err = unzOpenCurrentFile(uf);
		if (err != UNZ_OK) {
			//printf("error %d with zipfile in unzOpenCurrentFilePassword\n", err);
		}

		if (err == UNZ_OK) {
			fout = fopen(write_filename, "wb");

			if (fout == NULL) {
				//printf("error opening %s\n", write_filename);
			}
		}

		if (fout != NULL) {
		//	printf(" extracting: %s\n", write_filename);

			do {
				err = unzReadCurrentFile(uf, buf, size_buf);
				if (err<0) {
				//	printf("error %d with zipfile in unzReadCurrentFile\n", err);
					break;
				}
				if (err>0)
					if (fwrite(buf, err, 1, fout) != 1) {
						cout << "write\n" << '\n';
						//printf("error in writing extracted file\n");
						err = UNZ_ERRNO;
						break;
					}
			} while (err>0);
			if (fout)
				fclose(fout);

			if (err == 0)
				change_file_date(write_filename, file_info.dosDate,
				file_info.tmu_date);
		}

		if (err == UNZ_OK) {
			err = unzCloseCurrentFile(uf);
			if (err != UNZ_OK) {
				//printf("error %d with zipfile in unzCloseCurrentFile\n", err);
			}
		}
		else
			unzCloseCurrentFile(uf); /* don't lose the error */
	}

	free(buf);
	return err;
}
int extract_folder(string zipfile, string dest/* = ""*/) {
	unzFile uf = unzOpen(zipfile.c_str());
	if (uf == NULL) {
		cout << "file does not exist or it is invalid" << '\n';
		return 0;
	}

	if (dest.size() == 0)
		dest = zipfile.substr(0, zipfile.rfind('\\') + 1);
	if (dest[dest.length() - 1] != '\\')
		dest += '\\';
	if (!PathFileExists(str_to_wstr(dest).c_str()))
		if (!CreateDirectory(str_to_wstr(dest).c_str(), NULL))
			cout << error_code_to_text(GetLastError()) << '\n';

	uLong i;
	unz_global_info gi;
	int err;
	/*FILE* fout = NULL;*/

	err = unzGetGlobalInfo(uf, &gi);
	if (err != UNZ_OK)
		printf("error %d with zipfile in unzGetGlobalInfo \n", err);

	for (i = 0; i < gi.number_entry; i++) {
		console.setConsoleColor(t::cyan | b::black);
		cout << setw(3) << ((double)(i + 1) / (double)gi.number_entry) * 100 << "% "
			<< setw(3) << i + 1
			<< " /"
			<< setw(3) << gi.number_entry
			<< " files\t\t\t\t\r";
		console.restorePreviousAttributes();
		cout.flush();

		if (extract_currentfile(uf, dest) != UNZ_OK)
			break;

		if ((i + 1) < gi.number_entry) {
			err = unzGoToNextFile(uf);
			if (err != UNZ_OK) {
				printf("error %d with zipfile in unzGoToNextFile\n", err);
				break;
			}
		}
	}
	console << endl;

	return 1;
}

