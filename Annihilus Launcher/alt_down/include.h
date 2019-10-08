#ifndef include_header_guard
#define include_header_guard

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include <Windows.h>
#include <string.h>
#include <time.h>
#include <algorithm>
#include <thread>
#include <fstream>
#include <iostream>
#include "Shlwapi.h"
//lean and mean
#include <Commdlg.h>
#include <sapi.h>

#include "common.h"
#include "hotkey.h"
#include "image.h"
#include "d2funcs.h"
#include "keypress.h"
#include "resource.h"
#include "ocr.h"
#include "Custom Trackbar.h"
#include "window.h"
//#include "Tokenhackdll.h"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "winmm.lib")


#endif



/*


>nec
>/
>20%
>dex
>s+20
>(2)

********************
*** Configuration File  ***
********************
*Tahoma Regular 10 (no word wrap)
*you can edit values after a :(colon), between () or between []
**PRESS RELOAD BUTTON AFTER YOU MAKE ANY CHANGES TO THIS FILE IF YOU WANT THEM TO GO INTO EFFECT


*			(*)<--- comment (anything after a * is ignored)
*			(>)<--- item stat matchline
*			anything between () is a virtual keycode. modifier keys : (alt = 256, ctrl = 512, shift = 1024, windows key = 2048)
*				 turn on console + kb to see the virtual keycode of various keys if you want to manually set a hotkey
*			anything bewteen [] is the previous value of the on/off state (value after colon)

*----------------------------------------------------------------
** Program functions and hotkeys **   0 = OFF | 1+ = ON
*----------------------------------------------------------------

wmc (368)[0]		:	1	*locks cursor inside forground window
close (369)[0]		:	1	*closes tokenhack
"/nopickup" (118)[0]	:	1	*types /nopickup
pause (120)[0]		:	1	*pauses tokenhack (turns off all hotkeys)
transmute (121)[0]		:	2	*simulates leftclick over transmute button (can set position manually). 2 to highlight stats that are matched with '>' matchlines (to test if your matchline will work)
auto_roll (117)[0]		:	1	*2 to hide window while rolling
read_stats (220)[0]	:	2	*2 to highlight stats that are matched with '>' matchlines (to test if your matchline will work)

*test (112)[0]		:	1	*rrrrrrainbows
*test2(113)[0]		:	1	*spooooooky ghoooost
*test3(114)[0]		:	1	*layered window screenshot test

***manually set transmute button position here. you can use (auto) or ([pt.x], [pt.y]) ex-> 240, 335
**turn on console + kb to get cursor pos when you press the transmute hotkey (hover over transmute button)
**(-1, -1) is same as auto

transmute button pos	:	-1, -1 		*auto

layered window fix		:	1	*this could potentially not work so im giving the option to turn it off
layered window delay	:	100	*time in milliseconds for roller to wait for layered window to be applied (increasing fixes initial no stats found problems)
ocr title display		:	1	*maybe changing the title breaks something?
ocr same stats timeout	:	30	*approx. # of seconds before roller will exit because same stats found in a row (game dropped or timed out etc)
ocr no stats timeout	: 	30	*approx. # of seconds before roller will exit because no stats are found in a row

*----------------------------------------------------------------
** Toggle States **	:	0=OFF | 1=ON
*----------------------------------------------------------------
volume[0x7878]		:	0x7878	*volume ranges from 0x0000 to 0xFFFF
RAINBOW[1]		:	0
console[0]		:	1
keyboardhookoutput[1]	:	0
show tokenhack on start[0]	:	1

*----------------------------------------------------------------
** Launch button **
*----------------------------------------------------------------

gamepath		:	C:\Program Files (x86)\Diablo II d2pk 5\Diablo II.exe
target lines		:	-w -nohide *-skiptobent

*----------------------------------------------------------------
**  Auto roller stat definitions and color settings **
*----------------------------------------------------------------
*you may have to change these, press read hotkey then open capture.bmp with mspaint to find the rgb value of the "core" color of the letters

BLUE RGB VALUE		:	80, 80,172

*Just leave these commented out, although they may work
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
*added OR logic (|) and multiple items (>>)

**example usage of OR and multi items
**>+20 to Life
**>Cold Resist +11%
**>>
**>>+5% Faster Hit Recovery
**>Cold Resist +11% | Lightning Resist +11% | Fire Resist +11%
**above would find a 20/11 cold res sc, or a sc with 5 fhr with either 11% cold, fire, or lite res

*above can be simplified to:
**>life
**>cold
**>>
**>5%
**>cold | light |  firer


*comment these out or delete them if you want to(they dont do anything)
i (84)[1]:	0
put(79)[1]:	0
these (75)[1]:	0
here(69)[1]:	0
so(78)[1] :	0
"rainbow"(73)[1] :	0
looks(83)[1] :	0
much(71)[1] :	0
better(82)[1] :	0
than(69)[1] :	0
it(65)[1] :	0
does(84)[1] :	0
with(69)[1] :	0
fewer(83)[1] :	0
things(84)[1] :	0







*/