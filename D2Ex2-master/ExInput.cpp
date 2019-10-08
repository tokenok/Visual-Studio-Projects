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
#include "ExInput.h"
#include "ExEvents.h"
#include "ExAim.h"
#include "ExAutoTele.h"
#include "ExChicken.h"
#include "ExMultiRes.h"
#include "ExBox.h"
#include "ExDownload.h"

#include <boost/lexical_cast.hpp>

// IMPORT FROM SCRAP Project
void ExInput::DefineBindings() {
#ifdef D2EX_PVM_BUILD
	D2Funcs.STORM_RegisterKeyDown(D2Funcs.D2GFX_GetHwnd(), VK_ATNext, &ExAutoTele::b_TeleportNext);
	D2Funcs.STORM_RegisterKeyDown(D2Funcs.D2GFX_GetHwnd(), VK_ATPrev, &ExAutoTele::b_TeleportPrev);
	D2Funcs.STORM_RegisterKeyDown(D2Funcs.D2GFX_GetHwnd(), VK_ATWP, &ExAutoTele::b_TeleportWP);
	D2Funcs.STORM_RegisterKeyDown(D2Funcs.D2GFX_GetHwnd(), VK_FastTP, &ExChicken::b_FastTP);
#endif
}

void ExInput::UndefineBindings() {
#ifdef D2EX_PVM_BUILD
	D2Funcs.STORM_UnregisterKeyDown(D2Funcs.D2GFX_GetHwnd(), VK_ATNext, &ExAutoTele::b_TeleportNext);
	D2Funcs.STORM_UnregisterKeyDown(D2Funcs.D2GFX_GetHwnd(), VK_ATPrev, &ExAutoTele::b_TeleportPrev);
	D2Funcs.STORM_UnregisterKeyDown(D2Funcs.D2GFX_GetHwnd(), VK_ATWP, &ExAutoTele::b_TeleportWP);
	D2Funcs.STORM_UnregisterKeyDown(D2Funcs.D2GFX_GetHwnd(), VK_FastTP, &ExChicken::b_FastTP);
#endif
}

void ExInput::RegisterMsgs(sMsg* pMsgList, int nCount) {
	HWND hWnd = D2Funcs.D2GFX_GetHwnd();
	ASSERT(hWnd)
		for (int i = 0; i < nCount; i++) {
			switch (pMsgList[i].MsgType) {
				case 0:
					D2Funcs.STORM_RegisterMsg(hWnd, pMsgList[i].wParam, pMsgList[i].fnCallBack);
					break;
				case 1:
					D2Funcs.STORM_RegisterCommand(hWnd, pMsgList[i].wParam, pMsgList[i].fnCallBack);
					break;
				case 2:
					D2Funcs.STORM_RegisterKeyUp(hWnd, pMsgList[i].wKey, pMsgList[i].fnCallBack);
					break;
				case 3:
					D2Funcs.STORM_RegisterKeyDown(hWnd, pMsgList[i].wKey, pMsgList[i].fnCallBack);
					break;
			}
		}
}

void ExInput::UnregisterMsgs(sMsg* pMsgList, int nCount) {
	HWND hWnd = D2Funcs.D2GFX_GetHwnd();
	ASSERT(hWnd);
	for (int i = 0; i < nCount; i++) {
		switch (pMsgList[i].MsgType) {
			case 0:
				D2Funcs.STORM_UnregisterMsg(hWnd, pMsgList[i].wParam, pMsgList[i].fnCallBack);
				break;
			case 1:
				D2Funcs.STORM_UnregisterCommand(hWnd, pMsgList[i].wParam, pMsgList[i].fnCallBack);
				break;
			case 2:
				D2Funcs.STORM_UnregisterKeyUp(hWnd, pMsgList[i].wKey, pMsgList[i].fnCallBack);
				break;
			case 3:
				D2Funcs.STORM_UnregisterKeyDown(hWnd, pMsgList[i].wKey, pMsgList[i].fnCallBack);
				break;
		}
	}
}


void __stdcall ExInput::RealmInput(wchar_t* Text, int Color) {
	//Nie mozna bylo odnalezc uzytkownika
	//Bzik*Bzik is using Diablo II Lord of Destruction and is currently in private game "Emce".
	//Uzytkownik ostatni raz byl widziany : Sun Feb 07 19:47:19
	//Twoja lista przyjaciol na Angrenost Realm:
	//.,;'-----------------------------------;,.
	//1: *ProdiGy, w grze "Tak",  uzywa Diablo II Lord of Destruction
	//2: InteR, offline
	//Rad dodal Cie do swojej listy przyjaciol
	//68: *Arni, siedzi na kanale "Diablo II",,  uzywa Diablo II Lord of Destruction
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//Koniec friend listy :S
	wstring Msg = Text;
	bool Pass = true;
	if (Msg.find(L"<BattleNet>:") != wstring::npos) {
		Msg.erase(0, 13);
#ifdef D2EX_CLOSED_BNET
		if (Msg.find(L"That user is not logged on.") != wstring::npos ||
			Msg.find(L"That character is not logged on.") != wstring::npos ||
			Msg.find(L"is using Diablo II") != wstring::npos)
#else
		if (Msg.find(L"Nie mozna bylo odnalezc uzytkownika") != wstring::npos ||
			Msg.find(L"Unknown user.") != wstring::npos ||
			Msg.find(L"is using Diablo II Lord of Destruction") != wstring::npos ||
			Msg.find(L"User was last seen on:") != wstring::npos ||
			Msg.find(L"Uzytkownik ostatni raz byl widziany") != wstring::npos)
#endif
			Pass = ExFriends::OnWhoisRequest(Msg);
	}

	if (Pass) D2Funcs.D2CLIENT_PrintGameString(Text, Color);
}

DWORD __fastcall ExInput::GameInput(wchar_t* wMsg) {
	static char In[400], *str, *tok;
	Misc::WideToChar(In, wMsg);
	str = strtok_s(In, " ", &tok);

	if (_stricmp(str, "#r") == 0) {
		str = strtok_s(NULL, " ", &tok);
		CreateThread(0, 0, &ExOOG::Rejoin, str, 0, 0);
		return -1;
	}
#if _DEBUG
	if (_stricmp(str, "#icon") == 0) {
		ExEventTextMsg hEvent;
		hEvent.Argument = 0;
		hEvent.MsgType = EXEVENT_TEXTMSG;
		hEvent.Color = COL_RED;
		hEvent.wX = -1;
		hEvent.wY = 50;
		hEvent.Sound = 10;
		hEvent.P_A6 = 0xA6;
		int Dmg = 10000;
		sprintf_s(hEvent.szMsg, 255, "%d !", Dmg);
		hEvent.PacketLen = 0xE + strlen(hEvent.szMsg) + 1;

		static int eLen = 0;
		D2Funcs.D2NET_ReceivePacket(&eLen, (BYTE*)&hEvent, hEvent.PacketLen);

		return -1;
	}
	if (_stricmp(str, "#eventst") == 0)
	{
		ExEventTextMsg hEvent;
		hEvent.Argument = 0;
		hEvent.MsgType = EXEVENT_TEXTMSG;
		hEvent.Color = COL_RED;
		hEvent.wX = -1;
		hEvent.wY = 150;
		hEvent.Sound = 10;
		hEvent.P_A6 = 0xA6;
		int Dmg = 10000;
		sprintf_s(hEvent.szMsg, 255, "%d !", Dmg);
		hEvent.PacketLen = 0xE + strlen(hEvent.szMsg) + 1;

		ExEventOverhead hEvent2;
		hEvent2.MsgType = EXEVENT_OVERHEAD;
		hEvent2.Color = COL_WHITE;
		hEvent2.UnitId = D2Funcs.D2CLIENT_GetPlayer()->dwUnitId;
		hEvent2.P_A6 = 0xA6;

		_snprintf_s(hEvent2.szCellPath, 255, 255, "data\\D2Ex\\Blobs");
		hEvent2.PacketLen = 0xE + strlen(hEvent2.szCellPath) + 1;

		static int eLen = 0;
		for (int i = 0; i < 4; ++i) {
			hEvent.Color = hEvent2.Color = rand() % 16;
			hEvent2.CellID = rand() % 4;
			D2Funcs.D2NET_ReceivePacket(&eLen, (BYTE*)&hEvent, hEvent.PacketLen);
			D2Funcs.D2NET_ReceivePacket(&eLen, (BYTE*)&hEvent2, hEvent2.PacketLen);
			
		}
		return -1;
	}
	if (_stricmp(str, "#icon2") == 0)
	{
		ExEventDownload pEvent = {};
		pEvent.P_A6 = 0xA6;
		pEvent.MsgType = EXEVENT_DOWNLOAD;
		pEvent.bExec = 0;
		strcpy_s(pEvent.szURL, 255, "http://download.thinkbroadband.com/1GB.zip");
		if (pEvent.szURL[0])
			pEvent.PacketLen = 14 + strlen(pEvent.szURL) + 1;
		else
			pEvent.PacketLen = 15;

		static int eLen = 0;
		D2Funcs.D2NET_ReceivePacket(&eLen, (BYTE*)&pEvent, pEvent.PacketLen);

		return -1;
	}
	static exId test_ui[100] = {exnull_t};
	if (strcmp(In, "#t1") == 0) {

		for (int i = 0; i < 100; ++i) {
			wostringstream str2;
			str2 << "Tescik " << i;
			test_ui[i] = gExGUI->add(new ExTextBox(10, 10 + (15 * i), COL_WHITE, 5, str2.str(), NULL));
		}
		return -1;
	}
	if (strcmp(In, "#t2") == 0) {
		ExDownload::ShowHide();
	}
	if (strcmp(In, "#t3") == 0) {
		auto test = blizz_unique_ptr<char>((char*)D2ASMFuncs::D2WIN_ReadFileFromMPQ("DATA\\LOCAL\\FONT\\LATIN\\README.TXT", NULL, NULL));
		DEBUGMSG("Read text with data: %s", test)
	}
	if (strcmp(In, "#dlstresstest") == 0) {
		const char* URLs[] = {
			"http://lolet.ml/test/file1.test", "http://lolet.ml/test/file2.test", "http://lolet.ml/test/file3.test", "http://lolet.ml/test/file4.test", "http://lolet.ml/test/file5.test",
			"http://lolet.ml/test/file6.test", "http://lolet.ml/test/file7.test", "http://lolet.ml/test/file8.test", "http://lolet.ml/test/file9.test", "http://lolet.ml/test/file10.test",
			"http://lolet.ml/test/file11.test", "http://lolet.ml/test/file12.test", "http://lolet.ml/test/file13.test", "http://lolet.ml/test/file14.test", "http://lolet.ml/test/file15.test",
			"http://lolet.ml/test/file16.test", "http://lolet.ml/test/file17.test", "http://lolet.ml/test/file18.test", "http://lolet.ml/test/file19.test", "http://lolet.ml/test/file20.test",
			"http://lolet.ml/test/file21.test", "http://lolet.ml/test/file22.test", "http://lolet.ml/test/file23.test", "http://lolet.ml/test/file24.test", "http://lolet.ml/test/file25.test",
			"http://lolet.ml/test/file26.test", "http://lolet.ml/test/file27.test", "http://lolet.ml/test/file28.test", "http://lolet.ml/test/file29.test", "http://lolet.ml/test/file30.test"
		};
		for (int i = 0; i < 30; ++i) {
			ExEventDownload pEvent = {};
			pEvent.P_A6 = 0xA6;
			pEvent.MsgType = EXEVENT_DOWNLOAD;
			pEvent.bExec = 0;
			strcpy_s(pEvent.szURL, 255, URLs[i]);
			if (pEvent.szURL[0])
				pEvent.PacketLen = 14 + strlen(pEvent.szURL) + 1;
			else
				pEvent.PacketLen = 15;

			static int eLen = 0;
			D2Funcs.D2NET_ReceivePacket(&eLen, (BYTE*)&pEvent, pEvent.PacketLen);
		}
		return -1;
	}
#ifdef D2EX_MULTIRES
	if (strcmp(In, "#fs") == 0) {
		ExMultiRes::enterFullscreen();
		return -1;
	}
#endif
#endif

#ifdef D2EX_ENABLE_PACKET_COMMANDS
	if (_stricmp(str, "#recv") == 0) {
		str = strtok_s(NULL, " ", &tok);
		BYTE data[512];
		static int eLen = 0;
		if (strlen(str) == 0) return 0;
		int length = Misc::ConvertHexStringToBytes(Misc::decomma(str), data, 512);
		D2Funcs.D2NET_ReceivePacket(&eLen, data, length);
		/*	wchar_t a[2000] = {9};
		static char astr[3*512];
		Misc::ConvertBytesToHexString(data,length,astr,3*512,',');
		static wchar_t wdata[3*512];*/
		//Misc::CharToWide(astr,strlen(astr)+1,wdata,strlen(astr)+1);
		//	swprintf(a,L"[%d,%d] : %s",eLen,length, wdata);
		//D2Funcs.D2CLIENT_PrintGameString(a,1);
		char astr[2048];
		Misc::ConvertBytesToHexString(data, length, astr, 2047, ',');
		wostringstream wdata;
		wdata << "Received " << astr;
		D2Funcs.D2CLIENT_PrintGameString(wdata.str().c_str(), 1);
		return -1;
	}
	if (_stricmp(str, "#decompress") == 0) {
		str = strtok_s(NULL, " ", &tok);
		BYTE data[512], data_out[512];
		if (strlen(str) == 0) return 0;
		int length = Misc::ConvertHexStringToBytes(Misc::decomma(str), data, 512);
		D2Funcs.FOG_DecompressPacket(data_out, length, data, length);

		char astr[2048];
		Misc::ConvertBytesToHexString(data_out, length, astr, 2047, ',');
		wostringstream wdata;
		wdata << "Decompressed to " << astr;
		D2Funcs.D2CLIENT_PrintGameString(wdata.str().c_str(), 1);
		return -1;
	}
	if (_stricmp(str, "#send") == 0) {
		int i = 1;
		str = strtok_s(NULL, " ", &tok);
		if (!str) return 0;
		char* str2 = strtok_s(NULL, " ", &tok);
		if (str2) i = atoi(str2);
		BYTE data[512];
		if (strlen(str) == 0) return 0;
		int length = Misc::ConvertHexStringToBytes(Misc::decomma(str), data, sizeof(data));
		for (int z = 0; z < i; z++) {
			D2Funcs.D2NET_SendPacket(length, 0, data);
			*D2Vars.D2CLIENT_SentBytes += length;
			*D2Vars.D2CLIENT_SentPackets++;
			Sleep(100);
		}
		char astr[2048];
		Misc::ConvertBytesToHexString(data, length, astr, 2047, ',');
		wostringstream wdata;
		wdata << "Sent " << astr;
		D2Funcs.D2CLIENT_PrintGameString(wdata.str().c_str(), 1);
		wostringstream n;
		n << i << " times";
		D2Funcs.D2CLIENT_PrintGameString(n.str().c_str(), 1);
		return -1;
	}
	if (_stricmp(str, "#blocked") == 0) {
		wostringstream wdata;
		wdata << "Currently blocked packets (C->S): ";
		for (auto it = BlockedPackets.begin(); it != BlockedPackets.end(); ++it) {
			wdata << hex << *it << ", ";
		}
		D2Funcs.D2CLIENT_PrintGameString(wdata.str().c_str(), 1);

		return -1;
	}
	if (_stricmp(str, "#block") == 0) {
		int blocked;
		char* str2 = strtok_s(NULL, " ", &tok);
		if (!str2)
			return 0;
		BYTE data[512];
		int length = Misc::ConvertHexStringToBytes(str2, data, sizeof(data));

		blocked = data[0];
		if (blocked == 0 || blocked > 255)
			return 0;
		for (auto it = BlockedPackets.begin(); it != BlockedPackets.end(); ++it) {
			if (*it == (BYTE)blocked) {
				wostringstream wdata;
				wdata << "Packet 0x" << hex << *it << " has been unblocked!";
				D2Funcs.D2CLIENT_PrintGameString(wdata.str().c_str(), 1);

				BlockedPackets.erase(it);
				return -1;
			}
		}
		BlockedPackets.push_back((BYTE)blocked);
		wostringstream wdata;
		wdata << "Packet 0x" << hex << blocked << " has been blocked!";
		D2Funcs.D2CLIENT_PrintGameString(wdata.str().c_str(), 1);
		return -1;
	}
#endif
	return 0;
}

DWORD __stdcall ExInput::PacketOutput(int PacketLen, int _1, BYTE *aPacket) {

	for (auto it = BlockedPackets.begin(); it != BlockedPackets.end(); ++it) {
		if (*it == aPacket[0]) {
			wostringstream wdata;
			wdata << "Packet 0x" << hex << *it << " has been just blocked!";
			D2Funcs.D2CLIENT_PrintGameString(wdata.str().c_str(), 1);
			return 0;
		}
	}
	return D2Funcs.D2NET_SendPacket(PacketLen, _1, aPacket);
}

DWORD __fastcall ExInput::PacketInput(BYTE* aPacket, int aLen) //CODE UNSAFE FOR WARDEN
{
	static char str[512 * 4];
	if (aPacket[0] == 0xAE) return 0;
	Misc::ConvertBytesToHexString(aPacket, aLen, str, 512 * 4, ',');
	Misc::Log("RCV : %s", str);
	return 1;
}

__declspec(naked) void ExInput::GameInput_STUB() {
	__asm
	{
		pushad
#ifdef VER_111B
			mov ecx, ebx
			call ExInput::GameInput
			cmp eax, -1
			popad
			je BlockIt
			call  D2Ptrs.D2CLIENT_ChatInput_I;
		ret

BlockIt :

		xor eax, eax
			ret
#else
			call ExInput::GameInput
			cmp eax, -1
			popad
			je BlockIt
			jmp D2Ptrs.D2CLIENT_ChatInput_I;
		ret

BlockIt :
		mov     ecx, 5; UIMode
			mov     edx, 1; HowSet
			push    0; bClearScreen
			call    D2Funcs.D2CLIENT_SetUiVar

			xor eax, eax
			ret 4
#endif

	}
}

__declspec(naked) void ExInput::PacketInput_STUB() //CODE UNSAFE FOR WARDEN
{
	__asm
	{
		pop ebp;
		pushad;

		call ExInput::PacketInput;
		test eax, eax;

		popad;
		jnz OldCode;

		mov edx, 0;

OldCode:
		call D2Ptrs.D2NET_ReceivePacket_I;

		push ebp;
		ret;
	}
}

//token added these
vector<HWND> ExInput::g_D2HWNDS;
string getwindowtext(HWND wnd) {
	int len = GetWindowTextLength(wnd) + 1;
	char* text = new char[len];
	GetWindowText(wnd, text, len);
	string ret(text);
	delete[] text;
	return ret;
}
BOOL CALLBACK ExInput::EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	switch (lParam) {
		case 1:{
			string text = getwindowtext(hwnd);
			if (text.find("Diablo II") != string::npos && text.find("- Annihilus") == string::npos)
				g_D2HWNDS.push_back(hwnd);
			break;
		}
	}
	return TRUE;
}

struct FilterInfo {
	DWORD dwcode;
	DWORD QualityNo;
	DWORD wnamestr;
};

//end token additions

LONG WINAPI ExInput::GameWindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	//tokens vars
	static bool adddown = false;//prevents multiple items from being removed by holding down 'hide' key by forcing it to be released first
	static bool remdown = false;
	static bool cursortrap = false;
	static unsigned int last_filter_pos;

	switch (uMsg) {
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_MOUSEMOVE:{
			if (gExGUI->io(uMsg, wParam))
				return 0;
			break;
		}

		//CASE 'NEXTMSG'
		//token added this
		case WM_COPYDATA:{
			COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
			switch (((COPYDATASTRUCT*)lParam)->dwData) {
				case ADD_FILTER_ITEM:
				case REMOVE_FILTER_ITEM:{
					FilterInfo* info = (FilterInfo*)cds->lpData;

					ItemConfig ic;
					ic.Code = info->dwcode;
					ic.Quality = info->QualityNo;
					ic.Color = 16;

					int hiddenpos = -1;
					int foundpos = -1;
					for (UINT i = 0; i < ItemArray.size(); i++) {
						if (ItemArray[i].Code == ic.Code && ItemArray[i].Quality == ic.Quality) {
							foundpos = (int)i;
							if (ItemArray[i].Color == 16) {
								hiddenpos = (int)i;
								break;
							}
						}
					}
					last_filter_pos = foundpos != -1 ? foundpos : ItemArray.size();

					string code(Misc::TransCode(info->dwcode), 4);
					wstring wname(D2Funcs.D2LANG_GetLocaleText(info->wnamestr));
					string type(wname.begin(), wname.end());
					string output = type + "(" + code + "), " + Misc::TransQuality(ic.Quality);
					wstring woutput = wstring(output.begin(), output.end());

					if (ADD_FILTER_ITEM == cds->dwData) {
						if (hiddenpos == -1) {//not being hidden, (hide it)
							D2Funcs.D2CLIENT_PrintGameString((L"hiding: " + woutput).c_str(), COL_DARK_GOLD);
							if (foundpos != -1) //if being colored a different color, but not being hidden, hide it
								ItemArray[foundpos].Color = 16;
							else //otherwise add it to list
								ItemArray.push_back(ic);
						}
					}
					else if (REMOVE_FILTER_ITEM == cds->dwData) {
						if (hiddenpos > -1) {//being hidden, (show it)
							D2Funcs.D2CLIENT_PrintGameString((L"showing: " + woutput).c_str(), COL_GOLD);
							ItemArray[hiddenpos].Color = -1;
						}
					}
					break;
				}
			}
			break;
		}
		case WM_CLOSE:{
			//cursortrap = false;
			break;
		}
	};
#if defined(D2EX_MULTIRES) && defined(_DEBUG)
	if (uMsg == WM_MOUSEWHEEL && (GET_KEYSTATE_WPARAM(wParam) & MK_CONTROL)) {
		if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) {
			if (*D2Vars.D2GFX_GfxMode > 0) {
				unsigned int x, y;
				ExMultiRes::D2GFX_GetModeParams(*D2Vars.D2GFX_GfxMode - 1, &x, &y);
				DEBUGMSG("Changing resolution to %dx%d", x, y);
				ExMultiRes::D2CLIENT_SetResolution((*D2Vars.D2GFX_GfxMode) - 1);
			}
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
			if ((unsigned int)*D2Vars.D2GFX_GfxMode < ExMultiRes::lResModes.size() + 2) {
				unsigned int x, y, r;
				if (ExMultiRes::GFX_GetRenderType() == VIDEO_MODE_GLIDE) {
					for (int i = *D2Vars.D2GFX_GfxMode + 1; i < ExMultiRes::lResModes.size(); ++i) {
						ExMultiRes::D2GFX_GetModeParams(i, &x, &y);
						if (x == 1600 && y == 1200 ||
							(x == 1280 && y == 1024) ||
							(x == 1024 && y == 768) ||
							(x == 800 && y == 600) ||
							(x == 640 && y == 480)) {
							r = i;
							break;
						}
					}
				}
				else {
					ExMultiRes::D2GFX_GetModeParams(*D2Vars.D2GFX_GfxMode + 1, &x, &y);
					r = *D2Vars.D2GFX_GfxMode + 1;
				}

				DEBUGMSG("Changing resolution to %dx%d", x, y);
				ExMultiRes::D2CLIENT_SetResolution(r);
			}
		}
		return 0;
	}
#endif

	//token did this - start
	DWORD key = wParam;
	if ((GetKeyState(VK_MENU) & 0x8000) != 0) key += 256;
	if ((GetKeyState(VK_SHIFT) & 0x8000) != 0) key += 256 * 4;
	if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) key += 256 * 2;
	if ((GetKeyState(VK_LWIN) & 0x8000) != 0) key += 256 * 8;
	if ((GetKeyState(VK_RWIN) & 0x8000) != 0) key += 256 * 8;

	if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) {
		if (key == CursorLockHKey) {
			if (!cursortrap) {
				cursortrap = true;
				if (*D2Vars.D2CLIENT_InGame)
					D2Funcs.D2CLIENT_PrintGameString(L"Cursor Locked", COL_DARK_GOLD);

				RECT rc;
				GetClientRect(hWnd, &rc);
				MapWindowPoints(hWnd, NULL, (LPPOINT)&rc, 2);
				if (FullScreen) rc = {rc.left, rc.top, rc.left + *D2Vars.D2CLIENT_ScreenWidth, rc.top + *D2Vars.D2CLIENT_ScreenHeight};
				ClipCursor(&rc);
			}
			else {
				cursortrap = false;
				if (*D2Vars.D2CLIENT_InGame)
					D2Funcs.D2CLIENT_PrintGameString(L"Cursor Unlocked", COL_DARK_GOLD);
				ClipCursor(NULL);
			}
		}
		if (!D2Vars.D2CLIENT_UIModes[UI_CHAT] && (key == ADDHK || key == ADDHK + 256 || key == ADDHK + 768 || key == REMOVEHK || key == REMOVEHK + 256 || key == REMOVEHK + 768)) {
			//Get item from cursor hover
			UnitAny* pItem = (*D2Vars.D2CLIENT_SelectedItem);
			if (!pItem) pItem = D2Funcs.D2CLIENT_GetSelectedUnit();
			if (!pItem || pItem->dwType != UNIT_ITEM) 
				return (LONG)CallWindowProcA(OldWNDPROC, hWnd, uMsg, wParam, lParam);

			ItemsTxt* pTxt = D2Funcs.D2COMMON_GetItemText(pItem->dwClassId);

			FilterInfo info;
			info.QualityNo = pItem->pItemData->QualityNo;
			info.dwcode = pTxt->dwcode;
			info.wnamestr = pTxt->wnamestr;

			COPYDATASTRUCT cds = {0};
			cds.cbData = sizeof(info);
			cds.lpData = &info;

			if (key == ADDHK || key == ADDHK + 256 || key == ADDHK + 768) {
				if (!adddown) {
					adddown = true;
					cds.dwData = ADD_FILTER_ITEM;

					g_D2HWNDS.clear();
					EnumWindows(EnumWindowsProc, 1);
					for (UINT i = 0; i < g_D2HWNDS.size(); i++)
						SendMessage(g_D2HWNDS[i], WM_COPYDATA, (WPARAM)GetActiveWindow(), (LPARAM)(LPVOID)&cds);

					if (last_filter_pos >= ItemArray.size()) 
						return (LONG)CallWindowProcA(OldWNDPROC, hWnd, uMsg, wParam, lParam);

					string code(Misc::TransCode(ItemArray[last_filter_pos].Code), 4);
					string configline = code + ", " + Misc::TransQuality(ItemArray[last_filter_pos].Quality) + ", " + Misc::TransColor(ItemArray[last_filter_pos].Color);
					WritePrivateProfileString("Item Config", boost::lexical_cast<string>(last_filter_pos + 1).c_str(), configline.c_str(), ConfigIni.c_str());
				}
			}
			if (key == REMOVEHK || key == REMOVEHK + 256 || key == REMOVEHK + 768) {
				if (!remdown) {
					remdown = true;
					cds.dwData = REMOVE_FILTER_ITEM;

					g_D2HWNDS.clear();
					EnumWindows(EnumWindowsProc, 1);
					for (UINT i = 0; i < g_D2HWNDS.size(); i++)
						SendMessage(g_D2HWNDS[i], WM_COPYDATA, (WPARAM)GetActiveWindow(), (LPARAM)(LPVOID)&cds);

					if (last_filter_pos >= ItemArray.size()) 
						return (LONG)CallWindowProcA(OldWNDPROC, hWnd, uMsg, wParam, lParam);
					
					string code(Misc::TransCode(ItemArray[last_filter_pos].Code), 4);
					string configline = code + ", " + Misc::TransQuality(ItemArray[last_filter_pos].Quality) + ", " + Misc::TransColor(ItemArray[last_filter_pos].Color);
					WritePrivateProfileString("Item Config", boost::lexical_cast<string>(last_filter_pos + 1).c_str(), configline.c_str(), ConfigIni.c_str());
				}
			}
		}
		if (key == InviteAllHKey) {
			for (RosterUnit* pUnit = *D2Vars.D2CLIENT_Roster; pUnit; pUnit = pUnit->pNext) {
				if (pUnit->dwUnitId == D2Funcs.D2CLIENT_GetPlayer()->dwUnitId)
					continue;

				DWORD Flaga = ExParty::GetPvpFlags(pUnit->dwUnitId);
				if (!(Flaga & (PVP_ALLIED | PVP_ALLIED_WITH_YOU | PVP_INVITED_BY_YOU | PVP_INVITED_YOU))) {
					BYTE aPacket[6];
					aPacket[0] = 0x5E;
					aPacket[1] = PB_INVITE_PLAYER;
					*(DWORD*)&aPacket[2] = pUnit->dwUnitId;
					D2Funcs.D2NET_SendPacket(6, 1, aPacket);
					*D2Vars.D2CLIENT_SentBytes += 6;
					*D2Vars.D2CLIENT_SentPackets++;
				}
			}
		}
		if (key == TransmuteHKey) {
			if (D2Vars.D2CLIENT_UIModes[UI_CUBE]) {

			}
		}

		if (key == BuffHKey) {
			ExPrecast::Do();
		}
		if (key == 87) {//w
			if (!D2Vars.D2CLIENT_UIModes[UI_CHAT])
				ExPrecast::Switch();
		}

		//token -end
		if (wParam == 'V') {
			if (GetKeyState(VK_CONTROL) < 0) {
				if ((D2Vars.D2CLIENT_UIModes[UI_CHAT]) && OpenClipboard(0)) {
					INPUT inp[2];
					memset(inp, 0, sizeof(INPUT));
					inp[0].type = INPUT_KEYBOARD;
					inp[0].ki.dwFlags = KEYEVENTF_UNICODE;
					inp[1] = inp[0];
					inp[1].ki.dwFlags |= KEYEVENTF_KEYUP;
					if (!GetClipboardData(CF_UNICODETEXT)) return 0;
					for (wchar_t* p = (wchar_t*)GetClipboardData(CF_UNICODETEXT); *p; p++) {
						if (*p == VK_RETURN) continue;
						inp[0].ki.wScan = inp[1].ki.wScan = *p;
						SendInput(2, inp, sizeof(INPUT));
					}
					CloseClipboard();
					return 0;
				}
			}
		}
		if (wParam == VK_CONTROL) {
			gControl = true;
		}
#ifdef D2EX_EXAIM_ENABLED
		if (wParam == VK_INSERT && D2Vars.D2CLIENT_UIModes[UI_CHAT] == 0) { ExAim::DoAttack(); return 0; }
#endif
#ifdef D2EX_SCRAP_HACKS
		if (StillSwitch) {
			if (D2Funcs.D2CLIENT_GetPlayer() && !D2Vars.D2CLIENT_UIModes[UI_CHAT]) {
				WORD vKey = GetKeyBind(44, false); // Fast Switch
				WORD vKey2 = GetKeyBind(44, true); //Alternative HotKey
				if (wParam == vKey || wParam == vKey2) {
					lParam = 0x00110001;
				}
			}
		}
#endif
	}
	if (uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) {
		if (wParam == VK_CONTROL) {
			gControl = false;
		}
		if (key == REMOVEHK || key == REMOVEHK + 256 || key == REMOVEHK + 768)
			remdown = false;
		if (key == ADDHK || key == ADDHK + 256 || key == ADDHK + 768)
			adddown = false;
	}
	//move items from one store to another or drop an item onto the ground from anywhere
	if (uMsg == WM_LBUTTONDOWN && GetAsyncKeyState(VK_CONTROL) < 0 && GetForegroundWindow() == hWnd) {
		//fix for tokenhack
		string windowname = getwindowtext(hWnd);
		if (windowname.find("ROLLING") != string::npos) {
			return (LONG)CallWindowProcA(OldWNDPROC, hWnd, uMsg, wParam, lParam);
		}

		UnitAny* Me = D2Funcs.D2CLIENT_GetPlayer();
		UnitAny* selected_item = (*D2Vars.D2CLIENT_SelectedItem);
		if (Me && selected_item) {
			int loc = ExPrecast::GetItemLocation(selected_item);
			
			//TODO implement item movement for trade window
			if (D2Vars.D2CLIENT_UIModes[UI_TRADE] || loc == STORAGE_TRADE)
				return (LONG)CallWindowProcA(OldWNDPROC, hWnd, uMsg, wParam, lParam);

			Inventory* inv = Me->pInventory;
			int destinvindex = loc == STORAGE_CUBE || loc == STORAGE_STASH ? 2
				: D2Vars.D2CLIENT_UIModes[UI_STASH] ? 6
				: D2Vars.D2CLIENT_UIModes[UI_CUBE] ? 5
				: D2Vars.D2CLIENT_UIModes[UI_NPCSHOP] ? -2
				: -1;
			if (GetAsyncKeyState(VK_MENU) < 0) {//cube to stash or stash to cube, or anywhere to cube
				destinvindex = D2Vars.D2CLIENT_UIModes[UI_STASH] ? 5
					: D2Vars.D2CLIENT_UIModes[UI_CUBE] ? 6
					: -2;
			}
			if (destinvindex == 5) {//dont allow cubeception
				string szTemp = ExPrecast::GetItemCode(selected_item);
				if (!szTemp.empty() && szTemp.compare(0, 3, "box") == 0)
					return (LONG)CallWindowProcA(OldWNDPROC, hWnd, uMsg, wParam, lParam);
			}
			if (destinvindex > -1 && destinvindex < inv->dwStoresCount && GetAsyncKeyState(VK_SHIFT) >= 0) {
				//get destination inv grid
				InventoryStore* is = (InventoryStore*)inv->pStores;
				vector<UnitAny*> ItemsProcessed;
				DWORD* Array = (DWORD*)is[destinvindex].pArray;
				if (is && Array) {
					vector<vector<int>> invgrid(is[destinvindex].Height, vector<int>(is[destinvindex].Width));
					for (int y = 0; y < is[destinvindex].Height; ++y) {
						for (int x = 0; x < is[destinvindex].Width; ++x) {
							UnitAny* item = (UnitAny*)Array[x + y * is[destinvindex].Width];
							if (item) {
								if (find(ItemsProcessed.begin(), ItemsProcessed.end(), item) != ItemsProcessed.end())
									continue;
								ItemsProcessed.push_back(item);
								ItemsTxt* txt = D2Funcs.D2COMMON_GetItemText(item->dwClassId);
								if (txt) {
									for (int h = 0; h < txt->binvheight; h++) {
										for (int w = 0; w < txt->binvwidth; w++) {
											invgrid[y + h][x + w] = 1;
										}
									}
								}
							}
						}
					}
					//move item to open spot if it exists
					ItemsTxt* txt = D2Funcs.D2COMMON_GetItemText(selected_item->dwClassId);
					if (txt) {
						for (UINT y = 0; y < invgrid.size() - txt->binvheight + 1; y++) {
							bool fits = true;
							for (UINT x = 0; x < invgrid[y].size() - txt->binvwidth + 1; x++) {
								fits = true;
								for (int h = 0; h < txt->binvheight; h++) {
									for (int w = 0; w < txt->binvwidth; w++) {
										if (invgrid[y + h][x + w] != 0) {
											fits = false;
											break;
										}
									}
									if (!fits) break;
								}
								if (fits) {
									if (selected_item->pItemData->BodyLoc)
										ExPrecast::RemoveFromBody(selected_item->pItemData->BodyLoc);
									else
										ExPrecast::RemoveFromBuffer(selected_item->dwUnitId);
									//thread this so it doesn't slow down gameplay (sleep)
									auto n = [](DWORD id, int x, int y, int destinvindex) {
										int count = 0;

										BYTE aPacket[17];
										memset(&aPacket, 0, 17);
										aPacket[0] = 0x18;
										*(DWORD*)&aPacket[1] = id;
										*(DWORD*)&aPacket[5] = x;
										*(DWORD*)&aPacket[9] = y;
										*(DWORD*)&aPacket[13] = destinvindex == 2 ? 0 : destinvindex == 6 ? 4 : destinvindex == 5 ? 3 : 0;

										while (count < 100) {
											if (D2Funcs.D2CLIENT_GetCursorItem() && D2Funcs.D2CLIENT_GetCursorItem()->dwUnitId == id) {
												D2Funcs.D2NET_SendPacket(17, 1, aPacket);
												break;
											}
											this_thread::sleep_for(chrono::milliseconds(10));
											count++;
										}
									};
									thread t(n, selected_item->dwUnitId, x, y, destinvindex);
									t.detach();

									break;
								}
							}
							if (fits) break;
						}
					}
				}
			}
			else if (destinvindex == -1 || GetAsyncKeyState(VK_SHIFT) < 0) {
				//drop item on ground
				if (selected_item->pItemData->BodyLoc)
					ExPrecast::RemoveFromBody(selected_item->pItemData->BodyLoc);
				else
					ExPrecast::RemoveFromBuffer(selected_item->dwUnitId);

				//thread this so it doesn't slow down gameplay (sleep)
				auto n = [](DWORD id) {
					int count = 0;
					ExPrecast::DropItem(id);
					while (count < 100) {
						if (D2Funcs.D2CLIENT_GetCursorItem() && D2Funcs.D2CLIENT_GetCursorItem()->dwUnitId == id) {
							break;
						}
						this_thread::sleep_for(chrono::milliseconds(10));
						count++;
					}
				};
				thread t(n, selected_item->dwUnitId);
				t.detach();
			}
			
		}
	}
	//end token code

	return (LONG)CallWindowProcA(OldWNDPROC, hWnd, uMsg, wParam, lParam);
}

void ExInput::Say(const char* Msg, ...) {
	va_list arguments;
	va_start(arguments, Msg);

	int len = _vscprintf(Msg, arguments) + 1;
	char * text = new char[len];
	vsprintf_s(text, len, Msg, arguments);
	va_end(arguments);

	BYTE* aPacket = new BYTE[strlen(text) + 6];
	memset(aPacket, 0, strlen(text) + 6);
	aPacket[0] = 0x15;
	*(WORD*)&aPacket[1] = 0x01;

	memcpy(aPacket + 3, text, strlen(text));
	D2Funcs.D2NET_SendPacket(strlen(text) + 6, 1, aPacket);

	delete[] text;
	delete[] aPacket;
}

WORD ExInput::GetKeyBind(int nEntry, BOOL bPrimary) {
	int i = 0;
	for (KeyBinding* k = D2Vars.D2CLIENT_KeyBindings; i < 114; ++k, ++i) {
		if (k->bPrimary == bPrimary && k->nHotkey == nEntry) return k->wKey;
	}

	return -1;
}


//Import from Scrap
wchar_t* ExInput::GetNameOfKey(WORD vKey) // pure copy&pasta
{
	if (vKey <= 223u) {
		switch (vKey) {
			case 1u:
				return D2Funcs.D2LANG_GetLocaleText(3763);
			case 2u:
				return D2Funcs.D2LANG_GetLocaleText(3764);
			case 3u:
				return D2Funcs.D2LANG_GetLocaleText(3765);
			case 4u:
				return D2Funcs.D2LANG_GetLocaleText(3766);
			case 0x15u:
				return D2Funcs.D2LANG_GetLocaleText(3771);
			case 0x17u:
				return D2Funcs.D2LANG_GetLocaleText(3772);
			case 0x18u:
				return D2Funcs.D2LANG_GetLocaleText(3773);
			case 0x19u:
				return D2Funcs.D2LANG_GetLocaleText(3774);
			case 0x1Bu:
				return D2Funcs.D2LANG_GetLocaleText(3775);
			case 0x1Cu:
				return D2Funcs.D2LANG_GetLocaleText(3776);
			case 0x1Du:
				return D2Funcs.D2LANG_GetLocaleText(3777);
			case 0x1Eu:
				return D2Funcs.D2LANG_GetLocaleText(3778);
			case 0x1Fu:
				return D2Funcs.D2LANG_GetLocaleText(3779);
			case 0x25u:
				return D2Funcs.D2LANG_GetLocaleText(3780);
			case 0x26u:
				return D2Funcs.D2LANG_GetLocaleText(3781);
			case 0x27u:
				return D2Funcs.D2LANG_GetLocaleText(3782);
			case 0x28u:
				return D2Funcs.D2LANG_GetLocaleText(3783);
			case 0x29u:
				return D2Funcs.D2LANG_GetLocaleText(3784);
			case 0x2Bu:
				return D2Funcs.D2LANG_GetLocaleText(3785);
			case 0x5Bu:
				return D2Funcs.D2LANG_GetLocaleText(3786);
			case 0x5Cu:
				return D2Funcs.D2LANG_GetLocaleText(3787);
			case 0x5Du:
				return D2Funcs.D2LANG_GetLocaleText(3788);
			case 0x90u:
				return D2Funcs.D2LANG_GetLocaleText(3789);
			case 8u:
				return D2Funcs.D2LANG_GetLocaleText(3790);
			case 9u:
				return D2Funcs.D2LANG_GetLocaleText(3791);
			case 0xCu:
				return D2Funcs.D2LANG_GetLocaleText(3792);
			case 0xDu:
				return D2Funcs.D2LANG_GetLocaleText(3793);
			case 0x10u:
				return D2Funcs.D2LANG_GetLocaleText(3794);
			case 0x11u:
				return D2Funcs.D2LANG_GetLocaleText(3795);
			case 0x12u:
				return D2Funcs.D2LANG_GetLocaleText(3796);
			case 0x13u:
				return D2Funcs.D2LANG_GetLocaleText(3797);
			case 0x14u:
				return D2Funcs.D2LANG_GetLocaleText(3798);
			case 0x20u:
				return D2Funcs.D2LANG_GetLocaleText(3799);
			case 0x21u:
				return D2Funcs.D2LANG_GetLocaleText(3800);
			case 0x22u:
				return D2Funcs.D2LANG_GetLocaleText(3801);
			case 0x23u:
				return D2Funcs.D2LANG_GetLocaleText(3802);
			case 0x24u:
				return D2Funcs.D2LANG_GetLocaleText(3803);
			case 0x2Au:
				return D2Funcs.D2LANG_GetLocaleText(3804);
			case 0x2Cu:
				return D2Funcs.D2LANG_GetLocaleText(3805);
			case 0x2Du:
				return D2Funcs.D2LANG_GetLocaleText(3806);
			case 0x2Eu:
				return D2Funcs.D2LANG_GetLocaleText(3807);
			case 0x2Fu:
				return D2Funcs.D2LANG_GetLocaleText(3808);
			case 0x60u:
				return D2Funcs.D2LANG_GetLocaleText(3809);
			case 0x61u:
				return D2Funcs.D2LANG_GetLocaleText(3810);
			case 0x62u:
				return D2Funcs.D2LANG_GetLocaleText(3811);
			case 0x63u:
				return D2Funcs.D2LANG_GetLocaleText(3812);
			case 0x64u:
				return D2Funcs.D2LANG_GetLocaleText(3813);
			case 0x65u:
				return D2Funcs.D2LANG_GetLocaleText(3814);
			case 0x66u:
				return D2Funcs.D2LANG_GetLocaleText(3815);
			case 0x67u:
				return D2Funcs.D2LANG_GetLocaleText(3816);
			case 0x68u:
				return D2Funcs.D2LANG_GetLocaleText(3817);
			case 0x69u:
				return D2Funcs.D2LANG_GetLocaleText(3818);
			case 0x6Au:
				return D2Funcs.D2LANG_GetLocaleText(3819);
			case 0x6Bu:
				return D2Funcs.D2LANG_GetLocaleText(3820);
			case 0x6Cu:
				return D2Funcs.D2LANG_GetLocaleText(3821);
			case 0x6Du:
				return D2Funcs.D2LANG_GetLocaleText(3822);
			case 0x6Eu:
				return D2Funcs.D2LANG_GetLocaleText(3823);
			case 0x6Fu:
				return D2Funcs.D2LANG_GetLocaleText(3824);
			case 0x70u:
				return D2Funcs.D2LANG_GetLocaleText(3825);
			case 0x71u:
				return D2Funcs.D2LANG_GetLocaleText(3826);
			case 0x72u:
				return D2Funcs.D2LANG_GetLocaleText(3827);
			case 0x73u:
				return D2Funcs.D2LANG_GetLocaleText(3828);
			case 0x74u:
				return D2Funcs.D2LANG_GetLocaleText(3829);
			case 0x75u:
				return D2Funcs.D2LANG_GetLocaleText(3830);
			case 0x76u:
				return D2Funcs.D2LANG_GetLocaleText(3831);
			case 0x77u:
				return D2Funcs.D2LANG_GetLocaleText(3832);
			case 0x78u:
				return D2Funcs.D2LANG_GetLocaleText(3833);
			case 0x79u:
				return D2Funcs.D2LANG_GetLocaleText(3834);
			case 0x7Au:
				return D2Funcs.D2LANG_GetLocaleText(3835);
			case 0x7Bu:
				return D2Funcs.D2LANG_GetLocaleText(3836);
			case 0x7Cu:
				return D2Funcs.D2LANG_GetLocaleText(3837);
			case 0x7Du:
				return D2Funcs.D2LANG_GetLocaleText(3838);
			case 0x7Eu:
				return D2Funcs.D2LANG_GetLocaleText(3839);
			case 0x7Fu:
				return D2Funcs.D2LANG_GetLocaleText(3840);
			case 0x80u:
				return D2Funcs.D2LANG_GetLocaleText(3841);
			case 0x81u:
				return D2Funcs.D2LANG_GetLocaleText(3842);
			case 0x82u:
				return D2Funcs.D2LANG_GetLocaleText(3843);
			case 0x83u:
				return D2Funcs.D2LANG_GetLocaleText(3844);
			case 0x84u:
				return D2Funcs.D2LANG_GetLocaleText(3845);
			case 0x85u:
				return D2Funcs.D2LANG_GetLocaleText(3846);
			case 0x86u:
				return D2Funcs.D2LANG_GetLocaleText(3847);
			case 0x87u:
				return D2Funcs.D2LANG_GetLocaleText(3848);
			case 0x91u:
				return D2Funcs.D2LANG_GetLocaleText(3849);
			case 0xBAu:
				return D2Funcs.D2LANG_GetLocaleText(3850);
			case 0xBBu:
				return D2Funcs.D2LANG_GetLocaleText(3851);
			case 0xBCu:
				return D2Funcs.D2LANG_GetLocaleText(3852);
			case 0xBDu:
				return D2Funcs.D2LANG_GetLocaleText(3853);
			case 0xBEu:
				return D2Funcs.D2LANG_GetLocaleText(3854);
			case 0xBFu:
				return D2Funcs.D2LANG_GetLocaleText(3855);
			case 0xC0u:
				return D2Funcs.D2LANG_GetLocaleText(3856);
			case 0xDBu:
				return D2Funcs.D2LANG_GetLocaleText(3857);
			case 0xDCu:
				return D2Funcs.D2LANG_GetLocaleText(3858);
			case 0xDDu:
				return D2Funcs.D2LANG_GetLocaleText(3859);
			case 0xDEu:
				return D2Funcs.D2LANG_GetLocaleText(3860);
		}

	}
	else {
		switch (vKey) {
			case 0x100u:
				return D2Funcs.D2LANG_GetLocaleText(3766);
			case 0x101u:
				return D2Funcs.D2LANG_GetLocaleText(3767);
			case 0x102u:
				return D2Funcs.D2LANG_GetLocaleText(3768);
			case 0x103u:
				return D2Funcs.D2LANG_GetLocaleText(3769);
			case 0x104u:
				return D2Funcs.D2LANG_GetLocaleText(3770);
		}
	}
	static wchar_t tmp;
	tmp = (wchar_t)vKey;
	return &tmp;
}