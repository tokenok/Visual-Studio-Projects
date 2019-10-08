/*==========================================================
* D2Ex2
* https://github.com/lolet/D2Ex2
* ==========================================================
* Copyright (c) 2011-2014 Bartosz Jankowski
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
* ==========================================================
*/

#include "stdafx.h"
#include "Misc.h"


char * Misc::decomma(char *str)
{
	char *out = str, *put = str;

	for (; *str != '\0'; ++str)
	{
		if (*str != ',')
			*put++ = *str;
	}
	*put = '\0';

	return out;
}

string Misc::DrawModeToString(int nDrawMode)
{
	switch (nDrawMode)
	{
	case DRAW_MODE_ALPHA_25: return "ALPHA_25";
	case DRAW_MODE_ALPHA_50: return "ALPHA_50";
	case DRAW_MODE_ALPHA_75: return "ALPHA_75";
	case DRAW_MODE_ALPHA_50_BRIGHT: return "ALPHA_50_BRIGHT";
	case DRAW_MODE_INVERTED: return "INVERTED";
	case DRAW_MODE_NORMAL: return "NORMAL";
	case DRAW_MODE_ALPHA_25_BRIGHT: return "ALPHA_25_BRIGHT";
	case DRAW_MODE_BRIGHT: return "BRIGHT";
	}
	return "?";
}


string Misc::RegReadString(const char * key, const char* value, string default) {
	HKEY hKey = {0};

	if (RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS ||
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {

		TCHAR result[3];
		DWORD nSize = 3;

		auto ret = RegQueryValueEx(hKey, value, NULL, NULL, (BYTE*)&result, &nSize);
		
		if (ret == ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return string(result);
		}
	}
	return default;
}

void Misc::RegWriteString(const char * key, const char* name, string value) {
	HKEY hKey = {0};

	if (RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS ||
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS){

		RegSetKeyValue(hKey, NULL, name, REG_SZ, &value, value.size());
		RegCloseKey(hKey);
	}
}

int Misc::RegReadDword(const char * key, const char* value, const DWORD default)
{
	HKEY hKey = { 0 };
	int result;
	
	if (RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS ||
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		DWORD nSize = 4;
		if (RegQueryValueEx(hKey, value, NULL, NULL, (BYTE*)&result, &nSize) == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return result;
		}
	}
	return default;
}

void Misc::RegWriteDword(const char * key, const char* name, const DWORD value)
{
	HKEY hKey = { 0 };

	if (RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS ||
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
	{
		RegSetKeyValue(hKey, NULL, name, REG_DWORD, &value, sizeof(DWORD));
		RegCloseKey(hKey);
	}
}

wstring Misc::ConvertTickToTime(DWORD nTick)
{
	if (nTick == 0) return L"00:00";
	nTick /= 1000;
	wchar_t wTime[70] = { 0 };
	swprintf_s(wTime, 70, L"%.2d:%.2d:%.2d", nTick / 3600, (nTick / 60) % 60, nTick % 60);
	return wTime;
}

int Misc::ClampInt(int min, int max, int value)
{
	if (value < min) {
		return min;
	}
	if (value > max) {
		return max;
	}
	return value;
}

string Misc::TransColor(int col) {
	if (col == 0) return "white"; 
	else if (col == 1) return "red";
	else if (col == 2) return "lightgreen";
	else if (col == 3) return "blue";
	else if (col == 4) return "darkgold";
	else if (col == 5) return "grey";
	else if (col == 6) return "black";
	else if (col == 7) return "gold";
	else if (col == 8) return "orange";
	else if (col == 9) return "yellow";
	else if (col == 10) return "darkgreen";
	else if (col == 11) return "purple";
	else if (col == 12) return "green";
	else if (col == 13) return "white2";
	else if (col == 14) return "black2";
	else if (col == 15) return "darkwhite";
	else if (col == 16) return "hide";
	else if (col == -1) return "null";
	return "";
}

int Misc::TransColor(string str)
{
	if (str == "white") return 0;
	else if (str == "red") return 1;
	else if (str == "lightgreen") return 2;
	else if (str == "blue") return 3;
	else if (str == "darkgold") return 4;
	else if (str == "grey") return 5;
	else if (str == "black") return 6;
	else if (str == "gold") return 7;
	else if (str == "orange") return 8;
	else if (str == "yellow") return 9;
	else if (str == "darkgreen") return 10;
	else if (str == "purple") return 11;
	else if (str == "green") return 12;
	else if (str == "white2") return 13;
	else if (str == "black2") return 14;
	else if (str == "darkwhite") return 15;
	else if (str == "hide") return 16;
	else if (str == "null") return -1;
	return 0;
}

string Misc::TransQuality(int qual) {
	if (qual == 0) return "whatever";
	else if (qual == 1) return "low";
	else if (qual == 2) return "normal";
	else if (qual == 3) return "superior";
	else if (qual == 4) return "magic";
	else if (qual == 5) return "set";
	else if (qual == 6) return "rare";
	else if (qual == 7) return "unique";
	else if (qual == 8) return "crafted";
	else if (qual == 9) return "tempered";
	return "";
}

int Misc::TransQuality(string str)
{
	if (str == "whatever") return 0;
	else if (str == "low") return 1;
	else if (str == "normal") return 2;
	else if (str == "superior") return 3;
	else if (str == "magic") return 4;
	else if (str == "set") return 5;
	else if (str == "rare") return 6;
	else if (str == "unique") return 7;
	else if (str == "crafted") return 8;
	else if (str == "tempered") return 9;
	return 0;
}

DWORD Misc::TransCode(const char* ptCode) // taken from afj666
{
	DWORD ItemCode = 0;

	if (strlen(ptCode) == 3)
	{
		char NewStr[5];
		sprintf_s(NewStr, 5, "%s ", ptCode);
		ItemCode = TransCode(NewStr);
	}
	else
	{
		ItemCode = (DWORD)(ptCode[3] << 24) + (DWORD)(ptCode[2] << 16) +
			(DWORD)(ptCode[1] << 8) + (DWORD)(ptCode[0]);
	}

	return ItemCode;
}

const char* Misc::TransCode(DWORD dwCode)
{
	BYTE* aCode = (BYTE*)&dwCode;
	char ItemCode[5] = { 0 };

	ItemCode[0] = aCode[0];
	ItemCode[1] = aCode[1];
	ItemCode[2] = aCode[2];
	ItemCode[3] = aCode[3];
	ItemCode[4] = 0;

	std::string str(ItemCode);

	return str.c_str();
}

wchar_t* Misc::ConvertIntegers(int in, wchar_t* out) //Taken from EUPK, converted to wchar_t
{
	wchar_t Buffer[100] = { 0 };
	swprintf_s(Buffer, 100, L"%d", in);
	switch (wcslen(Buffer)) {
	case 1:
	case 2:
	case 3:
		wcscpy_s(out, 100, Buffer);
		break;

	case 4:
		swprintf_s(out, 100, L"%c,%c%c%c", Buffer[0], Buffer[1], Buffer[2], Buffer[3]);
		break;
	case 5:
		swprintf_s(out, 100, L"%c%c,%c%c%c", Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4]);
		break;
	case 6:
		swprintf_s(out, 100, L"%c%c%c,%c%c%c", Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5]);
		break;
	case 7:
		swprintf_s(out, 100, L"%c,%c%c%c,%c%c%c", Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6]);
		break;
	case 8:
		swprintf_s(out, 100, L"%c%c,%c%c%c,%c%c%c", Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6], Buffer[7]);
		break;
	case 9:
		swprintf_s(out, 100, L"%c%c%c,%c%c%c,%c%c%c", Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6], Buffer[7], Buffer[8]);
		break;
	case 10:
		swprintf_s(out, 100, L"%c,%c%c%c,%c%c%c,%c%c%c", Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6], Buffer[7], Buffer[8], Buffer[9]);
		break;

	default:
		break;
	}
	return out;
}

void Misc::xorcrypt(char e[], char key[])
{
	int len = strlen(e);
	for (int i = 0, j = 0; i < len; i++)
	{
		e[i] = e[i] ^ key[j++];
		if (j == strlen(key)) j = 0;
	}
}

void Misc::xordecrypt(char *ePtr, char key[])
{
	int len = strlen(ePtr);
	for (int i = 0, j = 0; i < len; i++)
	{
		ePtr[i] = ePtr[i] ^ key[j++];
		if (j == strlen(key)) j = 0;
	}
}


BOOL Misc::WriteBytes(LPVOID lpAddr, LPVOID lpBuffer, DWORD dwLen)
{
	DWORD dwOldProtect;
	if (!VirtualProtect(lpAddr, dwLen, PAGE_READWRITE, &dwOldProtect))
		return FALSE;

	memcpy(lpAddr, lpBuffer, dwLen);

	if (!VirtualProtect(lpAddr, dwLen, dwOldProtect, &dwOldProtect))
		return FALSE;

	return TRUE;
}

BOOL Misc::WriteDword(DWORD* lpAddr, DWORD lpBuffer)
{
	DWORD dwOldProtect;
	if (!VirtualProtect(lpAddr, 4, PAGE_READWRITE, &dwOldProtect))
	{
		D2EXERROR("Failed to patch %d with %d", lpAddr, lpBuffer)
	}
	*lpAddr = lpBuffer;

	if (!VirtualProtect(lpAddr, 4, dwOldProtect, &dwOldProtect))
	{
		D2EXERROR("Failed to patch %d with %d", lpAddr, lpBuffer)
	}
	return TRUE;
}

void Misc::Patch(BYTE bInst, DWORD pAddr, DWORD pFunc, DWORD dwLen, char* Type)
{
	BYTE *bCode = new BYTE[dwLen];
	if (bInst)
	{
		::memset(bCode, 0x90, dwLen);
		bCode[0] = bInst;
		if (pFunc)
		{
			if (bInst == 0xE8 || bInst == 0xE9)
			{
				DWORD dwFunc = pFunc - (pAddr + 5);
				*(DWORD*)&bCode[1] = dwFunc;
			}
			else
				if (bInst == 0x68 || bInst == 0x05 || bInst == 0x5B)
				{
					*(LPDWORD)&bCode[1] = pFunc;
				}
				else
					if (bInst == 0x83)
					{
						*(WORD*)&bCode[1] = (WORD)pFunc;
					}
					else
					{
						bCode[1] = (BYTE)pFunc;
					}
		}
	}
	else
	{
		if (dwLen == 6)
		{
			::memset(bCode, 0x00, dwLen);
			*(DWORD*)&bCode[0] = pFunc;
		}
		else if (dwLen == 4)
			*(DWORD*)&bCode[0] = pFunc;
		else if (dwLen == 2)
			*(WORD*)&bCode[0] = (WORD)pFunc;
		else if (dwLen == 1)
			*(BYTE*)&bCode[0] = (BYTE)pFunc;
	}

	if (!Misc::WriteBytes((void*)pAddr, bCode, dwLen))
	{
		delete[] bCode;
		D2EXERROR("Error while patching game's %s with %d byte(s)", Type, dwLen);
	}
	delete[] bCode;
}

int Misc::GetHexValue(char c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}

	if (c >= 'a' && c <= 'f')
	{
		return c - 'a' + 10;
	}

	if (c >= 'A' && c <= 'F')
	{
		return c - 'A' + 10;
	}

	return -1;
}

int Misc::GetHexSpan(const char* string)
{
	int i = 0;
	for (; string[i] != '\0' && GetHexValue(string[i]) >= 0; i++);
	return i;
}

int Misc::ConvertHexStringToBytes(const char* string, void* data, int length)
{
	unsigned char* bytes = static_cast<unsigned char*>(data);

	int span = GetHexSpan(string);
	int offset = span / 2 + (span & 1) - 1;
	int end = span - 1;

	if (offset >= length)
	{
		offset = length - 1;
		end = length * 2 - 1;
	}

	int count = offset + 1;

	for (int i = end, j = 0; i >= 0 && offset < length; i--, j++)
	{
		int value = GetHexValue(string[i]);

		if ((j & 1) != 0)
		{
			bytes[offset--] |= (value << 4);
		}

		else
		{
			bytes[offset] = value;
		}
	}

	return count;
}

void Misc::ShowMsgBox(char * Msg, ...)
{
	va_list arguments;
	va_start(arguments, Msg);

	int len = _vscprintf(Msg, arguments) + 1;
	char * text = new char[len];
	vsprintf_s(text, len, Msg, arguments);
	va_end(arguments);

	MessageBoxA(0, text, "D2Ex", 0);

	delete[] text;
}

void Misc::Log(char* Msg...)
{
	va_list arguments;
	va_start(arguments, Msg);

	int len = _vscprintf(Msg, arguments) + 1;
	char * text = new char[len];
	vsprintf_s(text, len, Msg, arguments);
	va_end(arguments);

	char sTime[9];
	_strtime_s(sTime, 9);
	FILE* plik;
	fopen_s(&plik, "D2Ex.log", "a");
	if (plik)
	{
		fprintf(plik, "[%s] %s\n", sTime, text);
		fclose(plik);
	}
#if _DEBUG
	OutputDebugStringA(text);
	OutputDebugStringA("\n");
#endif
	delete[] text;
}

void Misc::Log(wchar_t* Msg, ...)
{
	va_list arguments;
	va_start(arguments, Msg);

	int len = _vscwprintf(Msg, arguments) + 1;
	wchar_t * text = new wchar_t[len];
	vswprintf_s(text, len, Msg, arguments);
	va_end(arguments);

	wchar_t sTime[9];
	_wstrtime_s(sTime, 9);
	FILE* plik;
	fopen_s(&plik, "D2Ex.log", "a");
	if (plik)
	{
		fwprintf(plik, L"[%s] %s\n", sTime, text);
		fclose(plik);
	}
#if _DEBUG
	OutputDebugStringW(text);
	OutputDebugStringW(L"\n");
#endif
	delete[] text;
}



void Misc::Debug(const char * szFunction, char *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	int len = _vscprintf(format, arguments) + 1;
	char * text = new char[len];
	vsprintf_s(text, len, format, arguments);
	va_end(arguments);

	SYSTEMTIME t = { 0 };
	GetLocalTime(&t);

	char* out = new char[strlen(text) + 24 + strlen(szFunction) + 3];
	sprintf_s(out, strlen(text) + 24 + strlen(szFunction) + 3, "[%04d-%02d-%02d %02d:%02d:%02d] %s: %s\n", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, szFunction, text);

	OutputDebugString(out);
	printf(out);

	delete[] out;
	delete[] text;
};


void Misc::Debug(const char * szFunction, wchar_t *format, ...)
{
	va_list arguments;
	va_start(arguments, format);

	int len = _vscwprintf(format, arguments) + 1;
	wchar_t * text = new wchar_t[len];
	vswprintf_s(text, len, format, arguments);
	va_end(arguments);
	SYSTEMTIME t = { 0 };
	GetLocalTime(&t);

	wchar_t* out = new wchar_t[wcslen(text) + 24 + (strlen(szFunction)*2) + 3];
	swprintf_s(out, wcslen(text) + 24 + (strlen(szFunction) * 2) + 3, L"[%04d-%02d-%02d %02d:%02d:%02d] %s: %s\n", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, szFunction, text);

	OutputDebugStringW(out);
	wprintf(out);

	delete[] text;
	delete[] out;
};


int Misc::round(double a) 
{
	return int(a + 0.5);
}

void Misc::WideToChar(CHAR* Dest, const WCHAR* Source)
{
	WideCharToMultiByte(CP_ACP, 0, Source, -1, Dest, 255, NULL, NULL);
}

void Misc::WideToChar(string& Dest, const wstring Source)
{
	Dest.assign(Source.begin(), Source.end());
}

void Misc::CharToWide(wstring& Dest, const string Source)
{
	Dest.assign(Source.begin(), Source.end());
}


int Misc::ConvertBytesToHexString(const void* data, int dataSize, char* string, int stringSize, char delimiter)
{
	const unsigned char* bytes = reinterpret_cast<const unsigned char*>(data);
	int count = 0;

	for (int i = 0; i < dataSize && stringSize - count > 2; i++)
	{
		unsigned char byte = bytes[i];

		if (delimiter != 0 && stringSize - count > 3 && i < dataSize - 1)
		{
			count += sprintf_s(string + count, stringSize - count, "%.2x%c", byte, delimiter);
		}

		else
		{
			count += sprintf_s(string + count, stringSize - count, "%.2x", byte);
		}
	}

	return count;
}

void Misc::ToUpperCase(char * q)
{
	unsigned char c;
	while (*q) { c = *q; *q = toupper(c); q++; }
}

void Misc::ToUpperCase(wchar_t * q)
{
	wchar_t c;
	while (*q) { c = *q; *q = toupper(c); q++; }
}

char* Misc::ConvertStan(int id)
{
	switch (id)
	{
	case 0:
		return "Offline";
	case 1:
		return "Selecting char";
	case 2:
		return "In a Channel :";
	case 3:
		return "In a Game :";
	case 4:
		return "Private Channel";
	case 5:
		return "Private Game";
	case 6:
		return "Private Lobby";
	}
	return "???";
}

char* Misc::ConvertClassID(int id)
{
	switch (id)
	{
	case 0:
		return "Amazon";
	case 1:
		return "Sorceress";
	case 2:
		return "Necromancer";
	case 3:
		return "Paladin";
	case 4:
		return "Barbarian";
	case 5:
		return "Druid";
	case 6:
		return "Assassin";
	}
	return "???";
}

void Misc::CharToWide(char* lpMultiByteStr, int cbMultiByte, wchar_t* lpWideCharStr, int cchWideChar)
{
	MultiByteToWideChar(0, 1, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
	for (DWORD i = 0; i < wcslen(lpWideCharStr); i++)
	{
		if (lpWideCharStr[i] >= 0xFF && lpWideCharStr[i + 1] == L'c')
		{
			if (lpWideCharStr[i + 2] >= L'0' && lpWideCharStr[i + 2] <= L'9')
			{
				lpWideCharStr[i] = 0xFF;
			}
		}

	}
}

