#pragma once

#include <string>
#include <vector>
#include <map>
#include <stack>

#include "d2data.h"

namespace trade {
	extern const std::string deffilename;
	extern std::string tradefilename;

	extern std::map<std::string, std::map<std::string, std::map<std::string, std::map<std::string, std::vector<ItemData*>>>>> data;

	enum {
		ADD_OK = 0,
		REMOVE_OK,
		NOT_MYFILE,
		ALREADY_ADDED,
		NOT_IN_TRADE_FILE,
		NOT_AN_ITEM
	};

	bool findtradeitem(const ItemData* item);
	bool is_itemcountmax(const ItemData* item, int cmp_depth = CMP_CHARACTER);
	int displayret(int ret);
	bool is_mytradefile();
	bool load(std::string filename = tradefilename);
	bool update();
	bool writefile();
	int add(std::vector<ItemData*> items, bool write = true);
	int remove(std::vector<ItemData*> items, bool write = true);
	int add(ItemData* item, bool write = true);
	int remove(ItemData* item, bool write = true);

	enum {
		UNDO_ADD = 1,
		UNDO_REMOVE,
		REDO_ADD,
		REDO_REMOVE
	};
	class Undo {
		std::stack<std::pair<std::vector<ItemData*>, int>> undostack;
		std::stack<std::pair<std::vector<ItemData*>, int>> redostack;
		bool stacklock;

		public:
		void push(ItemData* item, int action);
		void push(std::vector<ItemData*> items, int action);
		bool can_undo() { return !undostack.empty(); }
		bool can_redo() { return !redostack.empty(); }
		bool is_stacklocked() { return stacklock; }
		void clear();
		bool undo();
		bool redo();
	};
	extern Undo undo;
}

