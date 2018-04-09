#ifndef PLANE_H
#define PLANE_H

#include "seats.h"

#include <vector>

class Plane {
	private:
	int id;
	std::vector<std::vector<EconomySeat> > economySeats;
	std::vector<std::vector<FirstClassSeat> > firstclassSeats;
	std::vector<std::vector<EconomyPlusSeat> > economyplusSeats;

	public:
	int get_id() const { return id; }
	void set_id(int id) { this->id = id; }

	int get_economy_rows() const { return economySeats.size(); }
	int get_economy_cols() const { return economySeats.size() ? economySeats[0].size() : 0; }
	void set_economy_rows(int rows) { economySeats.resize(rows); }
	void set_economy_cols(int cols) { for (unsigned int i = 0; i < economySeats.size(); i++) economySeats[i].resize(cols); }

	int get_economyPlus_rows() const { return economyplusSeats.size(); }
	int get_economyPlus_cols() const { return economyplusSeats.size() ? economyplusSeats[0].size() : 0; }
	void set_economyPlus_rows(int rows) { economyplusSeats.resize(rows); }
	void set_economyPlus_cols(int cols) { for (unsigned int i = 0; i < economyplusSeats.size(); i++) economyplusSeats[i].resize(cols); }

	int get_firstClass_rows() const { return firstclassSeats.size(); }
	int get_firstClass_cols() const { return firstclassSeats.size() ? firstclassSeats[0].size() : 0; }
	void set_firstClass_rows(int rows) { firstclassSeats.resize(rows); }
	void set_firstClass_cols(int cols) { for (unsigned int i = 0; i < firstclassSeats.size(); i++) firstclassSeats[i].resize(cols); }
};
extern std::vector<Plane*> g_fleet;

#endif

