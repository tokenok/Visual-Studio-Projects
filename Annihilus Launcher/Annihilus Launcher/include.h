#ifndef include_header_guard
#define include_header_guard

#include <string>

#include "console.h"

extern console_class::Console console;

extern string VERSION_URL;
extern string DOWNLOAD_URL;

#define CREG_PATH L"Software\\Annihilus Launcher\\"
#define CREG_ROOTKEY HKEY_CURRENT_USER

#define CREG_KEY_VERSION L"Current Annihilus Version"
#define CREG_KEY_ANNIHILUS_PATH L"Annihilus directory"
#define CREG_KEY_CB_SKIPTOBNET L"skiptobnet"
#define CREG_KEY_CB_WINDOWMODE L"windowmode"
#define CREG_KEY_CB_SHOWCONSOLE L"showconsole"

#endif

