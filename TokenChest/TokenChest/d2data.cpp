#include "d2data.h"

#include "include.h"

using namespace std;

bool operator==(const vector<string>& lhs, const vector<string>& rhs) {
	if (lhs.size() != rhs.size())
		return false;
	for (UINT i = 0; i < lhs.size(); i++) {
		if (lhs[i] != rhs[i])
			return false;
	}
	return true;
}
bool operator==(const InvData& lhs, const InvData& rhs) {
	return (lhs.x == rhs.x
		&& lhs.y == rhs.y
		&& lhs.w == rhs.w
		&& lhs.h == rhs.h);
}
bool operator==(const ItemData& lhs, const ItemData& rhs) {
	return (lhs.realm == rhs.realm
		&& lhs.account == rhs.account
		&& lhs.character == rhs.character
		&& lhs.store == rhs.store
		&& lhs.basestats == rhs.basestats
		&& lhs.stats == rhs.stats
		
		&& lhs.invdata == rhs.invdata
		);
}
bool itemcmp(const ItemData& lhs, const ItemData& rhs, int cmp_depth/* = CMP_INVPOS*/) {
	if (!&lhs || !&rhs)
		return false;
	return (lhs.realm == rhs.realm
		&& (cmp_depth & CMP_ACCOUNT ? lhs.account == rhs.account : true)
		&& (cmp_depth & CMP_CHARACTER ? lhs.character == rhs.character : true)
		&& (cmp_depth & CMP_STORE ? lhs.store == rhs.store : true)
		&& (cmp_depth & CMP_INVPOS ? lhs.invdata == rhs.invdata : true)
		&& lhs.basestats == rhs.basestats
		&& lhs.stats == rhs.stats);
}

UINT CharacterData::getItemsCount() {
	return cube_items.size() + body_items.size() + stash_items.size() + merc_items.size() + inv_items.size();
}
ItemData* CharacterData::getItem(UINT pos) {
	if (pos < this->getItemsCount()) {
		if (pos < cube_items.size())
			return (&cube_items[pos]);
		pos -= cube_items.size();
		if (pos < body_items.size())
			return (&body_items[pos]);
		pos -= body_items.size();
		if (pos < stash_items.size())
			return (&stash_items[pos]);
		pos -= stash_items.size();
		if (pos < merc_items.size())
			return (&merc_items[pos]);
		pos -= merc_items.size();
		if (pos < inv_items.size())
			return (&inv_items[pos]);
	}
	return nullptr;
}

void getItemDataFromCode(string code, ItemData* item, bool gettype/* = false*/) {
	for (UINT i = 0; i < g_itemcodes.size(); i++) {
		if (g_itemcodes[i].code == code) {
			if (gettype)
				item->icode.type = g_itemcodes[i].type;
			item->icode.subtype = g_itemcodes[i].subtype;
			item->icode.basetype = g_itemcodes[i].basetype;			
			item->icode.tier = g_itemcodes[i].tier;
			return;
		}
	}
	/*ItemCode icode;
	icode.code = code;
	icode.type = item->name;
	icode.subtype = "Annihilus";
	icode.basetype = "Misc";
	icode.tier = 0;
	item->icode.code = code;
	item->icode.type = icode.type;
	item->icode.subtype = icode.subtype;
	item->icode.basetype = icode.basetype;
	item->icode.tier = icode.tier;
	g_itemcodes.push_back(icode);
	write_itemcodes(g_itemcodes);
	TabPage2Proc(g_TAB.getTab(1).wnd, WM_INITDIALOG, NULL, NULL);*/
}

vector<ItemData>* get_item_store(string realm, string account, string character, string store) {
	vector<ItemData>* items;
	for (UINT i = 0; i < g_characters.size(); i++) {
		if (realm == g_characters[i].realm && account == g_characters[i].account && character == g_characters[i].character) {
			items = store == "inventory" ? &g_characters[i].inv_items
				: store == "stash" ? &g_characters[i].stash_items
				: store == "cube" ? &g_characters[i].cube_items
				: store == "body" ? &g_characters[i].body_items
				: store == "mercenary" ? &g_characters[i].merc_items
				: NULL;
			return items;
		}
	}
	return NULL;
}
vector<ItemData>* get_item_store(const ItemData* item_in_store) {	
	return get_item_store(item_in_store->realm, item_in_store->account, item_in_store->character, item_in_store->store);
}
CharacterData* get_character(string realm, string account, string character) {
	for (UINT i = 0; i < g_characters.size(); i++) {
		if (realm == g_characters[i].realm && account == g_characters[i].account && character == g_characters[i].character)
			return &g_characters[i];
	}
	return NULL;
}
CharacterData* get_character(const ItemData* item) {
	return get_character(item->realm, item->account, item->character);
}

