#include "invview.h"

#include <map>
#include <set>
#include <gdiplus.h>
#pragma comment (lib,"gdiplus.lib")
#include <algorithm>

#include "statview.h"
#include "treeview.h"
#include "common.h"
#include "include.h"
#include "d2data.h"
#include "resource.h"
#include "trade.h"

#pragma warning(disable: 4503)

using namespace Gdiplus;
using namespace std;

//map<string, vector<string>> g_item_colors = {
//	{"Black", {"Jagged", "Forked", "Serrated", "Ferocious", "Cruel", "Rakescar", "The Diggler", "Howltusk", "Rockfleece", "Goblin Toe", "The Vile Husk", "Ironpelt", "Dwarf Star"}},
//	{"Black2", {"The Humongous", "Crushflange", "Rixots Keen", "Dimoaks Hew", "Undead Crown", "Snakecord", "Blackhand Key", "Blackleach Blade", "Skullcollector", "Demon Machine", "Darksight Helm", "Blackhorn's Face", "Corpsemourn", "Messerschmidt's Reaver", "The Cranium Basher"}},
//	{"Blue-Black", {"Maelstromwrath", "The Generals Tan Do Li Ga", "Irices Shard", "Woestave", "Gorefoot", "Baezil's Vortex", "Skin of the Vipermagi", "Stormspire"}},
//	{"Blue-Gray", {"Umes Lament", "The Battlebranch", "The Face of Horror", "Goldwrap", "The Atlantian", "Ribcracker", "Whichwild String", "Schaefer's Hammer", "Crescent Moon"}},
//	{"BrightBlue", {"Ironstone", "Ripsaw", "Pluckeye", "Heavenly Garb", "The Hand of Broc", "Coldkill", "Butcher's Pupil", "Carin Shard", "Stormspike", "Warpspear", "Stormchaser", "Constricting Ring", "Raven Frost", "Ginther's Rift"}},
//	{"BrightRed", {"Stormeye", "The Centurion", "Hotspur", "Pompe's Wrath", "Bloodletter", "Blade of Ali Baba", "Grim's Burning Dead", "Riphook"}},
//	{"CrystalBlue", {"Vulpine", "Drake's", "Dragon's", "Wyrm's", "Great Wyrm's", "Bahamut's", "Zircon", "Jacinth", "Turquoise", "Mnemonic"}},
//	{"CrystalGreen", {"Archer's", "Archer's", "Priest's", "Necromancer's", "Arch-Angel's", "Berserker's", "Jade", "Emerald", "Beryl", "Valkyrie's", "Hierophant's", "Witch-hunter's"}},
//	{"DarkBlue", {"Devious", "Fortified", "Null", "Antimagic"}},
//	{"DarkGold", {"Saintly", "Holy", "Massive", "Merciless", "Knight's", "Lord's", "King's", "Godly", "Savage", "Master's", "Grandmaster's", "Elysian", "Celestial"}},
//	{"DarkGreen", {"Toxic", "Pestilent"}},
//	{"DarkRed", {"Red", "Sanguinary", "Bloody", "Scarlet", "Crimson", "Carbuncle", "Carmine", "Vermillion", "Cinnabar", "Rusty", "Realgar", "Ruby"}},
//	{"Green-Black", {"Bane Ash", "Twitchthroe", "Treads of Cthon", "Kerke's Sanctuary", "Soulflay", "Wizendraw", "Darkglow", "Tearhaunch"}},
//	{"Green-Cyan", {"Steeldriver", "Steelgoad", "Serpent Lord", "Hawkmail", "Lenyms Cord", "Pus Spiter", "Vampiregaze", "Harlequin Crest", "Atma's Scarab"}},
//	{"LightBlue", {"Sapphire", "Cobalt", "Lapis Lazuli", "Boreal", "Hibernal"}},
//	{"LightGold", {"Gold", "Platinum", "Meteoric", "Strange", "Weird", "Bowyer's", "Archer's", "Gymnast's", "Athlete's", "Spearmaiden's", "Lancer's", "Blazing", "Volcanic", "Charged", "Powered", "Freezing", "Glacial", "Blighting", "Accursed", "Noxious", "Venomous", "Vodoun", "Golemlord's", "Hawk Branded", "Rose Branded", "Commander's", "Marshal's", "Warder's", "Guardian's", "Veteran's", "Master's", "Raging", "Furious", "Resonant", "Echoing", "Caretaker's", "Keeper's", "Feral", "Communal", "Terra's", "Gaea's", "Trickster's", "Cunning", "Psychic", "Shadow", "Sensei's", "Kenshi's"}},
//	{"LightGreen", {"Emerald"}},
//	{"LightPurple", {"Prismatic", "Shimmering", "Rainbow", "Chromatic", "Scintillating"}},
//	{"LightRed", {"Ruby", "Garnet", "Russet", "Smoking", "Flaming"}},
//	{"LightYellow", {"Glowing", "Amber", "Coral", "Camphor", "Ambergris"}},
//	{"Orange", {"Howling", "Screaming", "Wailing"}},
//	{"Purple", {"Fechmars Axe", "The Grim Reaper", "Zakarum's Hand", "Kuko Shakaku"}},
//	{"Purple-Black", {"Goreshovel", "The Dragon Chang", "Rattlecage", "Bul Katho's Wedding Band", "Saracen's Chance"}},
//	{"Red-Black", {"Knell Striker", "The Salamander", "Rimeraven", "Blinkbats Form", "Frostburn", "The Meat Scraper", "Hellslayer", "Doombringer"}},
//	{"Red-Gray", {"Lance of Yaggai", "Witherstring", "Chance Guards", "Stormrider", "The Impaler", "Skin of the Flayerd One", "Arkaine's Valor", "Baranar's Star"}},
//	{"White", {"Blanched", "Eburin", "Bone", "Ivory", "Miocene", "Oligocene", "Eocene", "Paleocene", "Knave's", "Jack's", "Jester's", "Joker's", "Trump", "Loud", "Calling", "Yelling", "Shouting", "Screaming", "Paradox", "Robineye", "Sparroweye", "Falconeye", "Hawkeye", "Eagleeye", "Visionary", "Snowflake", "Shivering", "Hibernal", "Ember", "Smoldering", "Smoking", "Flaming", "Scorching", "Static", "Glowing", "Buzzing", "Arcing", "Shocking", "Septic", "Envenomed", "Corosive", "Toxic", "Pestilent", "Headstriker", "Swordguard", "Highlord's Wrath"}},
//	{"Yellow", {"Blastbark", "Sparking Mail", "Nightsmoke", "Soulfeast Tine", "The Grandfather"}},
//	{"Yellow-Black", {"The Iron Jang Bong", "Venomsward", "Bladebuckle", "Windforce"}},
//	{"Yellow-Black2", {"Boneflesh", "Dark Clan Crusher", "Crown of Thieves", "Lidless Wall", "Eaglehorn"}},
//	{"Yellow-Gray", {"Rusthandle", "Iceblink", "Pierre Tombale Couant", "The Rising Sun"}},
//	{"Yellow-Gray2", {"Tarnhelm", "Bloodfist", "Duriel's Shell", "The Cat's Eye", "Mara's Kaleidoscope"}},
//	{"gray", {"Culwens Point", "Bloodthief", "Warlord's Trust", "Spellsteel", "Toothrow"}},
//	{"gray2", {"Bladebone", "Gull", "The Tannr Gorerod", "Greyform", "Magefist", "Spire of Honor", "Godstrike Arch", "Guardian Angel", "Tiamat's Rebuke", "Veil of Steel", "The Gladiator's Bane", "Wizardspike"}},
////};

void ethtransform(BYTE* r, BYTE* g, BYTE* b) {
	/**r = 255;
	*g -= 100;
	*b -= 100;*/
}

Image* LoadPicture(LPCWSTR lpszFileName) {
	Image* pImage = Image::FromFile(lpszFileName, FALSE);
	return pImage;
}
BOOL DrawPicture(HDC hDC, Image* pImage, LPRECT lpRect) {
	if (pImage == NULL || lpRect == NULL)
		return FALSE;
	Graphics graphics(hDC);
	graphics.DrawImage(pImage, lpRect->left, lpRect->top,
		lpRect->right - lpRect->left, lpRect->bottom - lpRect->top);
	return TRUE;
}
void PremultiplyBitmapAlpha(HDC hDC, HBITMAP hBmp) {
	BITMAP bm = {0};
	GetObject(hBmp, sizeof(bm), &bm);
	BITMAPINFO* bmi = (BITMAPINFO*)_alloca(sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD)));
	::ZeroMemory(bmi, sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD)));
	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BOOL bRes = ::GetDIBits(hDC, hBmp, 0, bm.bmHeight, NULL, bmi, DIB_RGB_COLORS);
	if (!bRes || bmi->bmiHeader.biBitCount != 32) return;
	LPBYTE pBitData = (LPBYTE) ::LocalAlloc(LPTR, bm.bmWidth * bm.bmHeight * sizeof(DWORD));
	if (pBitData == NULL) return;
	LPBYTE pData = pBitData;
	::GetDIBits(hDC, hBmp, 0, bm.bmHeight, pData, bmi, DIB_RGB_COLORS);
	for (int y = 0; y < bm.bmHeight; y++) {
		for (int x = 0; x < bm.bmWidth; x++) {
		//	pData[3] = 10;
			pData[0] = (BYTE)((DWORD)pData[0] * 100/* pData[3]*/ / 255);
			pData[1] = (BYTE)((DWORD)pData[1] * 100/* pData[3]*/ / 255);
			pData[2] = (BYTE)((DWORD)pData[2] * 100/* pData[3]*/ / 255);
			pData += 4;
		}
	}
	::SetDIBits(hDC, hBmp, 0, bm.bmHeight, pBitData, bmi, DIB_RGB_COLORS);
	::LocalFree(pBitData);
}
void TransparentBlitToHdc(HDC hdcDst, HBITMAP hbmSrc, int x, int y, int w, int h, COLORREF transparent, void(*transfunc)(BYTE*, BYTE*, BYTE*)/* = NULL*/) {
	HDC hdcScreen = GetDC(NULL);
	HDC hdcSrc = CreateCompatibleDC(hdcScreen);
	HBITMAP hbmOld = NULL;

	if (!transfunc) {
		HBITMAP mask = CreateBitmapMask(hbmSrc, transparent);
		hbmOld = static_cast<HBITMAP>(SelectObject(hdcSrc, mask));
		BitBlt(hdcDst, x, y, w, h, hdcSrc, 0, 0, SRCAND);
		HBITMAP deadmask = static_cast<HBITMAP>(SelectObject(hdcSrc, hbmSrc));
		DeleteObject(deadmask);
		BitBlt(hdcDst, x, y, w, h, hdcSrc, 0, 0, SRCPAINT);
	}
	else {
		BITMAP bm = {0};
		GetObject(hbmSrc, sizeof(bm), &bm);

		hbmOld = static_cast<HBITMAP>(SelectObject(hdcSrc, hbmSrc));
		HDC hdcSrcproxy = CreateCompatibleDC(hdcScreen);
		HBITMAP hbmSrcCpy = CreateCompatibleBitmap(hdcScreen, bm.bmWidth, bm.bmHeight);
		DeleteObject(SelectObject(hdcSrcproxy, hbmSrcCpy));
		BitBlt(hdcSrcproxy, 0, 0, bm.bmWidth, bm.bmHeight, hdcSrc, 0, 0, SRCCOPY);

		BITMAPINFO bmi = {0};
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = bm.bmWidth;
		bmi.bmiHeader.biHeight = bm.bmHeight;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;

		LPBYTE pBitData = (LPBYTE)LocalAlloc(LPTR, bm.bmWidth * bm.bmHeight * sizeof(DWORD));
		if (pBitData == NULL) return;
		BYTE* pData = pBitData;

		GetDIBits(hdcSrcproxy, hbmSrcCpy, 0, bm.bmHeight, pData, &bmi, DIB_RGB_COLORS);
		for (int y1 = 0; y1 < bm.bmHeight; y1++) {
			for (int x1 = 0; x1 < bm.bmWidth; x1++) {
				COLORREF pixel = pData[0] | pData[1] << 8 | pData[2] << 16;
				if (pixel != transparent) {
					transfunc(&pData[2], &pData[1], &pData[0]);
				}
				pData += 4;
			}
		}
		SetDIBits(hdcSrcproxy, hbmSrcCpy, 0, bm.bmHeight, pBitData, &bmi, DIB_RGB_COLORS);

		if (transfunc != ethtransform) {
			HBITMAP mask = CreateBitmapMask(hbmSrcCpy, transparent);
			DeleteObject(static_cast<HBITMAP>(SelectObject(hdcSrcproxy, mask)));
			BitBlt(hdcDst, x, y, w, h, hdcSrcproxy, 0, 0, SRCAND);
		}

		DeleteObject(static_cast<HBITMAP>(SelectObject(hdcSrcproxy, hbmSrcCpy)));
		
		BitBlt(hdcDst, x, y, w, h, hdcSrcproxy, 0, 0, SRCPAINT);

		DeleteDC(hdcSrcproxy);

		DeleteObject(hbmSrcCpy);

		LocalFree(pBitData);
	}

	if (hbmOld) SelectObject(hdcSrc, hbmOld);
	DeleteDC(hdcSrc);
	ReleaseDC(NULL, hdcScreen);
}
HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent) {
	HDC hdcMem, hdcMem2;
	HBITMAP hbmMask;
	BITMAP bm;

	// Create monochrome (1 bit) mask bitmap. 
	GetObject(hbmColour, sizeof(BITMAP), &bm);
	hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

	// Get some HDCs that are compatible with the display driver
	hdcMem = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);

	SelectBitmap(hdcMem, hbmColour);
	SelectBitmap(hdcMem2, hbmMask);

	// Set the background colour of the colour image to the colour
	// you want to be transparent.
	SetBkColor(hdcMem, crTransparent);

	// Copy the bits from the colour image to the B+W mask... everything
	// with the background colour ends up white while everythig else ends up
	// black...Just what we wanted.

	BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

	// Take our new mask and use it to turn the transparent colour in our
	// original colour image to black so the transparency effect will
	// work right.
	BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

	// Clean up.
	DeleteDC(hdcMem);
	DeleteDC(hdcMem2);

	return hbmMask;
}
string GetStorePNG(string itemstore, const InvData* data) {
	if (itemstore == "cube") return "supertransmogrifier";
	if (itemstore == "inventory" || itemstore == "body")  return data->x >= 11 ? "inv2" : "inv";
	if (itemstore == "stash") return "TradeStash";
	if (itemstore == "mercenary") return "NpcInv";
	else return "";
}
POINT get_item_inv_loc_from_pt(string store, POINT pt, const InvData& invdata) {
	if (store == "inventory") {
		vector<RECT> inv = {
			{18, 255, 47, 284}, {47, 255, 76, 284}, {76, 255, 105, 284}, {105, 255, 134, 284}, {134, 255, 163, 284}, {163, 255, 192, 284}, {192, 255, 221, 284}, {221, 255, 250, 284}, {250, 255, 279, 284}, {279, 255, 308, 284},
			{18, 284, 47, 313}, {47, 284, 76, 313}, {76, 284, 105, 313}, {105, 284, 134, 313}, {134, 284, 163, 313}, {163, 284, 192, 313}, {192, 284, 221, 313}, {221, 284, 250, 313}, {250, 284, 279, 313}, {279, 284, 308, 313},
			{18, 313, 47, 342}, {47, 313, 76, 342}, {76, 313, 105, 342}, {105, 313, 134, 342}, {134, 313, 163, 342}, {163, 313, 192, 342}, {192, 313, 221, 342}, {221, 313, 250, 342}, {250, 313, 279, 342}, {279, 313, 308, 342},
			{18, 342, 47, 371}, {47, 342, 76, 371}, {76, 342, 105, 371}, {105, 342, 134, 371}, {134, 342, 163, 371}, {163, 342, 192, 371}, {192, 342, 221, 371}, {221, 342, 250, 371}, {250, 342, 279, 371}, {279, 342, 308, 371}
		};
		for (UINT i = 0; i < inv.size(); i++)
			if (PtInRect(&inv[i], pt))
				return {(LONG)(i % 10), (LONG)(i / 10)};
	}
	else if (store == "body") {
		vector<RECT> body = {
			{132, 4, 190, 62}, {205, 30, 234, 59}, {134, 74, 192, 161}, {17, 45, 75, 161},
			{248, 45, 306, 161}, {91, 175, 120, 204}, {205, 175, 234, 204}, {134, 175, 192, 204},
			{251, 175, 309, 233}, {18, 176, 76, 234}, {17, 45, 75, 161}, {248, 45, 306, 161}
		};
		for (UINT i = 0; i < body.size(); i++)
			if (PtInRect(&body[i], pt) && !(invdata.x > 10 && (i == 3 || i == 4)))
				return {(LONG)i + 1, -1};
	}
	else if (store == "mercenary") {
		vector<RECT> merc = {{132, 4, 190, 62}, {134, 74, 192, 161}, {17, 45, 75, 161}, {248, 45, 306, 161}};
		for (UINT i = 0; i < merc.size(); i++)
			if (PtInRect(&merc[i], pt))
				return {(LONG)i + (i > 0 ? 2 : 1), -1};
	}
	else if (store == "cube") {
		vector<RECT> cube = {
			{16, 15, 45, 44}, {45, 15, 74, 44}, {74, 15, 103, 44}, {103, 15, 132, 44}, {132, 15, 161, 44}, {161, 15, 190, 44}, {190, 15, 219, 44}, {219, 15, 248, 44}, {248, 15, 277, 44}, {277, 15, 306, 44},
			{16, 44, 45, 73}, {45, 44, 74, 73}, {74, 44, 103, 73}, {103, 44, 132, 73}, {132, 44, 161, 73}, {161, 44, 190, 73}, {190, 44, 219, 73}, {219, 44, 248, 73}, {248, 44, 277, 73}, {277, 44, 306, 73},
			{16, 73, 45, 102}, {45, 73, 74, 102}, {74, 73, 103, 102}, {103, 73, 132, 102}, {132, 73, 161, 102}, {161, 73, 190, 102}, {190, 73, 219, 102}, {219, 73, 248, 102}, {248, 73, 277, 102}, {277, 73, 306, 102},
			{16, 102, 45, 131}, {45, 102, 74, 131}, {74, 102, 103, 131}, {103, 102, 132, 131}, {132, 102, 161, 131}, {161, 102, 190, 131}, {190, 102, 219, 131}, {219, 102, 248, 131}, {248, 102, 277, 131}, {277, 102, 306, 131},
			{16, 131, 45, 160}, {45, 131, 74, 160}, {74, 131, 103, 160}, {103, 131, 132, 160}, {132, 131, 161, 160}, {161, 131, 190, 160}, {190, 131, 219, 160}, {219, 131, 248, 160}, {248, 131, 277, 160}, {277, 131, 306, 160},
			{16, 160, 45, 189}, {45, 160, 74, 189}, {74, 160, 103, 189}, {103, 160, 132, 189}, {132, 160, 161, 189}, {161, 160, 190, 189}, {190, 160, 219, 189}, {219, 160, 248, 189}, {248, 160, 277, 189}, {277, 160, 306, 189},
			{16, 189, 45, 218}, {45, 189, 74, 218}, {74, 189, 103, 218}, {103, 189, 132, 218}, {132, 189, 161, 218}, {161, 189, 190, 218}, {190, 189, 219, 218}, {219, 189, 248, 218}, {248, 189, 277, 218}, {277, 189, 306, 218},
			{16, 218, 45, 247}, {45, 218, 74, 247}, {74, 218, 103, 247}, {103, 218, 132, 247}, {132, 218, 161, 247}, {161, 218, 190, 247}, {190, 218, 219, 247}, {219, 218, 248, 247}, {248, 218, 277, 247}, {277, 218, 306, 247}
		};
		for (UINT i = 0; i < cube.size(); i++)
			if (PtInRect(&cube[i], pt))
				return {(LONG)i % 10, (LONG)i / 10};
	}
	else if (store == "stash") {
		vector<RECT> stash = {
			{15, 84, 44, 113}, {44, 84, 73, 113}, {73, 84, 102, 113}, {102, 84, 131, 113}, {131, 84, 160, 113}, {160, 84, 189, 113}, {189, 84, 218, 113}, {218, 84, 247, 113}, {247, 84, 276, 113}, {276, 84, 305, 113},
			{15, 113, 44, 142}, {44, 113, 73, 142}, {73, 113, 102, 142}, {102, 113, 131, 142}, {131, 113, 160, 142}, {160, 113, 189, 142}, {189, 113, 218, 142}, {218, 113, 247, 142}, {247, 113, 276, 142}, {276, 113, 305, 142},
			{15, 142, 44, 171}, {44, 142, 73, 171}, {73, 142, 102, 171}, {102, 142, 131, 171}, {131, 142, 160, 171}, {160, 142, 189, 171}, {189, 142, 218, 171}, {218, 142, 247, 171}, {247, 142, 276, 171}, {276, 142, 305, 171},
			{15, 171, 44, 200}, {44, 171, 73, 200}, {73, 171, 102, 200}, {102, 171, 131, 200}, {131, 171, 160, 200}, {160, 171, 189, 200}, {189, 171, 218, 200}, {218, 171, 247, 200}, {247, 171, 276, 200}, {276, 171, 305, 200},
			{15, 200, 44, 229}, {44, 200, 73, 229}, {73, 200, 102, 229}, {102, 200, 131, 229}, {131, 200, 160, 229}, {160, 200, 189, 229}, {189, 200, 218, 229}, {218, 200, 247, 229}, {247, 200, 276, 229}, {276, 200, 305, 229},
			{15, 229, 44, 258}, {44, 229, 73, 258}, {73, 229, 102, 258}, {102, 229, 131, 258}, {131, 229, 160, 258}, {160, 229, 189, 258}, {189, 229, 218, 258}, {218, 229, 247, 258}, {247, 229, 276, 258}, {276, 229, 305, 258},
			{15, 258, 44, 287}, {44, 258, 73, 287}, {73, 258, 102, 287}, {102, 258, 131, 287}, {131, 258, 160, 287}, {160, 258, 189, 287}, {189, 258, 218, 287}, {218, 258, 247, 287}, {247, 258, 276, 287}, {276, 258, 305, 287},
			{15, 287, 44, 316}, {44, 287, 73, 316}, {73, 287, 102, 316}, {102, 287, 131, 316}, {131, 287, 160, 316}, {160, 287, 189, 316}, {189, 287, 218, 316}, {218, 287, 247, 316}, {247, 287, 276, 316}, {276, 287, 305, 316},
			{15, 316, 44, 345}, {44, 316, 73, 345}, {73, 316, 102, 345}, {102, 316, 131, 345}, {131, 316, 160, 345}, {160, 316, 189, 345}, {189, 316, 218, 345}, {218, 316, 247, 345}, {247, 316, 276, 345}, {276, 316, 305, 345},
			{15, 345, 44, 374}, {44, 345, 73, 374}, {73, 345, 102, 374}, {102, 345, 131, 374}, {131, 345, 160, 374}, {160, 345, 189, 374}, {189, 345, 218, 374}, {218, 345, 247, 374}, {247, 345, 276, 374}, {276, 345, 305, 374}
		};
		for (UINT i = 0; i < stash.size(); i++)
			if (PtInRect(&stash[i], pt))
				return {(LONG)i % 10, (LONG)i / 10};
	}
	return {-1, -1};
}
ItemData* get_item_from_loc(string realm, string account, string character, string store, POINT pt) {
	vector<ItemData>* items = get_item_store(realm, account, character, store);
	for (UINT i = 0; i < items->size(); i++) {
		if (store != "body" && store != "mercenary") {
			POINT itempt = {items->at(i).invdata.x, items->at(i).invdata.y};
			POINT itemdim = {items->at(i).invdata.w, items->at(i).invdata.h};
			RECT itemrc = {itempt.x, itempt.y, itempt.x + itemdim.x, itempt.y + itemdim.y};
			if (PtInRect(&itemrc, pt))
				return &items->at(i);
		}
		else
			if (pt.x == items->at(i).invdata.x)
				return &items->at(i);
	}
	return NULL;
}
RECT get_item_inv_rect(const ItemData* item) {
	if (!item) return {0};
	RECT rc = {0};
	if (item->store == "inventory") {
		rc.left = item->invdata.x * 29 + 18;
		rc.top = item->invdata.y * 29 + 255;
	}
	else if (item->store == "stash") {
		rc.left = item->invdata.x * 29 + 15;
		rc.top = item->invdata.y * 29 + 84;
	}
	else if (item->store == "cube") {
		rc.left = item->invdata.x * 29 + 16;
		rc.top = item->invdata.y * 29 + 15;
	}
	else if ((item->store == "body" || item->store == "mercenary")) {
		switch (item->invdata.x) {
			case 1:{//helm
				rc.left = 132;
				rc.top = 4;
				break;
			}
			case 2:{//amulet
				rc.left = 205;
				rc.top = 30;
				break;
			}
			case 3:{//armor
				rc.left = 134;
				rc.top = 74;
				break;
			}
			case 4:{//left wep1
				rc.left = (item->invdata.w == 2 ? 17 : 32);
				rc.top = (item->invdata.h == 4 ? 45 : item->invdata.h == 3 ? 59 : item->invdata.h == 2 ? 74 : 88);
				break;
			}
			case 5:{//right wep1
				rc.left = (item->invdata.w == 2 ? 248 : 263);
				rc.top = (item->invdata.h == 4 ? 45 : item->invdata.h == 3 ? 59 : item->invdata.h == 2 ? 74 : 88);
				break;
			}
			case 6:{//left ring
				rc.left = 91;
				rc.top = 175;
				break;
			}
			case 7:{//right ring
				rc.left = 205;
				rc.top = 175;
				break;
			}
			case 8:{//belt
				rc.left = 134;
				rc.top = 175;
				break;
			}
			case 9:{//boots
				rc.left = 251;
				rc.top = 175;
				break;
			}
			case 10:{//gloves
				rc.left = 18;
				rc.top = 176;
				break;
			}
			case 11:{//left wep2
				rc.left = (item->invdata.w == 2 ? 21 : 36);
				rc.top = (item->invdata.h == 4 ? 41 : item->invdata.h == 3 ? 55 : item->invdata.h == 2 ? 70 : 84);
				break;
			}
			case 12:{//right wep2
				rc.left = (item->invdata.w == 2 ? 252 : 267);
				rc.top = (item->invdata.h == 4 ? 41 : item->invdata.h == 3 ? 55 : item->invdata.h == 2 ? 70 : 84);
				break;
			}
			default:
				break;
		}
	}
	rc.right = 29 * item->invdata.w + rc.left;
	rc.bottom = 29 * item->invdata.h + rc.top;
	return rc;
}

bool is_item_in_selrc(ItemData* item, RECT *selectionrc) {
	RECT itemrc = get_item_inv_rect(item);
	return (itemrc.left < selectionrc->right && itemrc.right > selectionrc->left &&	itemrc.top < selectionrc->bottom && itemrc.bottom > selectionrc->top);
}
void FrameRectRainbow(HDC hDC, RECT *rc, int rate){
	HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
	
	static unsigned int rotate = 0;	
	rotate++;
	for (UINT i = 0; i < 12; i++) {
		COLORREF rainbowcol = RGB(255, 0, 105);
		if ((i + rotate / rate) % 12 == 1) rainbowcol = RGB(255, 0, 0);
		if ((i + rotate / rate) % 12 == 2) rainbowcol = RGB(255, 150, 0);
		if ((i + rotate / rate) % 12 == 3) rainbowcol = RGB(255, 255, 0);
		if ((i + rotate / rate) % 12 == 4) rainbowcol = RGB(105, 255, 0);
		if ((i + rotate / rate) % 12 == 5) rainbowcol = RGB(0, 255, 0);
		if ((i + rotate / rate) % 12 == 6) rainbowcol = RGB(0, 255, 150);
		if ((i + rotate / rate) % 12 == 7) rainbowcol = RGB(0, 255, 255);
		if ((i + rotate / rate) % 12 == 8) rainbowcol = RGB(0, 105, 255);
		if ((i + rotate / rate) % 12 == 9) rainbowcol = RGB(0, 0, 255);
		if ((i + rotate / rate) % 12 == 10) rainbowcol = RGB(150, 0, 255);
		if ((i + rotate / rate) % 12 == 11) rainbowcol = RGB(255, 0, 255);
		if ((i + rotate / rate) % 12 == 0) rainbowcol = RGB(255, 0, 105);
		HPEN pen = CreatePen(PS_SOLID, 1, rainbowcol);
		SelectObject(hDC, pen);

		int w = rc->right - rc->left;
		int h = rc->bottom - rc->top;
		auto start = [&](int d) -> int {return i * (d / 12); };
		auto end = [&](int d) -> int {return (i + 1) * (d / 12); };

		//left
		MoveToEx(hDC, rc->left, rc->bottom - start(h), NULL);
		LineTo(hDC, rc->left, i == 11 ? rc->top : rc->bottom - end(h));
		//top
		MoveToEx(hDC, rc->left + start(w), rc->top, NULL);
		LineTo(hDC, i == 11 ? rc->right : rc->left + end(w), rc->top);
		//right
		MoveToEx(hDC, rc->right, rc->top + start(h), NULL);
		LineTo(hDC, rc->right, i == 11 ? rc->bottom : rc->top + end(h));
		//bottom
		MoveToEx(hDC, rc->right - start(w), rc->bottom, NULL);
		LineTo(hDC, i == 11 ? rc->left : rc->right - end(w), rc->bottom);

		SelectObject(hDC, hpenOld);

		DeleteObject(pen);
	}
}

enum {
	MENU_SELECTMENU,
	MENU_STAT,
	MENU_WAYPOINT_NORM,
	MENU_WAYPOINT_NIGHT,
	MENU_WAYPOINT_HELL,
	MENU_QUEST_NORM,
	MENU_QUEST_NIGHT,
	MENU_QUEST_HELL,
	MENU_SKILLTREE
};
struct menu_option {
	BYTE id;
	string text;
	RECT rc;

	bool is_hover = false;
	bool has_focus = false;
	bool is_selected = false;
};
class charstats_menu {
	vector<menu_option> options;
	//320, 432
	public:

	int last_hover = -1;
	int focus = -1;
	int selected = -1;

	UINT getSize() { return options.size(); }
	void addOption(BYTE id, std::string text) {
		menu_option op;
		op.id = id;
		op.text = text;
		options.push_back(op);
	}
	menu_option& getOption(int pos) {
		return options[pos];
	}
	menu_option& getOptionFromId(BYTE id, int *pos = NULL) {
		for (UINT i = 0; i < options.size(); i++) {
			if (options[i].id == id) {
				*pos = i;
				return options[i];
			}
		}
	}
	BYTE hittest(POINT *pt) {
		for (UINT i = 0; i < options.size(); i++) {
			if (PtInRect(&options[i].rc, *pt)) 
				return options[i].id;
		}
		return MENU_SELECTMENU;
	}
	void calcLayout() {
		int w = 160;
		int h = 432 / (options.size() / 2);
		for (UINT i = 0; i < options.size(); i++) {
			int t = (i / 2) * h;
			options[i].rc = (i % 2 == 0 ? RECT {1, t + 1, w, !t ? h : t + h - 1} : RECT {w + 1, t + 1, w * 2, !t ? h : t + h - 1});
		}
	}
	RECT getRect(int pos) {
		return options[pos].rc;
	}

	int setHover(int pos, bool hover) {
		options[pos].is_hover = hover;
		return pos;
	}

	COLORREF getBackColor(int pos) {
		return  options[pos].is_hover ? RGB(80, 80, 80) : RGB(25, 25, 25);
	}
	COLORREF getFrameColor(int pos) {
		pos;
		return RGB(0, 127, 255);
	}
	COLORREF getBkColor(int pos) {
		return options[pos].is_hover ? RGB(80, 80, 80) : RGB(25, 25, 25);
	}
	COLORREF getTextColor(int pos) {
		pos;
		return /*options[pos].is_hover ? RGB(0, 127, 255) :*/ RGB(240, 0, 0);
	}	
};
charstats_menu characterpagemenu;

class inv_active_bmp_files {
	public:
	string active_store, active_character, active_account, active_realm;
	map<string, HBITMAP> invfiles;

	inv_active_bmp_files() {};
	~inv_active_bmp_files() {
		for (auto a : invfiles)
			DeleteObject(a.second);
	}

	HBITMAP* Load(string filename) {
		if (invfiles[filename] == NULL) {
			Image* img = LoadPicture(str_to_wstr(getexedir() + "\\inv\\" + filename + ".png").c_str());
			Bitmap* pBitmap = static_cast<Bitmap*>(img->Clone());
			HBITMAP bmp = NULL;
			pBitmap->GetHBITMAP(Color(0, 0, 0), &bmp);
			invfiles[filename] = bmp;
			delete pBitmap;
			delete img;
		}
		return &invfiles[filename];
	}
	BOOL UnLoad(string filename) {
		return DeleteObject(invfiles[filename]);
	}
	void UnLoadAll() {
		for (auto a : invfiles) {
			DeleteObject(a.second);
			a.second = NULL;
		}
		invfiles.clear();
	}

	void SetActive(const ItemData* item) {
		active_realm = item->realm;
		active_account = item->account;
		active_character = item->character;
		active_store = item->store;
	}
	BOOL isActive(const ItemData* item) {
		return (item->realm == active_realm && item->account == active_account && item->character == active_character && item->store == active_store);
	}
};

VOID CALLBACK TimerProc1(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	switch (idEvent) {
		case 0:{
			SendMessage(hwnd, WM_PAINT, NULL, NULL);
			break;
		}
	}
}

BOOL CALLBACK invProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static ItemData* _g_hover_item = NULL;
	static bool _is_cursor_in_client = false;

	static BYTE characterpage = MENU_STAT;// MENU_SELECTMENU;
	static BYTE selectedsubpage = 1;

	static POINT startpoint;
	static RECT selectionrc;
	static vector<ItemData*> selectionlist;
	static vector<ItemData*> inverseselectionlist;
	static vector<ItemData*> selectionlistcpy;
	static bool isdragging = false;
	static int dragcount = 0;
	static bool selblock;

	switch (message) {
		case WM_UPDATEUISTATE:{
			static bool first = true;
			if (first) {
				first = false;
				characterpagemenu.addOption(MENU_STAT, "Character Stats");
				characterpagemenu.addOption(MENU_SKILLTREE, "Skills");
				characterpagemenu.addOption(MENU_WAYPOINT_NORM, "Normal Waypoints");
				characterpagemenu.addOption(MENU_QUEST_NORM, "Normal Quests");
				characterpagemenu.addOption(MENU_WAYPOINT_NIGHT, "Nightmare Waypoints");
				characterpagemenu.addOption(MENU_QUEST_NIGHT, "Nightmare Quests");
				characterpagemenu.addOption(MENU_WAYPOINT_HELL, "Hell Waypoints");				
				characterpagemenu.addOption(MENU_QUEST_HELL, "Hell Quests");
				characterpagemenu.calcLayout();
			}
			break;
		}
		case WM_PAINT:{
			//TODO color items (rares, uniques etc)
			//TODO color eth items
			//TODO draw sockets + gems in sockets on hover / selection
			static inv_active_bmp_files active_bmps;

			RECT rcclient = getclientrect(hwnd);		

			HDC hDC = GetDC(hwnd);

			HWND tree = GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1);

			const ItemData* item = (ItemData*)TreeView_GetItemParam(tree, TreeView_GetSelection(tree));
			CharacterData* chardata = (CharacterData*)TreeView_GetItemParam(tree, TreeView_GetSelection(tree));
			CharacterStats* charstats = &chardata->character_stats;
			if (!item) {
				HBRUSH defback = CreateSolidBrush(RGB(100, 100, 100));
				FillRect(hDC, &rcclient, defback);
				DeleteObject(defback);
				ReleaseDC(hwnd, hDC);
				active_bmps.UnLoadAll();
				break;
			}

			HDC hDCmem = CreateCompatibleDC(hDC);

			HBITMAP hbmScreen, hbmOldBitmap;
			hbmScreen = CreateCompatibleBitmap(hDC, rcclient.right, rcclient.bottom);
			hbmOldBitmap = (HBITMAP)SelectObject(hDCmem, hbmScreen);
			//////////////////////////////////////////////////			

			int childcount = TreeView_GetChildCount(tree, TreeView_GetSelection(tree));
			switch (childcount) {
				case 0:
				case 1:{
					//paint background
					Image* img = LoadPicture(str_to_wstr(getexedir() + "\\inv\\PANEL\\" + GetStorePNG(item->store, &item->invdata) + ".png").c_str());
					Bitmap* pBitmap = static_cast<Bitmap*>(img->Clone());
					HBITMAP hbackbmp = NULL;
					pBitmap->GetHBITMAP(Color(0, 0, 0), &hbackbmp);
					DeleteObject(SelectObject(hDCmem, hbackbmp));

					//manage HBITMAPS
					/*if (!active_bmps.isActive(item)) {
						active_bmps.SetActive(item);
						active_bmps.UnLoadAll();
					}*/

					//loop through all items of selected store and draw them. highlight the selected one
					string cur = item->store;
					for (int i = 0; i < 2; i++) {
						vector<ItemData>* items = get_item_store(item->realm, item->account, item->character, cur);
						if (!items->size())
							break;
						if (item->store != "inventory" && item->store != "body") i++;
						cur = cur == "inventory" ? "body" : "inventory";
						RECT rc;
						for (UINT i = 0; i < items->size(); i++) {
							string invfile = items->at(i).quality == "unique" && items->at(i).invdata.invu.size() > 0 ? items->at(i).invdata.invu
								: items->at(i).quality == "set" && items->at(i).invdata.invs.size() > 0 ? items->at(i).invdata.invs
								: items->at(i).invdata.inv;
							rc = get_item_inv_rect(&items->at(i));

							if (items->at(i).invdata.y < 0
								&& (item->invdata.x > 10 && (items->at(i).invdata.x == 4 || items->at(i).invdata.x == 5)
								|| (item->invdata.x < 11 && items->at(i).invdata.x > 10)));
							else {
								HBITMAP* hitembmp = active_bmps.Load(invfile);
								bool painted = false;
								for (auto stat : items->at(i).stats) {
									if (stat.find("Ethereal") != string::npos) {
										painted = true;
										TransparentBlitToHdc(hDCmem, *hitembmp, rc.left, rc.top, rc.right, rc.bottom, RGB(0, 0, 0), ethtransform);
										break;
									}
								}
								if (!painted)
									TransparentBlitToHdc(hDCmem, *hitembmp, rc.left, rc.top, rc.right, rc.bottom, RGB(0, 0, 0));

								//draw trade rect (pink)
								if (items->at(i).is_trade && trade::is_mytradefile()) {
									HBRUSH highlight = CreateSolidBrush(RGB(255, 0, 255));
									FrameRect(hDCmem, &rc, highlight);
									rc.left++; rc.right--; rc.top++; rc.bottom--;
									DeleteObject(highlight);
								}
								//draw hover rect (blue)
								if (_g_hover_item && &items->at(i) == _g_hover_item) {
									HBRUSH highlight = CreateSolidBrush(RGB(0, 0, 255));
									FrameRect(hDCmem, &rc, highlight);
									rc.left++; rc.right--; rc.top++; rc.bottom--;
									FrameRect(hDCmem, &rc, highlight);
									DeleteObject(highlight);
								}
								//draw selection highlight rects (green or rainbow)
								if (find(selectionlist.begin(), selectionlist.end(), &items->at(i)) != selectionlist.end()) {
									HBRUSH highlight = CreateSolidBrush(RGB(0, 255, 0));
									isdragging && dragcount > 7 ? FrameRectRainbow(hDCmem, &rc, selectionlist.size() * 6) : FrameRect(hDCmem, &rc, highlight);									
									rc.left++; rc.right--; rc.top++; rc.bottom--;
									isdragging && dragcount > 7 ? FrameRectRainbow(hDCmem, &rc, selectionlist.size() * 6) : FrameRect(hDCmem, &rc, highlight);
									rc.left++; rc.right--; rc.top++; rc.bottom--;
									DeleteObject(highlight);
								}
							}
						}
					}
					///////////////////////////////////////////////////
					//draw selection rect (rainbow)
					if (isdragging) {
						FrameRectRainbow(hDCmem, &selectionrc, 6);
						RECT trc = {selectionrc.left + 1, selectionrc.top + 1, selectionrc.right - 1, selectionrc.bottom - 1};
						FrameRectRainbow(hDCmem, &trc, 6);
					}
					//////////////////////////////////////////////////////
					BitBlt(hDC, 0, 0, img->GetWidth(), img->GetHeight(), hDCmem, 0, 0, SRCCOPY);

					SelectObject(hDCmem, hbmOldBitmap);

					DeleteObject(hbmScreen);
					DeleteObject(hbackbmp);
					delete pBitmap;
					delete img;
					break;
				}
				case 2:{
					Image* img = NULL;
					Bitmap* pBitmap = NULL;
					HBITMAP hbackbmp = NULL;

					switch (characterpage) {
						case MENU_SELECTMENU:{
							HBRUSH defback = CreateSolidBrush(RGB(100, 100, 100));
							FillRect(hDCmem, &rcclient, defback);
							DeleteObject(defback);
							for (UINT i = 0; i < characterpagemenu.getSize(); i++) {
								RECT mrc = characterpagemenu.getRect(i);

								HBRUSH backcol = CreateSolidBrush(characterpagemenu.getBackColor(i));
								FillRect(hDCmem, &mrc, backcol);
								DeleteObject(backcol);

							/*	HBRUSH framecol = CreateSolidBrush(characterpagemenu.getFrameColor(i));
								FrameRect(hDCmem, &mrc, framecol);
								DeleteObject(framecol);*/

								SetTextColor(hDCmem, characterpagemenu.getTextColor(i));
								SetBkColor(hDCmem, characterpagemenu.getBkColor(i));
								RECT textrc = {mrc.left, (mrc.bottom - mrc.top) / 2 - 7 + mrc.top, mrc.right, (mrc.bottom - mrc.top) / 2 + 5 + mrc.top};
								DrawText(hDCmem, str_to_wstr(characterpagemenu.getOption(i).text).c_str(), 
									characterpagemenu.getOption(i).text.size(), &textrc, DT_CENTER | DT_NOCLIP);
								
								HPEN outerhighlight = CreatePen(PS_SOLID, 1, RGB(135, 135, 135));
								HPEN innerhighlight = CreatePen(PS_SOLID, 1, RGB(61, 61, 61));
								HPEN innershadow = CreatePen(PS_SOLID, 1, RGB(30, 30, 30));
								HPEN outershadow = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

								if (characterpagemenu.getOption(i).has_focus) {
									if (characterpagemenu.getOption(i).is_selected) {
										SelectObject(hDCmem, innershadow);
										Rectangle(hDCmem, mrc.left + 1, mrc.top + 1, mrc.right - 1, mrc.bottom - 1);

										SelectObject(hDCmem, outershadow);
										Rectangle(hDCmem, mrc.left, mrc.top, mrc.right, mrc.bottom);
									}
									else {
										SelectObject(hDCmem, outerhighlight);
										MoveToEx(hDCmem, mrc.left + 1, mrc.top + 1, NULL);
										LineTo(hDCmem, mrc.right - 2, mrc.top + 1);
										MoveToEx(hDCmem, mrc.left + 1, mrc.top + 2, NULL);
										LineTo(hDCmem, mrc.left + 1, mrc.bottom - 2);

										SelectObject(hDCmem, innerhighlight);
										MoveToEx(hDCmem, mrc.left + 2, mrc.top + 2, NULL);
										LineTo(hDCmem, mrc.right - 3, mrc.top + 2);
										MoveToEx(hDCmem, mrc.left + 2, mrc.top + 3, NULL);
										LineTo(hDCmem, mrc.left + 2, mrc.bottom - 3);

										SelectObject(hDCmem, innershadow);
										MoveToEx(hDCmem, mrc.left + 2, mrc.bottom - 3, NULL);
										LineTo(hDCmem, mrc.right - 2, mrc.bottom - 3);
										MoveToEx(hDCmem, mrc.right - 3, mrc.top + 2, NULL);
										LineTo(hDCmem, mrc.right - 3, mrc.bottom - 3);

										SelectObject(hDCmem, outershadow);
										MoveToEx(hDCmem, mrc.left + 1, mrc.bottom - 2, NULL);
										LineTo(hDCmem, mrc.right - 1, mrc.bottom - 2);
										MoveToEx(hDCmem, mrc.right - 2, mrc.top + 1, NULL);
										LineTo(hDCmem, mrc.right - 2, mrc.bottom - 2);
										Rectangle(hDCmem, mrc.left, mrc.top, mrc.right, mrc.bottom);
									}
									if (characterpagemenu.getOption(i).has_focus) {
										SelectObject(hDCmem, outershadow);
										Rectangle(hDCmem, mrc.left + 4, mrc.top + 4, mrc.right - 4, mrc.bottom - 4);
									}
								}
								else {
									SelectObject(hDCmem, outerhighlight);
									MoveToEx(hDCmem, mrc.left, mrc.top, NULL);
									LineTo(hDCmem, mrc.right - 1, mrc.top);
									MoveToEx(hDCmem, mrc.left, mrc.top + 1, NULL);
									LineTo(hDCmem, mrc.left, mrc.bottom - 1);

									SelectObject(hDCmem, innerhighlight);
									MoveToEx(hDCmem, mrc.left + 1, mrc.top + 1, NULL);
									LineTo(hDCmem, mrc.right - 2, mrc.top + 1);
									MoveToEx(hDCmem, mrc.left + 1, mrc.top + 2, NULL);
									LineTo(hDCmem, mrc.left + 1, mrc.bottom - 2);

									SelectObject(hDCmem, innershadow);
									MoveToEx(hDCmem, mrc.left + 1, mrc.bottom - 2, NULL);
									LineTo(hDCmem, mrc.right - 1, mrc.bottom - 2);
									MoveToEx(hDCmem, mrc.right - 2, mrc.top + 1, NULL);
									LineTo(hDCmem, mrc.right - 2, mrc.bottom - 2);

									SelectObject(hDCmem, outershadow);
									MoveToEx(hDCmem, mrc.left, mrc.bottom - 1, NULL);
									LineTo(hDCmem, mrc.right, mrc.bottom - 1);
									MoveToEx(hDCmem, mrc.right - 1, mrc.top, NULL);
									LineTo(hDCmem, mrc.right - 1, mrc.bottom - 1);
								}
								
								DeleteObject(outerhighlight);
								DeleteObject(innerhighlight);
								DeleteObject(innershadow);
								DeleteObject(outershadow);								
							}
							break;
						}
						case MENU_STAT:{
							POINT pt = getclientcursorpos(hwnd);

							img = LoadPicture(str_to_wstr(getexedir() + "\\inv\\PANEL\\invchar6.png").c_str());
							pBitmap = static_cast<Bitmap*>(img->Clone());
							pBitmap->GetHBITMAP(Color(0, 0, 0), &hbackbmp);
							SelectObject(hDCmem, hbackbmp);

							SetBkMode(hDCmem, TRANSPARENT);
							SetTextColor(hDCmem, RGB(196, 196, 196));

							//name
							RECT rc = {12, 10, 180, 24};
							std::string text = chardata->character;
							DrawText(hDCmem, str_to_wstr(text).c_str(), text.size(), &rc, DT_VCENTER | DT_CENTER | DT_NOCLIP);

							//class
							rc = {194, 10, 309, 24};
							text = charstats->getCharacterClassStr();
							DrawText(hDCmem, str_to_wstr(text).c_str(), text.size(), &rc, DT_VCENTER | DT_CENTER | DT_NOCLIP);

							//str, dex, vit, nrg, life, mana, stam, lvl
							vector<int> stats = {STAT_STRENGTH, STAT_ENERGY, STAT_DEXTERITY, STAT_VITALITY,
								STAT_HITPOINTS, STAT_MAXHP, STAT_MANA, STAT_MAXMANA,
								STAT_STAMINA, STAT_MAXSTAMINA, STAT_LEVEL};
							for (UINT i = 0; i < stats.size(); i++) {
								RECT rc = charstats->getStatInvPos(stats[i]);
								std::string text = int_to_str(charstats->getStat(stats[i]));
								DrawText(hDCmem, str_to_wstr(text).c_str(), text.size(), &rc, DT_VCENTER | DT_CENTER | DT_NOCLIP);
							}

							//block chance or defense
							bool indefbox = false;
							if (pt.x < 311 && pt.x > 160 && pt.y > 192 && pt.y < 211) 
								indefbox = true;
							int totalblock = indefbox ? charstats->getStat(STAT_TOBLOCK) : charstats->getStat(STAT_ARMORCLASS);
							string blockstr = int_to_str(totalblock) + (indefbox ? "%" : "");
							RECT blockrc = charstats->getStatInvPos(STAT_ARMORCLASS);
							DrawText(hDCmem, str_to_wstr(blockstr).c_str(), blockstr.size(), &blockrc, DT_VCENTER | DT_CENTER | DT_NOCLIP);

							//experience
							rc = charstats->getStatInvPos(STAT_EXPERIENCE);
							text = uint_to_str(charstats->getStat(STAT_EXPERIENCE));
							text = FormatWithCommas(str_to_uint(text));
							DrawText(hDCmem, str_to_wstr(text).c_str(), text.size(), &rc, DT_VCENTER | DT_CENTER | DT_NOCLIP);
							vector<unsigned int> lvlexp = {500, 1500, 3750, 7875, 14175, 22680, 32886, 44396, 57715, 72144,
								90180, 112725, 140906, 176132, 220165, 275207, 344008, 430010, 537513, 671891,
								839864, 1049830, 1312287, 1640359, 2050449, 2563061, 3203826, 3902260, 4663553, 5493363,
								6397855, 7383752, 8458379, 9629723, 10906488, 12298162, 13815086, 15468534, 17270791, 19235252,
								21376515, 23710491, 26254525, 29027522, 32050088, 35344686, 38935798, 42850109, 47116709, 51767302,
								56836449, 62361819, 68384473, 74949165, 82104680, 89904191, 98405658, 107672256, 117772849, 128782495,
								140783010, 153863570, 168121381, 183662396, 200602101, 219066380, 239192444, 261129853, 285041630, 311105466,
								339515048, 370481492, 404234916, 441026148, 481128591, 524840254, 572485967, 624419793, 681027665, 742730244,
								809986056, 883294891, 963201521, 1050299747, 1145236814, 1248718217, 1361512946, 1484459201, 1618470619, 1764543065,
								1923762030, 2097310703, 2286478756, 2492671933, 2717422497, 2962400612, 3229426756, 3520485254};
							rc = {194, 44, 309, 64};
							bool inexpbox = false;
							if (PtInRect(&rc, pt))
								inexpbox = true;
							if (charstats->getStat(STAT_LEVEL) > 0) {
								if (charstats->getStat(STAT_LEVEL) >= 99)
									text = "0";
								else
									text = uint_to_str(inexpbox ? lvlexp[charstats->getStat(STAT_LEVEL) - 1] - charstats->getStat(STAT_EXPERIENCE) : lvlexp[charstats->getStat(STAT_LEVEL) - 1]);
							}
							else
								text = "";
							text = FormatWithCommas(str_to_uint(text));
							DrawText(hDCmem, str_to_wstr(text).c_str(), text.size(), &rc, DT_VCENTER | DT_CENTER | DT_NOCLIP);

							//resistances
							bool inresbox = false;
							if (pt.x < 311 && pt.x > 172 && pt.y > 331 && pt.y < 422)
								inresbox = true;
							int fire = charstats->getStat(STAT_FIRERESIST) - 100;
							int cold = charstats->getStat(STAT_COLDRESIST) - 100;
							int light = charstats->getStat(STAT_LIGHTRESIST) - 100;
							int poison = charstats->getStat(STAT_POISONRESIST) - 100;
							int	maxfire = charstats->getStat(STAT_MAXFIRERESIST) + 75;
							maxfire = inresbox ? fire : maxfire > 95 ? 95 : maxfire;
							int maxcold = charstats->getStat(STAT_MAXCOLDRESIST) + 75;
							maxcold = inresbox ? cold : maxcold > 95 ? 95 : maxcold;
							int maxlight = charstats->getStat(STAT_MAXLIGHTRESIST) + 75;
							maxlight = inresbox ? light : maxlight > 95 ? 95 : maxlight;
							int maxpoison = charstats->getStat(STAT_MAXPOISONRESIST) + 75;
							maxpoison = inresbox ? poison : maxpoison > 95 ? 95 : maxpoison;
							fire = fire > maxfire ? maxfire : fire;
							cold = cold > maxcold ? maxcold : cold;
							light = light > maxlight ? maxlight : light;
							poison = poison > maxpoison ? maxpoison : poison;
							RECT firerc = charstats->getStatInvPos(STAT_FIRERESIST);
							RECT coldrc = charstats->getStatInvPos(STAT_COLDRESIST);
							RECT lightrc = charstats->getStatInvPos(STAT_LIGHTRESIST);
							RECT poisonrc = charstats->getStatInvPos(STAT_POISONRESIST);
							SetTextColor(hDCmem, inresbox ? RGB(176, 68, 52) : maxfire <= fire ? RGB(148, 128, 100) : RGB(196, 196, 196));
							DrawText(hDCmem, str_to_wstr(int_to_str(fire)).c_str(), int_to_str(fire).size(), &firerc, DT_VCENTER | DT_CENTER | DT_NOCLIP);
							SetTextColor(hDCmem, inresbox ? RGB(80, 80, 172) : maxcold <= cold ? RGB(148, 128, 100) : RGB(196, 196, 196));
							DrawText(hDCmem, str_to_wstr(int_to_str(cold)).c_str(), int_to_str(cold).size(), &coldrc, DT_VCENTER | DT_CENTER | DT_NOCLIP);
							SetTextColor(hDCmem, inresbox ? RGB(216, 184, 100) : maxlight <= light ? RGB(148, 128, 100) : RGB(196, 196, 196));
							DrawText(hDCmem, str_to_wstr(int_to_str(light)).c_str(), int_to_str(light).size(), &lightrc, DT_VCENTER | DT_CENTER | DT_NOCLIP);
							SetTextColor(hDCmem, inresbox ? RGB(24, 252, 0) : maxpoison <= poison ? RGB(148, 128, 100) : RGB(196, 196, 196));
							DrawText(hDCmem, str_to_wstr(int_to_str(poison)).c_str(), int_to_str(poison).size(), &poisonrc, DT_VCENTER | DT_CENTER | DT_NOCLIP);
							break;
						}
						case MENU_SKILLTREE:{
							img = LoadPicture(str_to_wstr(getexedir() + "\\inv\\PANEL\\skilltree" + int_to_str(charstats->getStat(-1)) + int_to_str(selectedsubpage) + ".png").c_str());
							pBitmap = static_cast<Bitmap*>(img->Clone());
							pBitmap->GetHBITMAP(Color(0, 0, 0), &hbackbmp);
							SelectObject(hDCmem, hbackbmp);
							//TODO finish skill tree
							break;
						}
						case MENU_WAYPOINT_NORM:
						case MENU_WAYPOINT_NIGHT:
						case MENU_WAYPOINT_HELL: {
							img = LoadPicture(str_to_wstr(getexedir() + "\\inv\\PANEL\\waygateback.png").c_str());
							pBitmap = static_cast<Bitmap*>(img->Clone());
							pBitmap->GetHBITMAP(Color(0, 0, 0), &hbackbmp);
							SelectObject(hDCmem, hbackbmp);

							Image* imgwppage = LoadPicture(str_to_wstr(getexedir() + "\\inv\\PANEL\\waypointpage" + int_to_str(selectedsubpage) + ".png").c_str());
							RECT rc = {7, 3, (LONG)imgwppage->GetWidth() + 7, (LONG)imgwppage->GetHeight() + 3};
							DrawPicture(hDCmem, imgwppage, &rc);

							Image* icon = LoadPicture(str_to_wstr(getexedir() + "\\inv\\PANEL\\waygateicons.png").c_str());

							int x = 17;
							auto h = [](int m) -> int {return 36 * (m + 1) + 22; };
							RECT rcicon = {0};
							RECT rctext = {0};
							vector<string> wpnames = {
								"Rogue Encampment", "Cold Plains", "Stony Field", "Dark Wood", "Black Marsh", "Outer Cloister", "Jail level 1", "Inner Cloister", "Catacombs level 2",
								"Lut Gholein", "Sewers level 2", "Dry Hills", "Halls of the Dead level 2", "Far Oasis", "Lost City", "Palace Cellar level 1", "Arcane Sanctuary", "Canyon of the Magi",
								"Kurast Docks", "Spider Forest", "Great Marsh", "Flayer Jungle", "Lower Kurast", "Kurast Bazaar", "Upper Kurast", "Travincal", "Durance of Hate level 2",
								"Pandemonium Fortress", "City of the Damned", "River of Flames",
								"Harrogath", "Frigid Highlands", "Arreat Plateau", "Crystalline Passage", "Halls of Pain", "Glacial Trail", "Frozen Tundra", "The Ancients' Way", "Worldstone Keep level 2"
							};

							if (!(characterpage == MENU_WAYPOINT_NORM ? chardata->wp_norm.size() >= 39
								: characterpage == MENU_WAYPOINT_NIGHT ? chardata->wp_night.size() >= 39
								: characterpage == MENU_WAYPOINT_HELL ? chardata->wp_hell.size() >= 39
								: false))
								selectedsubpage = 0;
							switch (selectedsubpage) {
								case 1:{
									for (UINT i = 0; i < 9; i++) {
										if (characterpage == MENU_WAYPOINT_NORM ? chardata->wp_norm[i] == '1'
											: characterpage == MENU_WAYPOINT_NIGHT ? chardata->wp_night[i] == '1'
											: characterpage == MENU_WAYPOINT_HELL ? chardata->wp_hell[i] == '1'
											: false) {
											rcicon = {x, h(i), (LONG)icon->GetWidth() + x, (LONG)icon->GetHeight() + h(i)};
											DrawPicture(hDCmem, icon, &rcicon);

											rctext = {80, h(i) + 8, (LONG)img->GetWidth(), (LONG)img->GetHeight() + 8};
											DrawText(hDCmem, str_to_wstr(wpnames[i]).c_str(), wpnames[i].size(), &rctext, DT_VCENTER | DT_NOCLIP);
										}
									}
									break;
								}
								case 2:{
									for (UINT i = 9; i < 18; i++) {
										if (characterpage == MENU_WAYPOINT_NORM ? chardata->wp_norm[i] == '1'
											: characterpage == MENU_WAYPOINT_NIGHT ? chardata->wp_night[i] == '1'
											: characterpage == MENU_WAYPOINT_HELL ? chardata->wp_hell[i] == '1'
											: false) {
											rcicon = {x, h(i - (9 * (selectedsubpage - 1))), (LONG)icon->GetWidth() + x, (LONG)icon->GetHeight() + h(i - (9 * (selectedsubpage - 1)))};
											DrawPicture(hDCmem, icon, &rcicon);

											rctext = {80, h(i - (9 * (selectedsubpage - 1))) + 8, (LONG)img->GetWidth(), (LONG)img->GetHeight() + 8};
											DrawText(hDCmem, str_to_wstr(wpnames[i]).c_str(), wpnames[i].size(), &rctext, DT_VCENTER | DT_NOCLIP);
										}
									}
									break;
								}
								case 3:{
									for (UINT i = 18; i < 27; i++) {
										if (characterpage == MENU_WAYPOINT_NORM ? chardata->wp_norm[i] == '1'
											: characterpage == MENU_WAYPOINT_NIGHT ? chardata->wp_night[i] == '1'
											: characterpage == MENU_WAYPOINT_HELL ? chardata->wp_hell[i] == '1'
											: false) {
											rcicon = {x, h(i - (9 * (selectedsubpage - 1))), (LONG)icon->GetWidth() + x, (LONG)icon->GetHeight() + h(i - (9 * (selectedsubpage - 1)))};
											DrawPicture(hDCmem, icon, &rcicon);

											rctext = {80, h(i - (9 * (selectedsubpage - 1))) + 8, (LONG)img->GetWidth(), (LONG)img->GetHeight() + 8};
											DrawText(hDCmem, str_to_wstr(wpnames[i]).c_str(), wpnames[i].size(), &rctext, DT_VCENTER | DT_NOCLIP);
										}
									}
									break;
								}
								case 4:{
									for (UINT i = 27; i < 30; i++) {
										if (characterpage == MENU_WAYPOINT_NORM ? chardata->wp_norm[i] == '1'
											: characterpage == MENU_WAYPOINT_NIGHT ? chardata->wp_night[i] == '1'
											: characterpage == MENU_WAYPOINT_HELL ? chardata->wp_hell[i] == '1'
											: false) {
											rcicon = {x, h(i - (9 * (selectedsubpage - 1))), (LONG)icon->GetWidth() + x, (LONG)icon->GetHeight() + h(i - (9 * (selectedsubpage - 1)))};
											DrawPicture(hDCmem, icon, &rcicon);

											rctext = {80, h(i - (9 * (selectedsubpage - 1))) + 8, (LONG)img->GetWidth(), (LONG)img->GetHeight() + 8};
											DrawText(hDCmem, str_to_wstr(wpnames[i]).c_str(), wpnames[i].size(), &rctext, DT_VCENTER | DT_NOCLIP);
										}
									}
									break;
								}
								case 5:{
									for (UINT i = 30; i < 39; i++) {
										if (characterpage == MENU_WAYPOINT_NORM ? chardata->wp_norm[i] == '1'
											: characterpage == MENU_WAYPOINT_NIGHT ? chardata->wp_night[i] == '1'
											: characterpage == MENU_WAYPOINT_HELL ? chardata->wp_hell[i] == '1'
											: false) {
											rcicon = {x, h(i - 30), (LONG)icon->GetWidth() + x, (LONG)icon->GetHeight() + h(i - 30)};
											DrawPicture(hDCmem, icon, &rcicon);

											rctext = {80, h(i - 30) + 8, (LONG)img->GetWidth(), (LONG)img->GetHeight() + 8};
											DrawText(hDCmem, str_to_wstr(wpnames[i]).c_str(), wpnames[i].size(), &rctext, DT_VCENTER | DT_NOCLIP);
										}
									}
									break;
								}
							}
							break;
						}
						case MENU_QUEST_NORM:
						case MENU_QUEST_NIGHT:
						case MENU_QUEST_HELL:{
							img = LoadPicture(str_to_wstr(getexedir() + "\\inv\\PANEL\\questbackground.png").c_str());
							pBitmap = static_cast<Bitmap*>(img->Clone());
							pBitmap->GetHBITMAP(Color(0, 0, 0), &hbackbmp);
							SelectObject(hDCmem, hbackbmp);

							Image* imgwppage = LoadPicture(str_to_wstr(getexedir() + "\\inv\\PANEL\\waypointpage" + int_to_str(selectedsubpage) + ".png").c_str());
							RECT rc = {7, 3, imgwppage->GetWidth() + 7, imgwppage->GetHeight() + 3};
							DrawPicture(hDCmem, imgwppage, &rc);
							//TODO finish quests
							switch (selectedsubpage) {
								case 1:{
									for (UINT i = 0; i < 6; i++) {

									}
									break;
								}
								case 2:{
									for (UINT i = 0; i < 6; i++) {

									}
									break;
								}
								case 3:{
									for (UINT i = 0; i < 6; i++) {

									}
									break;
								}
								case 4:{
									for (UINT i = 0; i < 6; i++) {

									}
									break;
								}
								case 5:{
									for (UINT i = 0; i < 6; i++) {

									}
									break;
								}
							}
							break;
						}
					}
					/////////////////////////////////////////////////////
					BitBlt(hDC, 0, 0, 320, 432, hDCmem, 0, 0, SRCCOPY);

					SelectObject(hDCmem, hbmOldBitmap);

					DeleteObject(hbmScreen);
					if (hbackbmp)
						DeleteObject(hbackbmp);
					if (pBitmap)
						delete pBitmap;
					if (img)
						delete img;
					break;
				}
			}

			DeleteDC(hDCmem);
			ReleaseDC(hwnd, hDC);

			break;
		}
		case WM_ERASEBKGND:{
			return 0;
		}
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:{
			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			HWND htv = GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1);

			isdragging = true;
			SetCapture(hwnd);
			dragcount = 0;
			startpoint = pt; 
			if (GetAsyncKeyState(VK_CONTROL) >= 0) {
				inverseselectionlist.clear();
				selectionlistcpy.clear();
			}

			int childcount = TreeView_GetChildCount(htv, TreeView_GetSelection(htv));
			switch (childcount) {
				case 0:
				case 1:{
					if (_g_hover_item) {
						SetFocus(htv);
						if (GetAsyncKeyState(VK_CONTROL) < 0) {
							auto it = find(selectionlist.begin(), selectionlist.end(), _g_hover_item);
							if (it != selectionlist.end()) {
								selectionlist.erase(it);
								if (selectionlist.size()) {
									selblock = true;
									TreeView_SelectItem(htv, TreeView_GetHTVofD2item(selectionlist[selectionlist.size() - 1]));
									selblock = false;
								}
								else if (childcount == 0)
									TreeView_SelectItem(htv, TreeView_GetParent(htv, TreeView_GetSelection(htv)));
								break;
							}
						}
						else {
							selectionlist.clear();
						}
						selectionlist.push_back(_g_hover_item);
						selblock = true;
						TreeView_SelectItem(htv, TreeView_GetHTVofD2item(_g_hover_item));
						selblock = false;
						RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
					}
					else {
						ItemData* item = (ItemData*)TreeView_GetItemParam(GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1), TreeView_GetSelection(GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1)));
						if (!item) break;
					
						//clear selections when no item clicked
						if (GetAsyncKeyState(VK_CONTROL) >= 0) {
							selectionlist.clear();
							if (childcount == 0)
								TreeView_SelectItem(htv, TreeView_GetParent(htv, TreeView_GetSelection(htv)));
						}

						if (item->store == "body" || item->store == "inventory") {
							//item swap buttons pressed
							RECT left_wep_swap = {15, 23, 81, 44}, right_wep_swap = {246, 23, 312, 44};
							if (PtInRect(&left_wep_swap, pt) || PtInRect(&right_wep_swap, pt)) {
								int l = 0, r = 0, t = 0, b = 0;
								vector<ItemData>* items = get_item_store(item->realm, item->account, item->character, "body");
								for (UINT i = 0; i < items->size(); i++) {
									if (items->at(i).invdata.x == 4) l = 4;
									else if (items->at(i).invdata.x == 5) r = 5;
									else if (items->at(i).invdata.x == 11) t = 11;
									else if (items->at(i).invdata.x == 12) b = 12;
								}

								int s =
									item->invdata.x == 4 ? t ? t : b ? b : 4 :
									item->invdata.x == 5 ? b ? b : t ? t : 5 :
									item->invdata.x == 11 ? l ? l : r ? r : 11 :
									item->invdata.x == 12 ? r ? r : l ? l : 12 :
									l ? t ? t : b ? b : l :
									r ? b ? b : t ? t : r :
									t ? l ? l : r ? r : t :
									b ? r ? r : l ? l : b : -1;

								if (s == -1) break;

								for (UINT i = 0; i < items->size(); i++) {
									if (items->at(i).invdata.x == s) {
										selblock = true;
										TreeView_SelectItem(htv, TreeView_GetHTVofD2item(&items->at(i)));
										selblock = false;
										break;
									}
								}
							}
						}
					}
					if (message == WM_LBUTTONDBLCLK) {
						if (_g_hover_item) {
							if (_g_hover_item->is_trade)
								trade::remove(_g_hover_item);
							else
								trade::add(_g_hover_item);
							RECT rc = getclientrect(hwnd);
							RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
						}
					}
					break;
				}
				case 2:{
					switch (characterpage) {
						case MENU_SELECTMENU:{
							characterpage = characterpagemenu.hittest(&pt);
							selectedsubpage = 1;
							RECT rc = getclientrect(hwnd);
							RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
							break;
						}
						case MENU_STAT:{
							//close button
							RECT closerc = {128, 389, 159, 420};
							if (PtInRect(&closerc, pt)) {
								characterpage = MENU_SELECTMENU;
								RECT rc = getclientrect(hwnd);
								RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
							}
							break;
						}
						case MENU_SKILLTREE:{
							//hitcheck for tab buttons
							if (pt.x > 229 && pt.y > 107) {
								if (pt.y < 214)
									selectedsubpage = 1;
								else if (pt.y >= 214 && pt.y < 323)
									selectedsubpage = 2;
								else if (pt.y >= 323)
									selectedsubpage = 3;
								else selectedsubpage = 1;
								RECT rc = getclientrect(hwnd);
								RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
							}
							//hitcheck for close buttons
							int t = ((CharacterData*)TreeView_GetItemParam(htv, TreeView_GetSelection(htv)))->character_stats.getStat(-1);
							int v = t * 10 + selectedsubpage;
							vector<int> left = {1, 12, 13, 21, 23, 31, 33, 42, 61}, center = {2, 32, 63}, right = {3, 11, 22, 41, 43, 51, 52, 53, 62};
							RECT closerc = find(left.begin(), left.end(), v) != left.end() ? RECT {15, 385, 46, 416}
								: find(center.begin(), center.end(), v) != center.end() ? RECT {100, 385, 131, 416}
								: find(right.begin(), right.end(), v) != right.end() ? RECT {171, 385, 202, 416}
							: RECT {0};
							if (PtInRect(&closerc, pt)) {
								characterpage = MENU_SELECTMENU;
								RECT rc = getclientrect(hwnd);
								RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
							}
							break;
						}
						case MENU_WAYPOINT_NORM:
						case MENU_WAYPOINT_NIGHT:
						case MENU_WAYPOINT_HELL:{
							//hitcheck for tab buttons
							if (pt.y < 30) {
								if (pt.x < 68)
									selectedsubpage = 1;
								else if (pt.x >= 68 && pt.x < 130)
									selectedsubpage = 2;
								else if (pt.x >= 130 && pt.x < 192)
									selectedsubpage = 3;
								else if (pt.x >= 192 && pt.x < 254)
									selectedsubpage = 4;
								else if (pt.x >= 254)
									selectedsubpage = 5;
								else selectedsubpage = 1;
								RECT rc = getclientrect(hwnd);
								RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
							}
							//close button
							RECT closerc = {272, 386, 303, 417};
							if (PtInRect(&closerc, pt)) {
								characterpage = MENU_SELECTMENU;
								RECT rc = getclientrect(hwnd);
								RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
							}
							break;
						}
						case MENU_QUEST_NORM:
						case MENU_QUEST_NIGHT:
						case MENU_QUEST_HELL:{
							//hitcheck for tab buttons
							if (pt.y < 30) {
								if (pt.x < 68)
									selectedsubpage = 1;
								else if (pt.x >= 68 && pt.x < 130)
									selectedsubpage = 2;
								else if (pt.x >= 130 && pt.x < 192)
									selectedsubpage = 3;
								else if (pt.x >= 192 && pt.x < 254)
									selectedsubpage = 4;
								else if (pt.x >= 254)
									selectedsubpage = 5;
								else selectedsubpage = 1;
								RECT rc = getclientrect(hwnd);
								RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
							}
							RECT closerc = {278, 391, 309, 422};
							if (PtInRect(&closerc, pt)) {
								characterpage = MENU_SELECTMENU;
								RECT rc = getclientrect(hwnd);
								RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
							}
							break;
						}
					}
					break;
				}
			}
			break;
		}
		case WM_LBUTTONUP:{
			if (isdragging) {
				ReleaseCapture();
				isdragging = false;
				startpoint = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
				selectionrc = {0, 0, 0, 0};
				selectionlistcpy = selectionlist;
				KillTimer(hwnd, 0);
				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case WM_RBUTTONDOWN:{
			HWND htv = GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1);
			int childcount = TreeView_GetChildCount(htv, TreeView_GetSelection(htv));
			if (childcount < 2) {
				//open cube items
				if (_g_hover_item && _g_hover_item->icode.code == "box") {
					HWND htv = GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1);
					HTREEITEM selection = TreeView_GetSelection(htv);
					HTREEITEM parent = TreeView_GetChild(htv, selection) == NULL ? TreeView_GetParent(htv, selection) : selection;
					parent = TreeView_GetParent(htv, parent);
					HTREEITEM first = TreeView_GetChild(htv, parent);
					HTREEITEM cube = TreeView_FindSibling(htv, first, "cube");
					first = TreeView_GetChild(htv, cube);
					TreeView_SelectItem(htv, cube ? first : parent);
				}
				else {
					//context menu
					if (!_g_hover_item) break;
					ItemData* item = _g_hover_item;

					selblock = selectionlist.size() > 1 ? true : false;
					TreeView_SelectItem(GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1), TreeView_GetHTVofD2item(item));
					selblock = false;

					HMENU menu = CreatePopupMenu();
					enum {
						search_name = 1,
						search_type,
						search_sub_type,
						search_base_type,
						add_to_trade_file,
						remove_from_trade_file,
						open_stats_new_wnd
					};
					if (selectionlist.size() > 1) {
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, search_name, str_to_wstr("Search 'these' Item Names (" + int_to_str(selectionlist.size()) + ")").c_str());
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, search_type, str_to_wstr("Search 'these' Item Types (" + int_to_str(selectionlist.size()) + ")").c_str());
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, search_sub_type, str_to_wstr("Search 'these' Item Sub-Types (" + int_to_str(selectionlist.size()) + ")").c_str());
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, search_base_type, str_to_wstr("Search 'these' Item Base-Types (" + int_to_str(selectionlist.size()) + ")").c_str());
						InsertMenu(menu, (UINT)-1, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
						if (trade::is_mytradefile()) {							
							InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, add_to_trade_file, str_to_wstr("Add 'these' Items to trade file (" + int_to_str(selectionlist.size()) + ")").c_str());
							InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, remove_from_trade_file, str_to_wstr("Remove 'these' Items from trade file (" + int_to_str(selectionlist.size()) + ")").c_str());
						}
						InsertMenu(menu, (UINT)-1, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, open_stats_new_wnd, str_to_wstr("Show 'these' Items in New Window (" + int_to_str(selectionlist.size()) + ")").c_str());
					}
					else {
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, search_name, str_to_wstr("Search 'this' Item Name (" + item->name + ")").c_str());
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, search_type, str_to_wstr("Search 'this' Item Type (" + item->icode.type + ")").c_str());
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, search_sub_type, str_to_wstr("Search 'this' Item Sub-Type (" + item->icode.subtype + ")").c_str());
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, search_base_type, str_to_wstr("Search 'this' Item Base-Type (" + item->icode.basetype + ")").c_str());
						InsertMenu(menu, (UINT)-1, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
						if (trade::is_mytradefile()) {
							if (!trade::is_itemcountmax(item))
								InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, add_to_trade_file, str_to_wstr("Add 'this' Item to trade file (" + item->name + ")").c_str());
							if (trade::findtradeitem(item))
								InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, remove_from_trade_file, str_to_wstr("Remove 'this' Item from trade file (" + item->name + ")").c_str());
						}
						InsertMenu(menu, (UINT)-1, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
						InsertMenu(menu, (UINT)-1, MF_BYCOMMAND | MF_STRING | MF_ENABLED, open_stats_new_wnd, L"Show 'this' Item in New Window");
					}
					POINT pt = getcursorpos();
					UINT clicked = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_VERNEGANIMATION, pt.x, pt.y, NULL, hwnd, NULL);
					switch (clicked) {
						case search_name:{
							if (!selectionlist.size()) break;

							g_TAB.selectTab(1);

							//remove duplicates
							vector<string> nameslist;
							for (UINT i = 0; i < selectionlist.size(); i++) {
								if (find(nameslist.begin(), nameslist.end(), selectionlist[i]->name) == nameslist.end())
									nameslist.push_back(selectionlist[i]->name);
							}

							string names = nameslist[0];
							for (UINT i = 1; i < nameslist.size(); i++) {
								names += "|" + nameslist[i];
							}

							ComboBox_SetText(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHNAME), str_to_wstr(names).c_str());
							SetWindowText(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHSTATS), L"");
							if (Button_GetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDBTN)) == BST_CHECKED) {
								Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDBTN), BST_UNCHECKED);
								SendMessage(g_TAB.getTab(1).wnd, WM_COMMAND, IDC_ADVANCEDBTN, NULL);
							}
							SendMessage(g_TAB.getTab(1).wnd, WM_COMMAND, MAKEWPARAM(IDC_SEARCHNAME, CBN_EDITCHANGE), NULL);
							SetFocus(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHRESULTS));

							break;
						}
						case search_type:
						case search_sub_type:
						case search_base_type:{
							g_TAB.selectTab(1);
							ComboBox_SetText(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHNAME), L"");
							SetWindowText(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHSTATS), L"");
							if (Button_GetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDBTN)) == BST_UNCHECKED) {
								Button_SetCheck(GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDBTN), BST_CHECKED);
								SendMessage(g_TAB.getTab(1).wnd, WM_COMMAND, IDC_ADVANCEDBTN, NULL);
							}
							SendMessage(g_TAB.getTab(1).wnd, WM_COMMAND, IDC_ADVANCEDALLQUALITY, NULL);

							HWND tree = GetDlgItem(g_TAB.getTab(1).wnd, IDC_ADVANCEDCHARFILTER);
							HTREEITEM hti = TreeView_GetRoot(tree);
							TreeView_SetCheckStateForAllChildren(tree, hti, FALSE);
							TreeView_SetCheckState(tree, hti, TRUE);
							TreeView_SetCheckStateForAllChildren(tree, TreeView_GetChild(tree, hti), TRUE);

							for (auto item : selectionlist) {
								vector<string> hier = {
									"All Items",
									clicked <= search_base_type ? item->icode.basetype : "",
									clicked <= search_sub_type ? item->icode.subtype : "",
									clicked <= search_type ? item->icode.tier == 1 ? "Normal" : item->icode.tier == 2 ? "Exceptional" : item->icode.tier == 3 ? "Elite" : "" : "",
									clicked <= search_type ? item->icode.code : ""
								};
								hier.erase(std::remove(hier.begin(), hier.end(), ""), hier.end());
								for (UINT i = 0; i < hier.size(); i++) {
									if (TreeView_GetChildCount(tree, hti) == 0) {
										HTREEITEM current = hti;
										do {
											ItemCode* tcode = (ItemCode*)TreeView_GetItemParam(tree, current);
											if (tcode && tcode->code == hier[i]) {
												hti = current;
												break;
											}
										} while ((current = TreeView_GetNextSibling(tree, current)) != NULL);
									}
									else
										hti = TreeView_FindSibling(tree, hti, hier[i]);
									if (i == hier.size() - 1) {
										if (hti) {
											TreeView_SetCheckState(tree, hti, TRUE);
											TreeView_SetCheckStateForAllChildren(tree, TreeView_GetChild(tree, hti), TRUE);
										}//else failed
										break;
									}
									hti = TreeView_GetChild(tree, hti);
								}
								hti = TreeView_GetRoot(tree);
							}

							SendMessage(g_TAB.getTab(1).wnd, WM_COMMAND, MAKEWPARAM(IDC_SEARCHNAME, CBN_EDITCHANGE), NULL);
							SetFocus(GetDlgItem(g_TAB.getTab(1).wnd, IDC_SEARCHRESULTS));

							break;
						}
						case add_to_trade_file:{
							trade::add(selectionlist);
							break;
						}
						case remove_from_trade_file:{
							trade::remove(selectionlist);
							break;
						}
						case open_stats_new_wnd:{
							for (auto a : selectionlist)
								open_item_in_new_wnd(a);

							break;
						}
					}

					DestroyMenu(menu);
				}
			}
			break;
		}
		case WM_MOUSEMOVE:{
			static ItemData* previtem = NULL;

			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			
			if (isdragging) {
				dragcount++;
				RECT rcclient = getclientrect(hwnd);
				selectionrc.left = min(startpoint.x, pt.x); selectionrc.left = rcclient.left > selectionrc.left ? rcclient.left : selectionrc.left;
				selectionrc.right = max(startpoint.x, pt.x); selectionrc.right = rcclient.right < selectionrc.right ? rcclient.right : selectionrc.right;
				selectionrc.top = min(startpoint.y, pt.y); selectionrc.top = rcclient.top > selectionrc.top ? rcclient.top : selectionrc.top;
				selectionrc.bottom = max(startpoint.y, pt.y); selectionrc.bottom = rcclient.bottom < selectionrc.bottom ? rcclient.bottom : selectionrc.bottom;
				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			}

			if (!_is_cursor_in_client) {
				_is_cursor_in_client = true;
				TRACKMOUSEEVENT tme = {0};
				tme.cbSize = sizeof(tme);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hwnd;
				TrackMouseEvent(&tme);
			}

			HWND htv = GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1);
			if (GetFocus() == GetDlgItem(g_TAB.getTab(0).wnd, IDC_STATVIEW))
				SetFocus(NULL);

			int childcount = TreeView_GetChildCount(htv, TreeView_GetSelection(htv));
			switch (childcount) {
				case 0:
				case 1:{
					ItemData* item = (ItemData*)TreeView_GetItemParam(htv, TreeView_GetSelection(htv));
					if (!item) break;

					//selection rect
					if (isdragging && dragcount > 7) {
						_g_hover_item = NULL;

						string cur = item->store;
						for (int j = 0; j < 2; j++) {
							if (item->store != "inventory" && item->store != "body") j++;
							string temp = cur;
							cur = cur == "inventory" ? "body" : cur == "body" ? "inventory" : cur;
							vector<ItemData>* storelist = get_item_store(item->realm, item->account, item->character, temp);
							for (UINT i = 0; i < storelist->size(); i++) {
								if (GetAsyncKeyState(VK_CONTROL) >= 0) {
									auto it = find(selectionlist.begin(), selectionlist.end(), &storelist->at(i));
									if (is_item_in_selrc(&storelist->at(i), &selectionrc)) {
										if (it == selectionlist.end())
											selectionlist.push_back(&storelist->at(i));
									}
									else {
										if (it != selectionlist.end())
											selectionlist.erase(it);
									}
								}
								else {
									auto it = find(inverseselectionlist.begin(), inverseselectionlist.end(), &storelist->at(i));
									if (is_item_in_selrc(&storelist->at(i), &selectionrc)) {
										if (it == inverseselectionlist.end())
											inverseselectionlist.push_back(&storelist->at(i));
									}
									else {
										if (it != inverseselectionlist.end())
											inverseselectionlist.erase(it);
									}
								}
							}
						}
						if (GetAsyncKeyState(VK_CONTROL) >= 0)
							selectionlistcpy = selectionlist;
						else {
							selectionlist = selectionlistcpy;
							for (UINT i = 0; i < inverseselectionlist.size(); i++) {
								auto it = find(selectionlist.begin(), selectionlist.end(), inverseselectionlist[i]);
								if (it == selectionlist.end())
									selectionlist.push_back(inverseselectionlist[i]);
								else
									selectionlist.erase(it);
							}
						}

						if (dragcount == 8)
							SetTimer(hwnd, 0, 8, TimerProc1);
						//SendMessage(hwnd, WM_PAINT, 0, 0);
					}
					else {
						string cur = item->store;
						for (int i = 0; i < 2; i++) {
							if (item->store != "inventory" && item->store != "body") i++;
							string temp = cur;
							cur = cur == "inventory" ? "body" : cur == "body" ? "inventory" : cur;
							POINT loc = get_item_inv_loc_from_pt(temp, pt, item->invdata);
							if (loc.x > -1) {
								i++;
								_g_hover_item = get_item_from_loc(item->realm, item->account, item->character, temp, loc);
								if (_g_hover_item != previtem) {
									update_stat_view(GetDlgItem(g_TAB.getTab(0).wnd, IDC_STATVIEW), _g_hover_item);
									SendMessage(hwnd, WM_PAINT, 0, 0);
								}
							}
							else if (i) {
								_g_hover_item = NULL;
								if (previtem) {
									if (TreeView_IsSelectedD2Item())
										update_stat_view(GetDlgItem(g_TAB.getTab(0).wnd, IDC_STATVIEW), item);
									SendMessage(hwnd, WM_PAINT, 0, 0);
								}
								previtem = NULL;
								break;
							}
							previtem = _g_hover_item;
						}
					}
					break;
				}
				case 2:{
					switch (characterpage) {
						case MENU_SELECTMENU:{
							//set hover state for all menu items
							int newhover = -1;
							for (UINT i = 0; i < characterpagemenu.getSize(); i++) {
								RECT menurect = characterpagemenu.getRect(i);
								if (PtInRect(&menurect, pt))
									newhover = characterpagemenu.setHover(i, true);
								else
									characterpagemenu.setHover(i, false);
							}
							if (newhover != characterpagemenu.last_hover) {
								RECT rc = getclientrect(hwnd);
								RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
							}
							characterpagemenu.last_hover = newhover;
							break;
						}
						case MENU_STAT:{
							//paint resistances
							static bool inresbox = false;
							if (pt.x < 311 && pt.x > 172 && pt.y > 331 && pt.y < 422) {
								if (!inresbox) {
									inresbox = true;
									RECT rc = getclientrect(hwnd);
									RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
								}
							}
							else if (inresbox) {
								inresbox = false;
								RECT rc = getclientrect(hwnd);
								RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
							}
							//paint block chance
							static bool indefbox = false;
							if (pt.x < 311 && pt.x > 160 && pt.y > 192 && pt.y < 211) {
								if (!indefbox) {
									indefbox = true;
									RECT rc = getclientrect(hwnd);
									RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
								}
							}
							else if (indefbox) {
								indefbox = false;
								RECT rc = getclientrect(hwnd);
								RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
							}
							//paint exp box
							static bool inexpbox = false;
							RECT rc = {194, 44, 309, 64};
							if (PtInRect(&rc, pt)) {
								if (!inexpbox) {
									inexpbox = true;
									RECT rc = getclientrect(hwnd);
									RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
								}
							}
							else if (inexpbox) {
								inexpbox = false;
								RECT rc = getclientrect(hwnd);
								RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE);
							}
							break;
						}
					}
					break;
				}
			}
			break;
		}
		case WM_MOUSELEAVE:{
			_is_cursor_in_client = false;

			HWND htv = GetDlgItem(g_TAB.getTab(0).wnd, IDC_TREE1);

			int childcount = TreeView_GetChildCount(htv, TreeView_GetSelection(htv));
			switch (childcount) {
				case 0:
				case 1:{
					ItemData* item = (ItemData*)TreeView_GetItemParam((htv), TreeView_GetSelection(htv));
					_g_hover_item = NULL;

					update_stat_view(GetDlgItem(g_TAB.getTab(0).wnd, IDC_STATVIEW), childcount == 1 ? NULL : item);
					SendMessage(hwnd, WM_PAINT, 0, 0);
					break;
				}
				case 2:{
					for (UINT i = 0; i < characterpagemenu.getSize(); i++) 
						characterpagemenu.setHover(i, false);
					characterpagemenu.last_hover = -1;
					SendMessage(hwnd, WM_PAINT, 0, 0);
					break;
				}
			}
			break;
		}
		case WM_SELECINVTITEM:{
			if (!selblock)
				selectionlist = {(ItemData*)lParam};

			SendMessage(hwnd, WM_PAINT, 0, 0);

			break;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

