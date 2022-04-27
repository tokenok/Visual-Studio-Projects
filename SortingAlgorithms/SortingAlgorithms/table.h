#pragma once

#include <list>
#include <string>
#include <utility>
#include <functional>
#include <algorithm>
#include <iostream>

template<typename RowData>
class Table {
public:
	Table() {}

	Table(std::vector<RowData>& rows) {
		AddRows(rows);
	}

	void AddRows(std::vector<RowData>& rows) {
		for (unsigned i = 0; i < rows.size(); ++i)
			AddRow(rows[i]);
	}

	void AddRow(RowData& row) {	
		Rows.push_back(row);
		RowData::RowAddedProc(Rows.back());
	}

	unsigned GetRowCount() { return Rows.size(); }

	RowData GetRow(unsigned i) {
		return Rows[i];
	}

	std::vector<RowData> GetRows() {
		return Rows;
	}

	/*void SortBy(typename RowData::ColumnId col_id, bool is_ascending = true) {
		std::stable_sort(Rows.begin(), Rows.end(), std::bind(RowData::CompareByColumn, std::placeholders::_1, std::placeholders::_2, col_id, is_ascending));
	}*/

	void SortByColumn(typename RowData::ColumnId col_id, bool is_ascending = true) {
		RowData::SortByColumnProc(col_id, is_ascending);
	}

	void PrintRow(unsigned row_index) {
		Rows[row_index].Print();
	}

	void PrintRows() {
		for (unsigned i = 0; i < Rows.size(); ++i)
			PrintRow(i);
	}

	std::wstring GetColumnName(typename RowData::ColumnId col_id) {
		return RowData::ColumnNames[col_id];
	}

private:
	std::list<RowData> Rows;
};




