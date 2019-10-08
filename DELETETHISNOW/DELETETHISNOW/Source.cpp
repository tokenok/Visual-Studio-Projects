#include <Windows.h>

int main() {
	ShellExecute(NULL, L"open", L"C:\\Program Files\\iTunes\\iTunes.exe", NULL, NULL, SW_SHOW);
	ShellExecute(NULL, L"open", L"C:\Windows\\explorer.exe", NULL, NULL, SW_SHOW);

	return 0;
}