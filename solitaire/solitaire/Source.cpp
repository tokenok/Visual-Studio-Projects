#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>

#include "C:\CPPlibs\common\common.h"

using namespace std;

HWND g_hwnd;

const TCHAR className[] = TEXT("Solitaire");

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

ATOM init_register_class(HINSTANCE hInstance) {
	WNDCLASSEX wnd = { 0 };
	wnd.hInstance = hInstance;
	wnd.lpszClassName = className;
	wnd.lpfnWndProc = WndProc;
	wnd.style = NULL;
	wnd.cbSize = sizeof(WNDCLASSEX);
	//wnd.hIconSm = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(RESOURCE));
	return RegisterClassEx(&wnd);
}

const vector<string> CardSuits = {	"spades", "diamonds", "clubs", "hearts" };
const vector<string> CardValues = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };

class Card {
public:
	int suit;
	int value;

	Card() {
		suit = -1;
		value = -1;
	}

	Card(int s, int v) {
		suit = s; value = v;
	}

	void printCard() {
		cout << CardValues[value] << " of " << CardSuits[suit] << '\n';
	}
};

class Game {
public:
	vector<vector<Card>> piles;
	vector<vector<Card>> score;
	vector<Card> draw;
	vector<Card> deck;

	Game() {
		piles.reserve(7);
		score.reserve(4);

		for (int i = 0; i < CardSuits.size(); i++) {
			for (int j = 0; j < CardValues.size(); j++) {
				Card t = { i, j };
				deck.push_back(t);
			}
		}				
		shuffle<Card>(deck);

		for (int i = 0; i < 7; i++) {
			for (int j = i; j < 7; j++) {
				piles[j].push_back(Draw());
			}
		}

		cout << '\n';
	}

	Card Draw() {
		Card ret;
		if (deck.size() > 0) {
			ret = deck[0];
			deck.erase(deck.begin());
		}
		return ret;
	}

};

Game* game = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*args*/, int /*iCmdShow*/) {
	SHOW_CONSOLE();

	init_register_class(hInstance);

	g_hwnd = CreateWindowEx(NULL, className, L"Solitaire", WS_OVERLAPPEDWINDOW, 350, 150, 800, 600, NULL, NULL, hInstance, NULL);

	ShowWindow(g_hwnd, SW_SHOW);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_CREATE: {
			if (!game)
				game = new Game();

			break;
		}
		case WM_CLOSE:
		case WM_DESTROY: {
			EndDialog(hwnd, MB_OK);
			PostQuitMessage(0);
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}