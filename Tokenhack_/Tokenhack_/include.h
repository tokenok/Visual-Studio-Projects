///auto start file setting dont start on reload
///config button
///show or minimize on start -file
///launch pd button
///file comments
///reload button
///OCR
///>matchlines----8
///HOOKS
///directory set for launch


///**features**

//imagefile tray menu / button
//README
//tray icon lock
//force destroy icon
//OR(|), NOT(!), and wildcard(?)/ wildcard sequence (\?), logic for match lines
//multiple items
//itunes gui
//custom window and dialog
//update use function (simplify)
//input stats with edit control
//keeping old values on uncheck [prev]
//roll count
//new ocr
//settings dialog
//start with last active tab
//

///**functionality**

//find box

///*image
//previous image input path
//crop
//grow
//gray
//brightness
//contrast
//invert
//convert to type (gif, png, bmp, tiff, jpg)
//animate gif
//compare two images
//dither
//video to image sequence
//type text in file (use) #typethisline
//inventory to stash vise versa
//gheed finder

/*fixes
timer display fix + escape

*/
#ifndef include
#define include


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>
#include <time.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <thread>
#include <sapi.h>
#include <objidl.h>
#include <gdiplus.h>
//lean and mean
#include <Shellapi.h>
#include <Commdlg.h>
#include <tlhelp32.h>
#include "Shlwapi.h"

std::string VERSION = "TokenHack_v2.127";
std::wstring WVERSION = std::wstring(VERSION.begin(), VERSION.end());

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "Shlwapi.lib")

//#pragma comment(linker,"\"/manifestdependency:type='win32' \
//name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
//processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"") 

using namespace std;

ifstream filein;
ofstream fileout;
bool ocr_spaces_on = true;
string gamedir;
string gamepath = "";
string targetlines = "";
int transmutebuttonx = -1;
int transmutebuttony = -1;
int goldbuttonx = -1;
int goldbuttony = -1;
int weapsonswitchkey = 0;
HBRUSH color_static_brush = NULL;

//HWND lastwindow = NULL;

////////////////////////////////////////////////////////////
int totalonfunctions = 0;

int tokenhackfunctionwindowxposoffset = 5;

int windowwidth = 400;
int functionnameyoffsum = 0;

int functionnameheight = 16;
int functionnamewidth = 180;

int tabbuttonheight = functionnameheight;

int checkboxwidth = 15;
int checkboxheight = 15;

int buttonwidth = 150;
int buttonheight = functionnameheight;

int checkboxxoff = 0 + tokenhackfunctionwindowxposoffset;
int buttonxoff = checkboxwidth + tokenhackfunctionwindowxposoffset + 1;
int functionnamexoff = 180 + tokenhackfunctionwindowxposoffset;
int statboxxoff = 370 + tokenhackfunctionwindowxposoffset;

int bottomheight = 135;

////////////////////////////////////////////////////////////

string dec_to_base(int num, int base) {
	string ret = "";
	if (base < 2)
		return ret;
	if (base > 36)//what symbols to use past 'Z'?
		return ret;
	if (num == 0)
		return "0";
	string t = "";
	for (int k = 0; num > 0; k++) {
		int w = num % base;
		if (w > 9)
			t += w + 55;
		else
			t += w + 48;
		num /= base;
	}
	for (int i = t.length() - 1; i > -1; i--)
		ret += t[i];
	return ret;
}

string int_to_str(int num) {
	cout.clear();
	stringstream out;
	out << num; 
	return out.str();
}
int str_to_int(string str) {
	cout.clear();
	stringstream out(str);
	int num;
	out >> num;
	return num;
}

int str_to_hex(string str) {
	cout.clear();
	stringstream out(str);
	int num;
	out >> hex >> num;
	return num;
}

wstring str_to_wstr(string s) {
	wstring ret(s.begin(), s.end());
	return ret;
}
string wstr_to_str(wstring ws) {
	string ret(ws.begin(), ws.end());
	return ret;
}

string error_code_to_text(DWORD error_code) {
	string ret;
	LPTSTR buf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error_code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&buf,
		0, NULL);
	return wstr_to_str(buf) + '(' + int_to_str(error_code) + ')';
}

string milliseconds_to_hms(double milliseconds) {
	string seconds = int_to_str((int)fmod((milliseconds / 1000), 60));
	string minutes = int_to_str((int)fmod(((milliseconds / (1000 * 60))), 60));
	string hours = int_to_str((int)fmod(((milliseconds / (1000 * 60 * 60))), 24));
	if (seconds.size() < 2)
		seconds = '0' + seconds;
	if (minutes.size() < 2)
		minutes = '0' + minutes;
	return (hours + ':' + minutes + ':' +  seconds);
}

#endif

/*



**>(stats to find)
>+30% Faster Block Rate
>20% Increased Chance of Blocking
>All Resistances +45
>socketed(4)


**#(text to type)
#FREDLOVESTOKEN
#wtf, who is fred
#oh, him? hes just a random
#he wishes he was as cool as token
#i heard he cant even name lock
#yea hes pretty terrible

********************
*** Configuration File  ***
********************
*Tahoma Regular 10 (no word wrap)
*you can only edit values after the colon. DONT CHANGE ANYTHING ELSE


*	(*)<--- comment (asterisk)
*	(>)<--- stat text to match
*	(#)<--- text line to spam


*----------------------------------------------------------------
** Program functions and hotkeys **   0=OFF | 1=ON
*----------------------------------------------------------------

~wmc (368)		:	1
~close (369)		:	1
~gamble (370)		:	1
~gold_drop (115)		:	1	*1 for normal walk (left click only); 2 for casting with left click(presses shift and left click)
~wsg_cast (116)		:	1
~skill_bug (81)		:	1
~skill_bug_set (192)	:	1
~"/nopickup" (118)		:	1
~left_click (119)		:	1	*1 for single click; 2 for toggle
~pause (120)		:	1
~transmute (121)		:	1
~".drop" (122)		:	1
~switch_weapons (87)	:	1
~auto_roll (117)		:	1	*2 to hide window while rolling
~read_stats (220)		:	1	*2 to type stats to active window
~text_spam(123)		:	1
~test (113)		:	0


*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*******************************************************
***manually set button positions here. can use (auto) or ([pt.x], [pt.y]) ex-> (240, 335) [no parentheses].
**(-1, -1) is same as auto
transmute button pos	:	-1, -1 		*auto
gold button pos		:	auto
*******************************************************
*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*******************************************************
***set weapon swap key (as a virtual keycode) ex-> 'w' = 0x57 (hex) |  'w' = 87 (dec) | 'w' = 'w'
**use -1 for scroll wheel up, -2 for scroll wheel down
**use console + kb to see virtual keycode for different keys
weapon swap key		:	87
*******************************************************
*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*******************************************************
**Skill bugging**
*(press skillbug set hotkey) -> then press the hotkey (needs to be a-z) of the skill you want to cast first,
*then press the hotkey of the skill you want to bug onto.
*ex. Bugging for farcast (z->charged strike, x->throw)
*press skillbug set hotkey ->  press z -> then press x -> then activate with skillbug key
*******************************************************
*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

*----------------------------------------------------------------
** Toggle States **	:	0=OFF | 1=ON
*----------------------------------------------------------------
sound			:	1
show tokenhack on start	:	1
RAINBOW		:	0
console			:	0
keyboardhookoutput	:	0

*----------------------------------------------------------------
** Launch button settings**
*----------------------------------------------------------------

*if you want to change game path you can do it manually here or
*just delete everything after the colon ("game path : [delete this]"), save, then reload, then click the launch button to open file explorer

*when launching using a shortcut remove any target line commands from the shortcut

gamepath		:
target lines		:	-w -nohide *-skiptobent


*----------------------------------------------------------------
**  Auto roller stat definitions and color settings ** ex. >3%fasterrun/walk   <---put anywhere you want
*----------------------------------------------------------------
*you may have to change these, open capture.bmp with mspaint to find the rgb value of the "core" color of the letters

BLUE RGB VALUE		:	80, 80,172
*WHITE RGB VALUE		:	196, 196, 196
*YELLOW RGB VALUE	:	216, 184, 100
*GRAY RGB VALUE		:	80, 80, 80
*GOLD RGB VALUE		:	148, 128, 100
*GREEN RGB VALUE		:	24, 252, 0
*ORANGE RGB VALUE	:	208, 132, 32
*RED RGB VALUE		:	176, 68, 52
*MAGENTA RGB VALUE	:	164, 32, 252

***********************************************
*start each matchline with '>' 	each line has AND logic, all lines must be found before roller will stop
*dont worry about spaces or capitalization as they are ignored
*type each stat as it is ingame
*symbols recognized by ocr: "a-z" (not j), "A-Z", "0-9", [":"],  [","],  [")"],  ["("],  ["+"],  ["-"],  ["%"],  ["'"] - lowercase 'j' cannot be read, instead use lowercase 'I' in your "> lines" (should never come up)
*****you can do shorthand, for example-> instead of: ">+20 to dexterity", you can just use ">dex" or just ">x" as x is likely to be unique
*****if you use shorthand make sure that it is a unique phrase (">STR" will be picked up in "caST Rate" as well as "STRength")
*****the "> line" must also be a contiguous phrase (ex. ">cold41" will not find an item with 41 cold res/dmg; use: ">cold resist +41" or ">cold res" or even just ">cold"
*the roller will stop once it cant read any blue text on the screen (move your cursor away from the item)

*example usage (crafted amulet):
**>+2 to Necromancer Skill Levels
**>+20% Faster Cast Rate
**>+30 to Strength
**>+20 to Dexterity
**>+20 to Mana
**>Regenerate Mana 10%

*this can be simplified to:
**>nec
**>20%f
**>stre
**>x



































*/

