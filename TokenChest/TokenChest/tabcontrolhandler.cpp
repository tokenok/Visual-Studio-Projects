#include "tabcontrolhandler.h"

#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")

#include "common.h"

TabClass::TabClass(HWND parent) {
	m_ParentControl = parent;
}

void TabClass::setParent(HWND parent) {
	m_ParentControl = parent;
}
HWND TabClass::getParent() {
	return m_ParentControl;
}
int TabClass::addTab(std::string tab_text, HWND wnd) {
	m_tabs.push_back({tab_text, wnd});
	m_tabs[m_tabs.size() - 1].pos = m_tabs.size() - 1;
	int tpos = m_tabs[m_tabs.size() - 1].pos;

	TCITEM tci;
	tci.mask = TCIF_TEXT;
	tci.pszText = str_to_LPWSTR(tab_text);
	TabCtrl_InsertItem(m_ParentControl, tpos, &tci);
	delete[] tci.pszText;

	SetWindowPos(wnd, NULL, 1, 21, 0, 0, SWP_NOSIZE);

	return tpos;
}
bool TabClass::selectTab(int tab_pos) {
	ShowWindow(m_tabs[tab_pos].wnd, SW_SHOW);
	if (tab_pos == m_selectedTab) return false;
	TabCtrl_SetCurSel(m_ParentControl, tab_pos);
	ShowWindow(m_tabs[m_selectedTab].wnd, SW_HIDE);
	m_selectedTab = tab_pos;
	return true;
}
myTab& TabClass::getTab(int tab_pos) {
	return m_tabs[tab_pos];
}

