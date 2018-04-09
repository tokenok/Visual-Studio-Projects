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
#include "ExOptions.h"

#include "ExInput.h"
#include "ExParty.h"
#include "ExBuffs.h"
#include "ExScreen.h"
#include "ExOOG.h"
#include "ExMultiRes.h"
#include "ExLagometer.h"

#include "Build.h"

#include  <boost\lexical_cast.hpp>

//---Menu recon stuff
static BOOL MenuValueClicked;
static BOOL isMenuClicked;
static DWORD MenuKeyClicked;


static sMsg D2MenuMsgs[] = {
	{ SMSG_WM_MSG,		WM_LBUTTONDOWN,		ExOptions::m_LBUTTONDOWN },
	{ SMSG_WM_MSG,		WM_LBUTTONUP,		ExOptions::m_LBUTTONUP },
	{ SMSG_WM_MSG,		WM_RBUTTONDOWN,		ExOptions::m_LBUTTONDOWN }, // added
	{ SMSG_WM_MSG,		WM_RBUTTONUP,		ExOptions::m_LBUTTONUP },   // added
	{ SMSG_WM_KEY_DOWN, VK_DOWN,			NULL },
	{ SMSG_WM_KEY_DOWN, VK_UP,				NULL },
	{ SMSG_WM_KEY_DOWN, VK_LEFT,			NULL },
	{ SMSG_WM_KEY_DOWN, VK_RIGHT,			NULL },
	{ SMSG_WM_KEY_DOWN, VK_RETURN,			ExOptions::m_OnEnter},
};

//---Replacement for messages

void ExOptions::FillMissingCallbacks()
{
	D2MenuMsgs[4].fnCallBack = *D2Vars.D2CLIENT_MenuMsgs[2].fnCallBack;
	D2MenuMsgs[5].fnCallBack = *D2Vars.D2CLIENT_MenuMsgs[3].fnCallBack;
	D2MenuMsgs[6].fnCallBack = *D2Vars.D2CLIENT_MenuMsgs[4].fnCallBack;
	D2MenuMsgs[7].fnCallBack = *D2Vars.D2CLIENT_MenuMsgs[5].fnCallBack;

}

void ExOptions::RegisterMenuMsgs()
{
	ExInput::RegisterMsgs(D2MenuMsgs, sizeof(D2MenuMsgs) / sizeof(sMsg));
}

void ExOptions::UnregisterMenuMsgs()
{
	ExInput::UnregisterMsgs(D2MenuMsgs, sizeof(D2MenuMsgs) / sizeof(sMsg));
}


//---Menu recon stuff

void ExOptions::ShowHide()
{
	static bool wasAutoMap;
	if (D2Vars.D2CLIENT_UIModes[UI_MAINMENU]) 
	{
		D2Vars.D2CLIENT_UIModes[UI_MAINMENU] = 0;
		ExOptions::UnregisterMenuMsgs();

		ExInput::DefineBindings();
		*D2Vars.D2CLIENT_UI_Unk2 = 0;
		if (wasAutoMap)
			D2Vars.D2CLIENT_UIModes[UI_AUTOMAP] = 1;
	}
	else
	{
		if (ExParty::isOpen()) 
		{ 
			ExParty::ShowHide(); 
			return;
		}

		if (!D2Funcs.D2CLIENT_ClearScreen3(0, 1)) 
		{
			if (D2Vars.D2CLIENT_UIModes[UI_AUTOMAP]) 
				wasAutoMap = true;
			else
				wasAutoMap = false;
			if (D2Vars.D2CLIENT_UIModes[UI_NPCMENU])
				return;
			D2Funcs.D2CLIENT_ClearScreen();
			if (D2Funcs.D2CLIENT_GetPlayer()->dwMode == PLAYER_MODE_DEAD || D2Funcs.D2CLIENT_GetPlayer()->dwMode == PLAYER_MODE_DEATH)
				return;
			D2Vars.D2CLIENT_UIModes[UI_MAINMENU] = 1;
			ExInput::UndefineBindings();
			ExOptions::RegisterMenuMsgs();
			*D2Vars.D2CLIENT_UI_Unk2 = 1;
			*D2Vars.D2CLIENT_UI_Unk1 = 0;

			ExOptions::MainMenu(0, 0);
		}
	}
}

void ExOptions::OnClick(StormMsg * Msg)
{
	int nMenu = *D2Vars.D2CLIENT_SelectedMenu;
	D2MenuEntry * Entries = *D2Vars.D2CLIENT_D2MenuEntries;

	if (Entries[nMenu].EnableCheck)
		if (!Entries[nMenu].EnableCheck(&Entries[nMenu], nMenu)) 
			return;

	if (Msg->Msg == WM_RBUTTONUP && (Entries[nMenu].dwMenuType != D2MENU_SWITCHEX && Entries[nMenu].dwMenuType != D2MENU_SWITCH))
		return;
	if (Msg->wParam & MK_MBUTTON)
		return;

	switch (Entries[nMenu].dwMenuType) {
	case D2MENU_SWITCHEX:
	case D2MENU_SWITCH:
	{
		if (Msg->Msg == WM_RBUTTONUP)
		Entries[nMenu].dwCurrentSwitch--;
		else
		Entries[nMenu].dwCurrentSwitch++;
		if (Entries[nMenu].dwCurrentSwitch > Entries[nMenu].dwSwitchesNo - 1)
			Entries[nMenu].dwCurrentSwitch = 0;
		if (Entries[nMenu].dwCurrentSwitch < 0)
			Entries[nMenu].dwCurrentSwitch = Entries[nMenu].dwSwitchesNo;

		if (Entries[nMenu].OnPress)
			Entries[nMenu].OnPress(&Entries[nMenu], Msg);
		D2ASMFuncs::D2CLIENT_PlaySound(1);
	}
		break;
	case D2MENU_NORMAL:
	{
		if (Entries[nMenu].OnPress)
			Entries[nMenu].OnPress(&Entries[nMenu], Msg);
		D2ASMFuncs::D2CLIENT_PlaySound(2);
	}
		break;
	case D2MENU_KEY:
	{
		if (MenuKeyClicked) 
		{
			D2Funcs.STORM_UnregisterMsg(D2Funcs.D2GFX_GetHwnd(), WM_KEYDOWN, &m_OnGetKey);
			MenuKeyClicked = 0;
		}
		else	
		{
			D2Funcs.STORM_RegisterMsg(D2Funcs.D2GFX_GetHwnd(), WM_KEYDOWN, &m_OnGetKey);
			MenuKeyClicked = nMenu;
		}
	}
		break;
	}
}

void __stdcall ExOptions::m_OnGetKey(StormMsg * Msg)
{
	if (MenuKeyClicked && Msg->wParam != VK_ESCAPE)
	{
		D2MenuEntry * Entries = *D2Vars.D2CLIENT_D2MenuEntries;
		//	ExInput::UndefineBindings();
		*Entries[MenuKeyClicked].Bind = Msg->wParam;
		//		ExInput::DefineBindings();
	}
	MenuKeyClicked = 0;
	Msg->_2 = 1;
	Msg->_3 = 0;
	D2Funcs.STORM_ResetMsgQuene(Msg);
	D2Funcs.STORM_UnregisterMsg(D2Funcs.D2GFX_GetHwnd(), WM_KEYDOWN, &m_OnGetKey);
}

void __stdcall ExOptions::m_LBUTTONDOWN(StormMsg * Msg)
{
	WORD mX = LOWORD(Msg->lParam);
	WORD mY = HIWORD(Msg->lParam);
	int nMenu = D2ASMFuncs::D2CLIENT_GetSelectedMenu(mY);
	if (nMenu != -1)
		*D2Vars.D2CLIENT_SelectedMenu = nMenu;
	MenuUpdate(mX, mY);
	isMenuClicked = 1;
	Msg->_2 = 1;
	Msg->_3 = 0;
	D2Funcs.STORM_ResetMsgQuene(Msg);
}

void __stdcall  ExOptions::m_LBUTTONUP(StormMsg * Msg)
{
	WORD mY = HIWORD(Msg->lParam);
	if (isMenuClicked && D2ASMFuncs::D2CLIENT_GetSelectedMenu(mY) == *D2Vars.D2CLIENT_SelectedMenu) OnClick(Msg);
	//CBA to check what exacly those vars are, but they were in the original code...
	*D2Vars.D2CLIENT_UI_Unk1 = 0;
	*D2Vars.D2CLIENT_UI_Unk2 = 0;
	*D2Vars.D2CLIENT_UiUnk1 = 0;
	*D2Vars.D2CLIENT_UiUnk2 = 0;
	*D2Vars.D2CLIENT_UiUnk3 = 0;
	*D2Vars.D2CLIENT_UiUnk4 = 0;
	*D2Vars.D2CLIENT_UI_Unk7 = 16;
	*D2Vars.D2CLIENT_UI_Unk8 = 16;

	isMenuClicked = 0;
	MenuValueClicked = 0;
	Msg->_2 = 1;
	Msg->_3 = 0;
	D2Funcs.STORM_ResetMsgQuene(Msg);
}

void __stdcall  ExOptions::m_OnEnter(StormMsg * Msg) // 28.12 -> resolved
{
	OnClick(Msg);
	Msg->_2 = 1;
	Msg->_3 = 0;
	D2Funcs.STORM_ResetMsgQuene(Msg);
}

void ExOptions::MenuUpdate(int mX, int mY)
{
	int nMenu = *D2Vars.D2CLIENT_SelectedMenu;
	if (nMenu != D2ASMFuncs::D2CLIENT_GetSelectedMenu(mY) && !MenuValueClicked) return;

	D2MenuEntry * Entries = *D2Vars.D2CLIENT_D2MenuEntries;

	int nValue = Entries[nMenu].dwCurrentValue;

	if (Entries[nMenu].EnableCheck)
		if (!Entries[nMenu].EnableCheck(&Entries[nMenu], nMenu)) return;

	switch (Entries[nMenu].dwMenuType) {
	case D2MENU_BAR:
	{
		int CheckX, CheckX2;
		if (!Entries[nMenu].ptCellFile && !Entries[nMenu].wItemName[0]) {
			CheckX = (*D2Vars.D2CLIENT_ScreenWidth / 2) - 144;
			CheckX2 = (*D2Vars.D2CLIENT_ScreenWidth / 2) - 145;
		}
		else if (Entries[nMenu].ptCellFile)
		{
			CheckX = (*D2Vars.D2CLIENT_ScreenWidth / 2) - 59;
			CheckX2 = (*D2Vars.D2CLIENT_ScreenWidth / 2) - 60;
		}
		else
		{
			CheckX = (*D2Vars.D2CLIENT_ScreenWidth / 2) - 59;
			CheckX2 = (*D2Vars.D2CLIENT_ScreenWidth / 2) - 60;
		}
		if (MenuValueClicked || mX > CheckX && mX < CheckX + 289) {
			if (mX >= CheckX2 + 12)	{
				if (mX <= CheckX2 + 277) {
					Entries[nMenu].dwCurrentValue = int(((mX - (CheckX + 12)) / (132.5 / (Entries[nMenu].dwMaxValue - 1)) + 1) / 2);
				}
				else
					Entries[nMenu].dwCurrentValue = Entries[nMenu].dwMaxValue - 1;
			}
			else
				Entries[nMenu].dwCurrentValue = 0;
			MenuValueClicked = 1;
		}
	}
	}
	if (nValue != Entries[nMenu].dwCurrentValue) {
		if (Entries[nMenu].OnPress)
			Entries[nMenu].OnPress(&Entries[nMenu], 0);
		D2ASMFuncs::D2CLIENT_PlaySound(1);
	}
}

void  ExOptions::DrawMenuBar(int BarPosX, int BarPosY, D2Menu * Menu, D2MenuEntry *Entry, int nTransLvl)
{
	D2Funcs.D2GFX_DrawRectangle(BarPosX, BarPosY, BarPosX + 144, BarPosY + 30, 0, (Entry->dwBarType == 0) + 1);
	CellFile * cfBar = Entry->dwBarType ? *D2Vars.D2CLIENT_MenuBar2 : *D2Vars.D2CLIENT_MenuBar2;
	D2Funcs.D2WIN_DrawCellFile(cfBar, 400 + 230, BarPosY, 2, nTransLvl, -1);
	D2Funcs.D2WIN_DrawCellFile(*D2Vars.D2CLIENT_MenuBarSlider, 400 + 250, BarPosY, 2, nTransLvl, -1);
}

void ExOptions::DrawMenuRecon()
{
	ASSERT(*D2Vars.D2CLIENT_D2Menu);
	ASSERT(*D2Vars.D2CLIENT_D2MenuEntries);

	D2Menu * Menu = *D2Vars.D2CLIENT_D2Menu;
	D2MenuEntry * Entries = *D2Vars.D2CLIENT_D2MenuEntries;

	static CellContext Pent = { 0 };
	Pent.pCellFile = *D2Vars.D2CLIENT_MenuPent;
	Pent.nCellNo = 0;

	static DWORD LastTick = 0;
	static DWORD PentFrame = 0;
	D2Funcs.D2WIN_SetTextSize(6);
	ExScreen::DrawTextEx(2, 10, 5, 0, 5, "D2Ex2/Build %s%d.%s.%s.%s,",
#if !defined D2EX_CLOSED_BNET && !defined D2EX_SCRAP_HACK && !defined D2EX_MULTIRES && !defined D2EX_ARGOLD && \
	!defined D2EX_PVPGN_EXT && !defined D2EX_EXAIM_ENABLED && !defined D2EX_PVM_BUILD
		"GENERIC|"
#endif 
#ifdef D2EX_CLOSED_BNET
		D2EX_CLOSED_BNET
#endif
#ifdef D2EX_SCRAP_HACKS
		D2EX_SCRAP_HACKS
#endif
#ifdef D2EX_MULTIRES
		D2EX_MULTIRES
#endif
#ifdef D2EX_ARGOLD
		D2EX_ARGOLD
#endif
#ifdef D2EX_PVPGN_EXT
		D2EX_PVPGN_EXT
#endif
#ifdef D2EX_EXAIM_ENABLED
		D2EX_EXAIM_ENABLED
#endif
#ifdef D2EX_PVM_BUILD
		D2EX_PVM_BUILD
#endif
		, __BUILDNO__, __DATE__, __TIME__,
#ifdef _DEBUG
		"Nightly"
#else
		"Annihilus Release"
#endif
		);

	int MenuStartY = (*D2Vars.D2CLIENT_ScreenHeight - 80) / 2 - (Menu->dwEntriesNo * Menu->dwInterline) / 2;

	if (*D2Vars.D2CLIENT_PrevMouseY != *D2Vars.D2CLIENT_MouseY || *D2Vars.D2CLIENT_PrevMouseX != *D2Vars.D2CLIENT_MouseX)
	{
		*D2Vars.D2CLIENT_PrevMouseY = *D2Vars.D2CLIENT_MouseY;
		*D2Vars.D2CLIENT_PrevMouseX = *D2Vars.D2CLIENT_MouseX;
		if (isMenuClicked)
			MenuUpdate(*D2Vars.D2CLIENT_MouseX, *D2Vars.D2CLIENT_MouseY);
		//	D2Funcs.D2CLIENT_D2MenuChange(*D2Vars.D2CLIENT_MouseY,0,*D2Vars.D2CLIENT_MouseX);
		else
		{
			int SelMenu = D2ASMFuncs::D2CLIENT_GetSelectedMenu(*D2Vars.D2CLIENT_MouseY);
			if (SelMenu != -1) *D2Vars.D2CLIENT_SelectedMenu = SelMenu;
		}
	}

	for (int eNo = 0; (int)Menu->dwEntriesNo > eNo; eNo++)
	{
		Entries[eNo].dwYOffset = 0;
		int dwTrans = 0;
		if (!Entries[eNo].dwExpansion || D2Funcs.FOG_isExpansion())
		{
			if (!Entries[eNo].EnableCheck || (dwTrans = Entries[eNo].EnableCheck(&Entries[eNo], eNo)) != 0) dwTrans = 1;
			dwTrans = (4 * dwTrans) + 1;
			if (Entries[eNo].ValidateCheck) Entries[eNo].ValidateCheck(&Entries[eNo]);
			int MenuPosY = MenuStartY + eNo * Menu->dwInterline;
			int MenuTextY = MenuPosY + Menu->dwTextHeight;
			Entries[eNo].dwYOffset = MenuPosY;
			switch (Entries[eNo].dwMenuType)
			{
			case D2MENU_STATIC: // Static entry
			case D2MENU_NORMAL: // Normal entry
				if (Entries[eNo].ptCellFile)
					D2Funcs.D2WIN_DrawCellFile(Entries[eNo].ptCellFile, *D2Vars.D2CLIENT_ScreenWidth / 2, MenuTextY, 1, dwTrans, -1);
				else if (Entries[eNo].wItemName[0])
				{
					D2Funcs.D2WIN_SetTextSize(Entries[eNo].dwFontType ? Entries[eNo].dwFontType : 3);
					int Tw = D2Funcs.D2WIN_GetTextWidth(Entries[eNo].wItemName);
					D2Funcs.D2WIN_DrawTextEx(Entries[eNo].wItemName, (*D2Vars.D2CLIENT_ScreenWidth - Tw) / 2, MenuTextY, Entries[eNo].dwColor, 0, dwTrans);
				}
				break;
			case D2MENU_SWITCH: // Switch Entry
				if (Entries[eNo].ptCellFile)
					D2Funcs.D2WIN_DrawCellFile(Entries[eNo].ptCellFile, *D2Vars.D2CLIENT_ScreenWidth / 2 - 230, MenuTextY, 0, dwTrans, -1);
				else if (Entries[eNo].wItemName[0])
				{
					D2Funcs.D2WIN_SetTextSize(2);
					int Tw = D2Funcs.D2WIN_GetTextWidth(Entries[eNo].wItemName);
					D2Funcs.D2WIN_DrawTextEx(Entries[eNo].wItemName, (*D2Vars.D2CLIENT_ScreenWidth) / 2 - 250, MenuTextY, 0, 0, dwTrans);
				}
				if (Entries[eNo].ptSwitchCellFile[Entries[eNo].dwCurrentValue])
					D2Funcs.D2WIN_DrawCellFile(Entries[eNo].ptSwitchCellFile[Entries[eNo].dwCurrentValue], *D2Vars.D2CLIENT_ScreenWidth / 2 + 230, MenuTextY, 2, dwTrans, -1);
				else if (Entries[eNo].wItemName[0])
				{
					D2Funcs.D2WIN_SetTextSize(Entries[eNo].dwFontType ? Entries[eNo].dwFontType : 2);
					int Tw = D2Funcs.D2WIN_GetTextWidth(Entries[eNo].wSwitchItemName[Entries[eNo].dwCurrentValue]);
					D2Funcs.D2WIN_DrawTextEx(Entries[eNo].wSwitchItemName[Entries[eNo].dwCurrentValue], (*D2Vars.D2CLIENT_ScreenWidth - Tw) / 2 + 230, MenuTextY, 0, 0, dwTrans);
				}
				break;
			case D2MENU_BAR: // Bar Entry
				if (Entries[eNo].ptCellFile)
				{
					D2Funcs.D2WIN_DrawCellFile(Entries[eNo].ptCellFile, *D2Vars.D2CLIENT_ScreenWidth / 2 - 230, MenuTextY, 0, dwTrans, -1);
					D2Funcs.D2CLIENT_D2DrawBar(MenuPosY, 0, &Entries[eNo], dwTrans, Entries[eNo].ptCellFile == 0);
				}
				else if (Entries[eNo].wItemName[0])
				{
					D2Funcs.D2WIN_SetTextSize(Entries[eNo].dwFontType ? Entries[eNo].dwFontType : 7);
					int Tw = D2Funcs.D2WIN_GetTextWidth(Entries[eNo].wItemName);
					D2Funcs.D2WIN_DrawTextEx(Entries[eNo].wItemName, (*D2Vars.D2CLIENT_ScreenWidth - Tw) / 2 - 230, MenuTextY, 7, 0, dwTrans);
					D2Funcs.D2CLIENT_D2DrawBar(MenuPosY + 7, 0, &Entries[eNo], dwTrans, 0);
				}
				break;
			case D2MENU_KEY: // Key Config
				if (Entries[eNo].wItemName[0])
				{
					D2Funcs.D2WIN_SetTextSize(Entries[eNo].dwFontType ? Entries[eNo].dwFontType : 8);
					D2Funcs.D2WIN_DrawTextEx(Entries[eNo].wItemName, 130, MenuTextY, COL_ORANGE, 0, dwTrans);
					if (Entries[eNo].Bind) {
						wchar_t* szKey = ExInput::GetNameOfKey((WORD)*Entries[eNo].Bind);
						int Tw2 = D2Funcs.D2WIN_GetTextWidth(szKey);
						D2Funcs.D2WIN_DrawTextEx(szKey, (*D2Vars.D2CLIENT_ScreenWidth - Tw2) / 2 + 230, MenuTextY, MenuKeyClicked == eNo ? COL_RED : COL_WHITE, 0, dwTrans);
					}
				}
				break;
			case D2MENU_SWITCHEX: // Switch++ Entry
				D2Funcs.D2WIN_SetTextSize(Entries[eNo].dwFontType ? Entries[eNo].dwFontType : 2);
				if (Entries[eNo].wItemName[0])
				{
					int Tw = D2Funcs.D2WIN_GetTextWidth(Entries[eNo].wItemName);
					D2Funcs.D2WIN_DrawTextEx(Entries[eNo].wItemName, (*D2Vars.D2CLIENT_ScreenWidth) / 2 - 250, MenuTextY, 0, 0, dwTrans);

					Tw = D2Funcs.D2WIN_GetTextWidth(Entries[eNo].wSwitches[Entries[eNo].dwCurrentValue]->c_str());
					D2Funcs.D2WIN_DrawTextEx(Entries[eNo].wSwitches[Entries[eNo].dwCurrentValue]->c_str(), (*D2Vars.D2CLIENT_ScreenWidth - Tw) / 2 + 230, MenuTextY, 0, 0, dwTrans);
				}
				break;
			}
		}
	}
	if (PentFrame == 0) Pent.nCellNo = 0; //*D2Vars.D2CLIENT_WidestMenu
	else Pent.nCellNo = 8 - PentFrame;
	int PentY = Menu->dwMenuOffset + Entries[*D2Vars.D2CLIENT_SelectedMenu].dwYOffset;
	D2Funcs.D2GFX_DrawCellContext(&Pent, (int)(*D2Vars.D2CLIENT_ScreenWidth * 0.9), PentY, -1, 5, 0);
	Pent.nCellNo = PentFrame;
	D2Funcs.D2GFX_DrawCellContext(&Pent, (int)(*D2Vars.D2CLIENT_ScreenWidth * 0.02), PentY, -1, 5, 0);

	int Tick = GetTickCount();
	if (Tick - LastTick > 50)
	{
		LastTick = Tick;
		PentFrame = (PentFrame + 1) % 8;
	}

}

BOOL __fastcall ExOptions::GiveUpCheck(D2MenuEntry* ptEntry, DWORD ItemNo)
{
	Room1* aRoom = D2Funcs.D2COMMON_GetUnitRoom(D2Funcs.D2CLIENT_GetPlayer());
	int aLvl = D2Funcs.D2COMMON_GetLevelNoByRoom(aRoom);
	int aAct = D2Funcs.D2COMMON_GetActNoByLevelNo(aLvl);
	if (aLvl != D2Funcs.D2COMMON_GetTownLevel(aAct))
		return true;
	return false;
}

BOOL __fastcall ExOptions::GiveUpCB(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	const char szBuffer[] = "#gu";
	BYTE * aPacket = new BYTE[strlen(szBuffer) + 6];
	memset(aPacket, 0, strlen(szBuffer) + 6);
	aPacket[0] = 0x15;
	*(WORD*)&aPacket[1] = 1;
	memcpy(aPacket + 3, szBuffer, strlen(szBuffer));
	D2Funcs.D2NET_SendPacket(strlen(szBuffer) + 6, 1, aPacket);
	delete[] aPacket;
	D2Funcs.D2CLIENT_ClearScreen();
	return true;

}

BOOL __fastcall ExOptions::RejoinCB(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	CreateThread(0, 0, &ExOOG::Rejoin, "", 0, 0);
	D2Funcs.D2CLIENT_ClearScreen();
	return true;
}


BOOL __fastcall ExOptions::ValidateCheck(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
//	wstring tmp = L"You set " + boost::lexical_cast<wstring>(ptEntry->dwCurrentValue);
//	D2Funcs.D2CLIENT_PrintPartyString(tmp.c_str(), COL_WHITE);
	wcscpy_s((wchar_t*)ptEntry->szCellFile, 130, boost::lexical_cast<wstring>(ptEntry->dwCurrentValue).c_str());
	switch (*D2Vars.D2CLIENT_SelectedMenu)
	{
	case 0:
		BCLvl = ptEntry->dwCurrentValue;
		WritePrivateProfileString("D2Ex", "BCLvl", boost::lexical_cast<string>(ptEntry->dwCurrentValue).c_str(), ConfigIni.c_str());
		break;
	case 1:
		AmpLvl = ptEntry->dwCurrentValue;
		WritePrivateProfileString("D2Ex", "AmpLvl", boost::lexical_cast<string>(ptEntry->dwCurrentValue).c_str(), ConfigIni.c_str());
		break;
	case 2:
		SMLvl = ptEntry->dwCurrentValue;
		WritePrivateProfileString("D2Ex", "SMLvl", boost::lexical_cast<string>(ptEntry->dwCurrentValue).c_str(), ConfigIni.c_str());
		break;
	}
	return false;
}


/*
BOOL __fastcall ExOptions::D2ExColorLab(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	//wstring tmp = L"You set " + boost::lexical_cast<wstring>(ptEntry->dwCurrentValue);
	//D2Funcs.D2CLIENT_PrintPartyString(tmp.c_str(), COL_DARK_GOLD);
	wcscpy_s((wchar_t*)ptEntry->szCellFile, 130, boost::lexical_cast<wstring>(ptEntry->dwCurrentValue).c_str());
	switch (*D2Vars.D2CLIENT_SelectedMenu)
	{
	case 5:
		gTestColorRed = (BYTE)ptEntry->dwCurrentValue;
		break;
	case 6:
		gTestColorGreen = (BYTE)ptEntry->dwCurrentValue;
		break;
	case 7:
		gTestColorBlue = (BYTE)ptEntry->dwCurrentValue;
		break;
	}
	return false;
}
*/


BOOL __fastcall ExOptions::BuffECheck(D2MenuEntry* ptEntry, DWORD ItemNo)
{
	int SkillId = 1;
	switch (ItemNo)
	{
	case 0:
		SkillId = 155;
		break;
	case 1:
		SkillId = 66;
		break;
	case 2:
		SkillId = 17;
		break;
	}
	if (!isMenuClicked && *D2Vars.D2CLIENT_SelectedMenu == ItemNo)
	{
		wcscpy_s((wchar_t*)ptEntry->szCellFile, 130, ExBuffs::GetSkillName(SkillId));
	}

	return true;
}

BOOL __fastcall ExOptions::Buffs(D2MenuEntry* ptEntry, StormMsg *pMsg)
{
	static D2Menu NewMenu;
	static D2MenuEntry NewEntries[5];

	if (!NewMenu.dwEntriesNo)
	{
		NewMenu.dwEntriesNo = 5;
		NewMenu.dwInterline = 45;
		NewMenu.dwTextHeight = 35;
		NewMenu.dwMenuOffset = 51;
		NewMenu.dwBarHeight = 36;
	}

	wcscpy_s(NewEntries[0].wItemName, 130, ExBuffs::GetSkillName(155));
	wcscpy_s(NewEntries[1].wItemName, 130, ExBuffs::GetSkillName(66));
	wcscpy_s(NewEntries[2].wItemName, 130, ExBuffs::GetSkillName(17));
	wcscpy_s((wchar_t*)&NewEntries[3].szCellFile, 130, gLocaleId == LOCALE_POL ? L"W�ACZ EFEKTY" : L"ENABLE BUFF DISPLAY");
	wcscpy_s((wchar_t*)&NewEntries[3].szSwitchCellFiles[0], 130, gLocaleId == LOCALE_POL ? L"WY�." : L"OFF");
	wcscpy_s((wchar_t*)&NewEntries[3].szSwitchCellFiles[1], 130, gLocaleId == LOCALE_POL ? L"W�." : L"ON");
	wcscpy_s((wchar_t*)&NewEntries[4].szCellFile, 130, D2Funcs.D2LANG_GetLocaleText(3409));
	/*
	wcscpy_s((wchar_t*)&NewEntries[5].szCellFile, 130, L"Red");
	wcscpy_s((wchar_t*)&NewEntries[6].szCellFile, 130, L"Green");
	wcscpy_s((wchar_t*)&NewEntries[7].szCellFile, 130, L"Blue");

	NewEntries[5].OnPress = NewEntries[6].OnPress = NewEntries[7].OnPress = &ExOptions::D2ExColorLab;
	NewEntries[5].dwCurrentSwitch = gTestColorRed;
	NewEntries[6].dwCurrentSwitch = gTestColorGreen;
	NewEntries[7].dwCurrentSwitch = gTestColorBlue;
	NewEntries[5].dwMenuType = NewEntries[6].dwMenuType = NewEntries[7].dwMenuType = D2MENU_BAR;
	NewEntries[5].dwMaxValue = NewEntries[6].dwMaxValue = NewEntries[7].dwMaxValue = 255;
	*/
	NewEntries[4].OnPress = &ExOptions::D2ExOpts;
	NewEntries[3].OnPress = &ExOptions::BuffsOpt;
	NewEntries[3].dwMenuType = D2MENU_SWITCH;
	NewEntries[3].dwSwitchesNo = 2;
	NewEntries[3].dwCurrentSwitch = BuffsEnabled;


	NewEntries[0].EnableCheck = NewEntries[1].EnableCheck = NewEntries[2].EnableCheck = &ExOptions::BuffECheck;
	NewEntries[0].dwMenuType = NewEntries[1].dwMenuType = NewEntries[2].dwMenuType = D2MENU_BAR;
	NewEntries[0].OnPress = NewEntries[1].OnPress = NewEntries[2].OnPress = &ExOptions::ValidateCheck;

	NewEntries[0].dwCurrentValue = BCLvl;
	NewEntries[0].dwMaxValue = 55;
	NewEntries[1].dwCurrentValue = AmpLvl;
	NewEntries[1].dwMaxValue = 55;
	NewEntries[2].dwCurrentValue = SMLvl;
	NewEntries[2].dwMaxValue = 55;


	*D2Vars.D2CLIENT_SelectedMenu = 0;
	*D2Vars.D2CLIENT_D2Menu = &NewMenu;
	*D2Vars.D2CLIENT_D2MenuEntries = &NewEntries[0];
	return true;
}

BOOL __fastcall ExOptions::BuffsOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	BuffsEnabled = ptEntry->dwCurrentSwitch;
	WritePrivateProfileString("D2Ex", "BuffsEnabled", boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(), ConfigIni.c_str());
	if (!BuffsEnabled)
		ExBuffs::Clear();
	return TRUE;
}

BOOL __fastcall ExOptions::AutoMapOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	BlobType = ptEntry->dwCurrentSwitch;
	WritePrivateProfileString("D2Ex", "BlobType", boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(), ConfigIni.c_str());

	return TRUE;
}


BOOL __fastcall ExOptions::PermLifeOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	PermShowLife = ptEntry->dwCurrentSwitch;
	WritePrivateProfileString("D2Ex", "PermShowLife", boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(), ConfigIni.c_str());

	return TRUE;
}

BOOL __fastcall ExOptions::PermManaOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	PermShowMana = ptEntry->dwCurrentSwitch;
	WritePrivateProfileString("D2Ex", "PermShowMana", boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(), ConfigIni.c_str());

	return TRUE;
}

BOOL __fastcall ExOptions::VisOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
/*FullVisibility = ptEntry->dwCurrentSwitch;
WritePrivateProfileString("D2Ex", "FullVisibility", boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(), ConfigIni.c_str());
  - Disabled for PvM - Vav*/
return TRUE;
}

BOOL __fastcall ExOptions::AutoOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	AutoShowMap = ptEntry->dwCurrentSwitch;
	WritePrivateProfileString("D2Ex", "AutoShowMap", boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(), ConfigIni.c_str());

	return TRUE;
}

BOOL __fastcall ExOptions::MapOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	Maphack = ptEntry->dwCurrentSwitch;
	WritePrivateProfileString("D2Ex", "Maphack", boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(), ConfigIni.c_str());

	return TRUE;
}

BOOL __fastcall ExOptions::LagOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	bLagometer = ptEntry->dwCurrentSwitch;
	if (bLagometer) {
		if (lagometer == exnull_t)
			lagometer = gExGUI->add(new ExLagometer(*D2Vars.D2GFX_GfxMode > 2 ? (*D2Vars.D2CLIENT_ScreenWidth / 2 - 127) : 273, *D2Vars.D2CLIENT_ScreenHeight - 29));
	}
	else {
		gExGUI->remove(lagometer);
		lagometer = exnull_t;
	}
	WritePrivateProfileString("D2Ex", "Lagometer", boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(), ConfigIni.c_str());

	return TRUE;
}

BOOL __fastcall ExOptions::GoldOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	HideGold = ptEntry->dwCurrentSwitch;
	WritePrivateProfileString("D2Ex", "HideGold", boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(), ConfigIni.c_str());

	return TRUE;
}

BOOL __fastcall ExOptions::CrapOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	HideCrap = ptEntry->dwCurrentSwitch;
	WritePrivateProfileString("D2Ex", "HideCrap", boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(), ConfigIni.c_str());

	return TRUE;
}

//FSOpt added by token
BOOL __fastcall ExOptions::FSOpt(D2MenuEntry* ptEntry, StormMsg* pMsg) {
	FullScreen = ptEntry->dwCurrentSwitch;

	char buff[2];
	_itoa_s(FullScreen, buff, 10);

	Misc::RegWriteString("SOFTWARE\\Blizzard Entertainment\\Diablo II", "FullScreen", buff);
	ExScreen::ToggleFullScreen(FullScreen);

	return TRUE;
}

BOOL __fastcall ExOptions::EnteringOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{

	ptEntry->dwBarType = ptEntry->dwCurrentValue;
	EnteringFont = ptEntry->dwCurrentValue;
	WritePrivateProfileString("D2Ex", "EnteringFont", boost::lexical_cast<string>(ptEntry->dwCurrentValue).c_str(), ConfigIni.c_str());
	return false;
}

BOOL __fastcall ExOptions::ResolutionOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
#ifdef D2EX_MULTIRES
	if (!DisableMultiRes) {
		unsigned int x, y;
		unsigned int r = ptEntry->dwCurrentSwitch;
		if (r == 1) {
			r++;
			ptEntry->dwCurrentSwitch = r;
		}
		if (r > ExMultiRes::lResModes.size() + 3)
			r = 0;

		ExMultiRes::D2GFX_GetModeParams(r, &x, &y);
		DEBUGMSG("Changing resolution to %dx%d", x, y);
		ExMultiRes::D2CLIENT_SetResolution(r);
		Misc::RegWriteDword("SOFTWARE\\Blizzard Entertainment\\Diablo II", "ExResolution", r);
	}
#endif

	return true;
}

/*
BOOL __fastcall ExOptions::ResolutionChangedOpt(D2MenuEntry* ptEntry)
{
#ifdef D2EX_MULTIRES
	unsigned int x, y;
	int r = ptEntry->dwCurrentSwitch + 1;
	if (r == 1) r++;
	if (r > ExMultiRes::lResModes.size() + 3)
		r = 0;
	ptEntry->dwCurrentSwitch = r;
#endif

	return true;
}
*/

BOOL __fastcall ExOptions::EntECheck(D2MenuEntry* ptEntry, DWORD ItemNo)
{
	if (!*D2Vars.D2CLIENT_isMenuClick && *D2Vars.D2CLIENT_SelectedMenu == ItemNo)
		ptEntry->dwBarType = 0;

	return true;
}

BOOL __fastcall ExOptions::Disable(D2MenuEntry* ptEntry, DWORD ItemNo)
{
	return false;
}

BOOL __fastcall ExOptions::Various(D2MenuEntry* ptEntry, StormMsg* pMsg) {
	static D2Menu NewMenu;
	static D2MenuEntry NewEntries[11];

	if (!NewMenu.dwEntriesNo) {
		NewMenu.dwEntriesNo = 11;
		NewMenu.dwInterline = 40;
		NewMenu.dwTextHeight = 35;
		NewMenu.dwMenuOffset = 46;
		NewMenu.dwBarHeight = 32;

		NewEntries[0].dwMenuType = D2MENU_SWITCH;
		NewEntries[0].dwSwitchesNo = 3;
		NewEntries[0].OnPress = &ExOptions::AutoMapOpt;
		NewEntries[0].dwCurrentSwitch = BlobType;

		NewEntries[1].dwMenuType = D2MENU_BAR;
		NewEntries[1].dwMaxValue = 13;
		NewEntries[1].dwCurrentValue = EnteringFont;
		NewEntries[1].OnPress = &ExOptions::EnteringOpt;
		NewEntries[1].EnableCheck = &ExOptions::EntECheck;

		NewEntries[2].dwMenuType = NewEntries[3].dwMenuType = NewEntries[4].dwMenuType = NewEntries[5].dwMenuType = NewEntries[6].dwMenuType = NewEntries[7].dwMenuType = NewEntries[8].dwMenuType = NewEntries[9].dwMenuType = D2MENU_SWITCH;
		NewEntries[2].dwSwitchesNo = NewEntries[3].dwSwitchesNo = NewEntries[4].dwSwitchesNo = NewEntries[5].dwSwitchesNo = NewEntries[6].dwSwitchesNo = NewEntries[7].dwSwitchesNo = NewEntries[8].dwSwitchesNo = NewEntries[9].dwSwitchesNo = 2;

		NewEntries[2].dwCurrentSwitch = PermShowLife;
		NewEntries[2].OnPress = &ExOptions::PermLifeOpt;

		NewEntries[3].dwCurrentSwitch = PermShowMana;
		NewEntries[3].OnPress = &ExOptions::PermManaOpt;
#ifdef VER_113D
		NewEntries[2].EnableCheck = &ExOptions::Disable;
		NewEntries[3].EnableCheck = &ExOptions::Disable;
#endif
		//NewEntries[4].dwCurrentSwitch = FullVisibility; - disabled for PvM - Vav
		//NewEntries[4].OnPress = &ExOptions::VisOpt; - disabled for PvM - Vav

		NewEntries[4].dwCurrentSwitch = AutoShowMap;
		NewEntries[4].OnPress = &ExOptions::AutoOpt;

		NewEntries[5].dwCurrentSwitch = Maphack;
		NewEntries[5].OnPress = &ExOptions::MapOpt;

		NewEntries[6].dwCurrentSwitch = HideGold;
		NewEntries[6].OnPress = &ExOptions::GoldOpt;

		NewEntries[7].dwCurrentSwitch = HideCrap;
		NewEntries[7].OnPress = &ExOptions::CrapOpt;

		NewEntries[8].dwCurrentSwitch = bLagometer;
		NewEntries[8].OnPress = &ExOptions::LagOpt;

		NewEntries[9].dwCurrentSwitch = FullScreen;
		NewEntries[9].OnPress = &ExOptions::FSOpt;

		NewEntries[10].OnPress = &ExOptions::D2ExOpts;

		wcscpy_s((wchar_t*)&NewEntries[0].szCellFile, 130, gLocaleId == LOCALE_POL ? L"WSKA�NIK AUTOMAPY" : L"AUTOMAP BLOB");
		wcscpy_s((wchar_t*)&NewEntries[0].szSwitchCellFiles[0], 130, gLocaleId == LOCALE_POL ? L"KROPKA" : L"DOT");
		wcscpy_s((wchar_t*)&NewEntries[0].szSwitchCellFiles[1], 130, gLocaleId == LOCALE_POL ? L"KӣKO" : L"CIRCLE");
		wcscpy_s((wchar_t*)&NewEntries[0].szSwitchCellFiles[2], 130, gLocaleId == LOCALE_POL ? L"KRZY�YK" : L"CROSS");

		wcscpy_s((wchar_t*)&NewEntries[1].szCellFile, 130, gLocaleId == LOCALE_POL ? L"CZCIONKA NAZWY POZIOMU" : L"ENTERING LVL TEXT FONT");
		wcscpy_s((wchar_t*)&NewEntries[2].szCellFile, 130, gLocaleId == LOCALE_POL ? L"ZAWSZE POKAZUJ �YCIE" : L"ALWAYS SHOW LIFE");
		wcscpy_s((wchar_t*)&NewEntries[2].szSwitchCellFiles[0], 130, gLocaleId == LOCALE_POL ? L"WY�." : L"OFF");
		wcscpy_s((wchar_t*)&NewEntries[2].szSwitchCellFiles[1], 130, gLocaleId == LOCALE_POL ? L"W�." : L"ON");
		wcscpy_s((wchar_t*)&NewEntries[3].szCellFile, 130, gLocaleId == LOCALE_POL ? L"ZAWSZE POKAZUJ MANE" : L"ALWAYS SHOW MANA");
		wcscpy_s((wchar_t*)&NewEntries[3].szSwitchCellFiles[0], 130, gLocaleId == LOCALE_POL ? L"WY�." : L"OFF");
		wcscpy_s((wchar_t*)&NewEntries[3].szSwitchCellFiles[1], 130, gLocaleId == LOCALE_POL ? L"W�." : L"ON");
		/*wcscpy_s((wchar_t*)&NewEntries[4].szCellFile, 130, gLocaleId == LOCALE_POL ? L"PE�NA WIDOCZNO��" : L"FULL VISIBILITY (DISABLED)");
		wcscpy_s((wchar_t*)&NewEntries[4].szSwitchCellFiles[0], 130, gLocaleId == LOCALE_POL ? L"WY�." : L"OFF");
		wcscpy_s((wchar_t*)&NewEntries[4].szSwitchCellFiles[1], 130, gLocaleId == LOCALE_POL ? L"W�." : L"OFF"); - Disabled for PvM - Vav */
		wcscpy_s((wchar_t*)&NewEntries[4].szCellFile, 130, gLocaleId == LOCALE_POL ? L"AUTOM. W��CZ MAP�" : L"AUTO. TURN ON THE MAP");
		wcscpy_s((wchar_t*)&NewEntries[4].szSwitchCellFiles[0], 130, gLocaleId == LOCALE_POL ? L"WY�." : L"OFF");
		wcscpy_s((wchar_t*)&NewEntries[4].szSwitchCellFiles[1], 130, gLocaleId == LOCALE_POL ? L"W�." : L"ON");
		wcscpy_s((wchar_t*)&NewEntries[5].szCellFile, 130, L"MAPHACK");
		wcscpy_s((wchar_t*)&NewEntries[5].szSwitchCellFiles[0], 130, gLocaleId == LOCALE_POL ? L"WY�." : L"OFF");
		wcscpy_s((wchar_t*)&NewEntries[5].szSwitchCellFiles[1], 130, gLocaleId == LOCALE_POL ? L"W�." : L"ON");
		wcscpy_s((wchar_t*)&NewEntries[6].szCellFile, 130, gLocaleId == LOCALE_POL ? L"UKRYJ Z�OTO NA ZIEMI" : L"HIDE GOLD ON THE GROUND");
		wcscpy_s((wchar_t*)&NewEntries[6].szSwitchCellFiles[0], 130, gLocaleId == LOCALE_POL ? L"WY�." : L"OFF");
		wcscpy_s((wchar_t*)&NewEntries[6].szSwitchCellFiles[1], 130, gLocaleId == LOCALE_POL ? L"W�." : L"ON");
		wcscpy_s((wchar_t*)&NewEntries[7].szCellFile, 130, gLocaleId == LOCALE_POL ? L"UKRYJ �MIECI NA ZIEMI" : L"HIDE CRAP ON THE GROUND");
		wcscpy_s((wchar_t*)&NewEntries[7].szSwitchCellFiles[0], 130, gLocaleId == LOCALE_POL ? L"WY�." : L"OFF");
		wcscpy_s((wchar_t*)&NewEntries[7].szSwitchCellFiles[1], 130, gLocaleId == LOCALE_POL ? L"W�." : L"ON");
		wcscpy_s((wchar_t*)&NewEntries[8].szCellFile, 130, gLocaleId == LOCALE_POL ? L"LAGOMETR" : L"LAGOMETER");
		wcscpy_s((wchar_t*)&NewEntries[8].szSwitchCellFiles[0], 130, gLocaleId == LOCALE_POL ? L"WY�." : L"OFF");
		wcscpy_s((wchar_t*)&NewEntries[8].szSwitchCellFiles[1], 130, gLocaleId == LOCALE_POL ? L"W�." : L"ON");
		wcscpy_s((wchar_t*)&NewEntries[9].szCellFile, 130, gLocaleId == LOCALE_POL ? L"FULL SCREEN" : L"FULL SCREEN");//added by token
		wcscpy_s((wchar_t*)&NewEntries[9].szSwitchCellFiles[0], 130, gLocaleId == LOCALE_POL ? L"WY�." : L"OFF");//added by token
		wcscpy_s((wchar_t*)&NewEntries[9].szSwitchCellFiles[1], 130, gLocaleId == LOCALE_POL ? L"W�." : L"ON");//added by token

		wcscpy_s((wchar_t*)&NewEntries[10].szCellFile, 130, gLocaleId == LOCALE_POL ? L"POPRZEDNIE MENU" : L"PREVIOUS MENU");

	}

	*D2Vars.D2CLIENT_SelectedMenu = 0;
	*D2Vars.D2CLIENT_D2Menu = &NewMenu;
	*D2Vars.D2CLIENT_D2MenuEntries = &NewEntries[0];

	return TRUE;
}

// Import from Scrap
BOOL __fastcall ExOptions::KeyConfig(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	static D2Menu NewMenu = { 5, 28, 20, 37, 0, 0 };
	static D2MenuEntry NewEntries[5] = { 0 };

#ifndef D2EX_PVM_BUILD
	return false;
#endif

	if (!NewEntries[0].wItemName[0]) 
	{
		NewEntries[0].dwMenuType = D2MENU_STATIC;
		NewEntries[1].dwMenuType = NewEntries[2].dwMenuType = NewEntries[3].dwMenuType = D2MENU_KEY;
		NewEntries[1].Bind = (DWORD*)&VK_ATNext;
		NewEntries[2].Bind = (DWORD*)&VK_ATWP;
		NewEntries[3].Bind = (DWORD*)&VK_ATPrev;
		NewEntries[4].Bind = (DWORD*)&VK_FastTP;

		wcscpy_s(NewEntries[0].wItemName, 130, gLocaleId == LOCALE_POL ? L"KONFIG. KLAWISZY" : L"CONFIGURE CONTROLS");
		wcscpy_s(NewEntries[1].wItemName, 130, gLocaleId == LOCALE_POL ? L"AutoTele do nast. lok." : L"AutoTele to next loc.");
		wcscpy_s(NewEntries[2].wItemName, 130, gLocaleId == LOCALE_POL ? L"AutoTele do WP" : L"AutoTele to next WP");
		wcscpy_s(NewEntries[3].wItemName, 130, gLocaleId == LOCALE_POL ? L"AutoTele do poprz. lok." : L"AutoTele to prev loc.");
		wcscpy_s(NewEntries[4].wItemName, 130, gLocaleId == LOCALE_POL ? L"Szybkie TP" : L"Fast TP");
	}

	*D2Vars.D2CLIENT_SelectedMenu = 0;
	*D2Vars.D2CLIENT_D2Menu = &NewMenu;
	*D2Vars.D2CLIENT_D2MenuEntries = &NewEntries[0];
	return true;

}

BOOL __fastcall ExOptions::VideoOptions(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	static D2Menu NewMenu;
	static D2MenuEntry NewEntries[8];

	if (!NewMenu.dwEntriesNo)
	{
		//8, 45, 34, 49, 36, 0
		NewMenu.dwEntriesNo = 8;
		NewMenu.dwInterline = 50;
		NewMenu.dwTextHeight = 34;
		NewMenu.dwMenuOffset = 49;
		NewMenu.dwBarHeight = 36;

		memcpy(&NewEntries[0], (const void*)&D2Vars.D2CLIENT_VidOptionsMenu[0], sizeof(D2MenuEntry)* 8);

		wcscpy_s(NewEntries[0].wItemName, 130, L"VIDEO OPTIONS");
		wcscpy_s(NewEntries[1].wItemName, 130, L"RESOLUTION");
		wcscpy_s(NewEntries[2].wItemName, 130, L"LIGHTING QUALITY");
		wcscpy_s(NewEntries[3].wItemName, 130, L"BLENDED SHADOWS");
		wcscpy_s(NewEntries[4].wItemName, 130, L"PERSPECTIVE");
		wcscpy_s(NewEntries[5].wItemName, 130, L"GAMMA");
		wcscpy_s(NewEntries[6].wItemName, 130, L"CONTRAST");
		wcscpy_s(NewEntries[7].wItemName, 130, L"PREVIOUS MENU");

		NewEntries[0].ptCellFile = NewEntries[1].ptCellFile = NewEntries[2].ptCellFile = NewEntries[3].ptCellFile =
		NewEntries[4].ptCellFile = NewEntries[5].ptCellFile =	NewEntries[6].ptCellFile = NewEntries[7].ptCellFile = 0;

#ifdef D2EX_MULTIRES
		if (!DisableMultiRes) {
			NewEntries[1].dwMenuType = D2MENU_SWITCHEX;

			NewEntries[1].dwSwitchesNo = ExMultiRes::lResModes.size() + 3;
			NewEntries[1].dwCurrentSwitch = ExMultiRes::GFX_GetResolutionMode();
			NewEntries[1].OnPress = &ExOptions::ResolutionOpt;
			NewEntries[1].OnChange = 0;
			NewEntries[1].wSwitches[0] = new wstring(L"640x480");
			NewEntries[1].wSwitches[2] = NewEntries[1].wSwitches[1] = new wstring(L"800x600");
			int z = 3;
			for (auto i = ExMultiRes::lResModes.begin(); i != ExMultiRes::lResModes.end(); ++i) {
				wostringstream wstream;
				wstream << i->nWidth << "x" << i->nHeight;
				wstring * w = new wstring(wstream.str());
				NewEntries[1].wSwitches[z] = w;		// TODO: Consider where to free memory of these strings
				++z;
			}

			NewEntries[1].dwFontType = 2;
		}
#endif

		NewEntries[5].dwFontType = NewEntries[6].dwFontType = 2;

	}

	*D2Vars.D2CLIENT_SelectedMenu = 0;
	*D2Vars.D2CLIENT_D2Menu = &NewMenu;
	*D2Vars.D2CLIENT_D2MenuEntries = &NewEntries[0];

	return TRUE; 
}

BOOL __fastcall ExOptions::Options(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	static D2Menu NewMenu;
	static D2MenuEntry NewEntries[6];


	if (!NewMenu.dwEntriesNo)
	{
		NewMenu.dwEntriesNo = 6;
		NewMenu.dwInterline = 48;
		NewMenu.dwTextHeight = 45;
		NewMenu.dwMenuOffset = 51;

		D2Vars.D2CLIENT_SndOptionsMenu[7].OnPress = &ExOptions::Options;
		D2Vars.D2CLIENT_VidOptionsMenu[7].OnPress = &ExOptions::Options;
		D2Vars.D2CLIENT_MapOptionsMenu[6].OnPress = &ExOptions::Options;

		wcscpy_s(NewEntries[4].wItemName, 130, gLocaleId == LOCALE_POL ? L"OPCJE AUTOMAPY" : L"AUTOMAP OPTIONS");
		wcscpy_s(NewEntries[3].wItemName, 130, gLocaleId == LOCALE_POL ? L"USTAWIENIA KLAWISZY" : L"CONFIGURE CONTROLS");
		memcpy(NewEntries, (const void*)*&D2Vars.D2CLIENT_OptionsMenu, sizeof(D2MenuEntry)); // cpy Sound opt
		memcpy(&NewEntries[2], (const void*)&D2Vars.D2CLIENT_OptionsMenu[2], sizeof(D2MenuEntry)); // Automap, Controls
		memcpy(&NewEntries[2], (const void*)&D2Vars.D2CLIENT_OptionsMenu[2], sizeof(D2MenuEntry) * 2); // Automap, Controls
		memcpy(&NewEntries[5], (const void*)&D2Vars.D2CLIENT_OptionsMenu[4], sizeof(D2MenuEntry)); // Prev menu

		wcscpy_s(NewEntries[0].wItemName, 130, gLocaleId == LOCALE_POL ? L"OPCJE D�WI�KOWE" : L"SOUND OPTIONS");
		wcscpy_s(NewEntries[1].wItemName, 130, gLocaleId == LOCALE_POL ? L"OPCJE GRAFICZNE" : L"VIDEO OPTIONS");
		wcscpy_s(NewEntries[4].wItemName, 130, gLocaleId == LOCALE_POL ? L"OPCJE AUTOMAPY" : L"AUTOMAP OPTIONS");
		wcscpy_s(NewEntries[3].wItemName, 130, gLocaleId == LOCALE_POL ? L"USTAWIENIA KLAWISZY" : L"CONFIGURE CONTROLS");
		wcscpy_s(NewEntries[2].wItemName, 130, gLocaleId == LOCALE_POL ? L"USTAWIENIA D2EX" : L"EXTRA OPTIONS");
		wcscpy_s(NewEntries[5].wItemName, 130, gLocaleId == LOCALE_POL ? L"POPRZEDNIE MENU" : L"PREVIOUS MENU");

		NewEntries[0].ptCellFile = NewEntries[1].ptCellFile = NewEntries[2].ptCellFile = NewEntries[3].ptCellFile = NewEntries[4].ptCellFile = NewEntries[5].ptCellFile = 0;

		NewEntries[1].OnPress = &ExOptions::VideoOptions;

		//NewEntries[2].dwCurrentValue = COL_ORANGE;
		NewEntries[2].OnPress = &ExOptions::D2ExOpts;
		NewEntries[5].OnPress = &ExOptions::MainMenu;
	}

	*D2Vars.D2CLIENT_SelectedMenu = 0;
	*D2Vars.D2CLIENT_D2Menu = &NewMenu;
	*D2Vars.D2CLIENT_D2MenuEntries = &NewEntries[0];

	return TRUE;
}

BOOL __fastcall ExOptions::D2ExOpts(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	static D2Menu NewMenu = { 4, 48, 45, 51, 0, 0 };
	static D2MenuEntry NewEntries[4] = { 0 };

	if (!NewEntries[0].OnPress)
	{
		NewEntries[0].OnPress = &ExOptions::Buffs;
		//NewEntries[1].OnPress = &ExOptions::KeyConfig;
		NewEntries[1].OnPress = &ExOptions::Various;
		NewEntries[2].OnPress = &ExOptions::Options;

		wcscpy_s(NewEntries[0].wItemName, 130, gLocaleId == LOCALE_POL ? L"USTAWIENIA EFEKT�W" : L"CONFIGURE BUFFS");
		//wcscpy_s(NewEntries[1].wItemName, 130, gLocaleId == LOCALE_POL ? L"USTAWIENIA KLAWISZY" : L"CONFIGURE CONTROLS");
		wcscpy_s(NewEntries[1].wItemName, 130, gLocaleId == LOCALE_POL ? L"RӯNE USTAWIENIA" : L"MISC. SETTINGS");
		wcscpy_s(NewEntries[2].wItemName, 130, gLocaleId == LOCALE_POL ? L"POPRZEDNIE MENU" : L"PREVIOUS MENU");
	}

	*D2Vars.D2CLIENT_SelectedMenu = 0;
	*D2Vars.D2CLIENT_D2Menu = &NewMenu;
	*D2Vars.D2CLIENT_D2MenuEntries = &NewEntries[0];
	return true;

}

BOOL __fastcall ExOptions::MainMenu(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	static D2Menu NewMenu;
	static D2MenuEntry NewEntries[4];

	if (!NewMenu.dwEntriesNo)
	{
		NewMenu.dwEntriesNo = 4;
		NewMenu.dwInterline = 50;
		NewMenu.dwTextHeight = 45;
		NewMenu.dwMenuOffset = 51;



		memcpy(NewEntries, (const void*)*&D2Vars.D2CLIENT_OldMenu, sizeof(D2MenuEntry));
		memcpy(&NewEntries[2], (const void*)&D2Vars.D2CLIENT_OldMenu[1], sizeof(D2MenuEntry)* 2);

		NewEntries[0].ptCellFile = NewEntries[2].ptCellFile = NewEntries[3].ptCellFile = 0;

		int i = 0;
		wcscpy_s((wchar_t*)&NewEntries[i].szCellFile, 130, gLocaleId == LOCALE_POL ? L"OPCJE" : L"OPTIONS");
		NewEntries[i].OnPress = &ExOptions::Options;
		
#ifdef D2EX_PVPGN_EXT
	#ifdef D2EX_PVPGN_GIVEUP
		wcscpy_s((wchar_t*)&NewEntries[++i].szCellFile,130,gLocaleId == LOCALE_POL ? L"PODDAJ SI�" : L"GIVE UP");
		NewEntries[i].OnPress = &ExOptions::GiveUpCB;
		NewEntries[i].EnableCheck = &ExOptions::GiveUpCheck;
	#else
		NewMenu.dwEntriesNo = 3;

	#endif
#else
		wcscpy_s((wchar_t*)&NewEntries[++i].szCellFile, 130, gLocaleId == LOCALE_POL ? L"SZYBKI POWR�T" : L"QUICK REJOIN");
		NewEntries[i].OnPress = &ExOptions::RejoinCB;
		NewEntries[i].EnableCheck = 0;
#endif
		wcscpy_s((wchar_t*)&NewEntries[++i].szCellFile, 130, gLocaleId == LOCALE_POL ? L"ZAPIS I WYJ�CIE Z GRY" : L"SAVE AND EXIT GAME");
		NewEntries[i].OnPress = &ExOOG::LeaveGame;

		wcscpy_s((wchar_t*)&NewEntries[++i].szCellFile, 130, gLocaleId == LOCALE_POL ? L"POWR�T DO GRY" : L"RETURN TO GAME");
		NewEntries[i].OnPress = *D2Vars.D2CLIENT_OldMenu[2].OnPress;
	}

	*D2Vars.D2CLIENT_SelectedMenu = 1;
	*D2Vars.D2CLIENT_D2Menu = &NewMenu;
	*D2Vars.D2CLIENT_D2MenuEntries = &NewEntries[0];

	return true;
}
