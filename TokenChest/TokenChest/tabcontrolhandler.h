#pragma once

#include <Windows.h>
#include <string>
#include <vector>

struct myTab {
	std::string label;
	HWND wnd;
	int pos;
};
class TabClass {
	private:
	HWND m_ParentControl;
	std::vector<myTab> m_tabs;
	int m_selectedTab = 0;

	public:
	TabClass() {}
	TabClass(HWND parent);

	void setParent(HWND parent);
	HWND getParent();
	int addTab(std::string tab_text, HWND wnd);
	bool selectTab(int tab_pos);
	myTab& getTab(int tab_pos);
};

