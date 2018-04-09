#ifndef include_header_guard
#define include_header_guard

#include <Windows.h>
#include <string.h>
#include <time.h>
#include <algorithm>
#include <thread>
#include <gdiplus.h>
#include <fstream>
#include <memory>
#include <iostream>
#include <math.h>
#include <Dbt.h>
#include "Shlwapi.h"

#include "common.h"
#include "window.h"
#include "hotkey.h"
#include "image.h"
#include "d2funcs.h"
#include "keypress.h"
#include "resource.h"
#include "ocr.h"
#include "Custom Trackbar.h"
#include "Tokenhackdll.h"

#include "iTunes.h"
#include "Voice.h"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "winmm.lib")

HWND g_lclicktarget;
POINT g_lclickpt;

//#pragma comment(linker,"\"/manifestdependency:type='win32' \
//name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
//processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"") 

#endif

/*



********************
*** Configuration File  ***
********************
*Tahoma Regular 10 (no word wrap)
*you can only edit values after the colon. DONT CHANGE ANYTHING ELSE
**PRESS RELOAD BUTTON AFTER YOU MAKE ANY CHANGES TO THIS FILE IF YOU WANT THEM TO GO INTO EFFECT


*			(*)<--- comment (asterisk)
*			(#)<--- text to spam
*			(>)<---	item stat matchline


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
~auto_roll (117)		:	1	*2 to hide window while rolling, 3 to reduce window size by 40%
~read_stats (220)		:	1	*2 to type stats to active window
~text_spam(123)		:	0

~timer(112)		:	0	*number of seconds
~test (113)		:	0

~itunes_play(120)		:	0
~itunes_prev(122)		:	0
~itunes_next(121)		:	0
~itunes_volume_up(1062)	:	0
~itunes_volume_down(1064)	:	0


*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*******************************************************
***manually set button positions here. can use (auto) or ([pt.x], [pt.y]) ex-> (240, 335) [no parentheses].
**turn console + kb on to get cursor pos when you press roll hotkey (hover over where you want it to click)
**(-1, -1) is same as auto
transmute button pos	:	-1, -1 		*auto
gold button pos		:	auto
*******************************************************
*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*******************************************************
***set weapon swap key (as a virtual keycode) ex-> 'w' = 0x57 (hex) |  'w' = 87 (dec)
**use -1 for scroll wheel up, -2 for scroll wheel down
**turn on console + kb to see virtual keycode for different keys
weapon swap key		:	-1
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
console			:	1
keyboardhookoutput	:	0

*----------------------------------------------------------------
** Launch button settings**
*----------------------------------------------------------------

*if you want to change game path you can do it manually here or
*just delete everything after the colon ("game path : [delete this]"), save, then reload, then click the launch button to open file explorer

*when launching using a shortcut remove any target line commands from the shortcut

gamepath		:	C:\Program Files (x86)\Diablo II PD\Diablo II\D2Loader.exe
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
*****if you use shorthand make sure that it is a unique phrase (">STR" will be found in "caST Rate" as well as "STRength")
*****the "> line" must also be a contiguous phrase (ex. ">cold41" will not find: Cold Resist +41; use: ">cold resist +41" or ">cold res" or even just ">cold"
*the roller will stop once it cant read any text on the screen (move your cursor away from the item, minimize/ close the window)
*you may also press the 'Break' key to stop it
**TL;DR. use unique contiguous phrases if you want, or just use the full text that appears in-game

*example usage (crafted amulet):
**>+2 to Necromancer Skill Levels
**>+20% Faster Cast Rate
**>+30 to Strength
**>+20 to Dexterity
**>+20 to Mana
**>Regenerate Mana 10%

*above can be simplified to:
**>nec
**>20%f
**>stre
**>x


>this_is_text_that
>the_auto_roller
>will_try_to_match
>open_config_to
>edit_it





















































*/

