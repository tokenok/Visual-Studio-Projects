#pragma once

#include <random>
#include <ctime>
#include <numeric>

enum InitTypeValue {
	Regular,
	FewUnique,
	VeryFewUnique,
	NoUnique,
	Random,
	Normal
};

enum InitTypeOrder {
	Shuffled,
	FrayedAscending,
	FrayedDescending,
	SawtoothAscending,
	SawtoothDescending,
	VeryNearSorted,
	Reversed,
	Sorted,
	PipeOrgan,
	FrayedFront,
	FrayedMiddle,
	FrayedEnd,
	RandomFront,
	RandomMiddle,
	RandomEnd,
	Alternating,
	WorstCaseQuickSortMedianOf3,
};

class Initializer {
public:
	template<class Vector>
	static void Init(Vector* vec, size_t size, InitTypeValue itv, InitTypeOrder ito) {
		InitValues(vec, size, itv);
		InitOrder(vec, size, ito);
	}

	template<class Vector>
	static void InitValues(Vector* vec, size_t size, InitTypeValue itv) {
		Vector& v = *vec;
		switch (itv) {
			case InitTypeValue::Regular: {
				for (unsigned i = 1; i <= size; i++)
					v.push_back(i);//must use push_back
				break;
			}
			case InitTypeValue::FewUnique: {//FewUnique
				int steps = 5;
				int f = size / steps;
				for (unsigned i = 0; i < size; i++)
					v.push_back(((i / f) + 1) * f);//must use push_back
				break;
			}
			case InitTypeValue::VeryFewUnique: {//VeryFewUnique
				for (unsigned i = 0; i < size; i++) {
					int num_few = (int)((double)size * .025);
					if ((double)i / (double)size < (double)num_few / (double)size) v.push_back(size / 3);
					else if ((double)i / (double)size > (double)(size - num_few) / (double)size) v.push_back(size - 1);
					else v.push_back(2 * size / 3);//must use push_back
				}
				break;
			}
			case InitTypeValue::NoUnique: {
				unsigned val = size / 2;
				for (unsigned i = 0; i < size; i++)
					v.push_back(val);
				break;
			}
			case InitTypeValue::Random: {
				std::vector<typename Vector::value_type> temp;
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<size_t> dis(1, size);
				for (unsigned i = 1; i <= size; i++)
					temp.push_back(dis(gen));//must use push_back
				std::sort(temp.begin(), temp.end());
				v = temp;
				break;
			}
			case InitTypeValue::Normal: {
				std::vector<typename Vector::value_type> temp;
				std::random_device rd;
				std::mt19937 gen(rd());
				std::normal_distribution<double> dis((double)size / 2.0, (double)size / 10.0);
				for (unsigned i = 1; i <= size; i++)
					temp.push_back((unsigned)dis(gen) % size);//must use push_back
				std::sort(temp.begin(), temp.end());
				v = temp;
				break;
			}
			default: {
				throw(std::exception("Unhandled init value case"));
			}
		}
	}


	template<class Vector>
	static void InitOrder(Vector* vec, size_t size, InitTypeOrder ito) {
		Vector& v = *vec;
		//scramble elements (random is assumed)
		switch (ito) {
			case InitTypeOrder::Sorted: {
				//do nothing
				break;
			}
			case InitTypeOrder::FrayedAscending:
			case InitTypeOrder::FrayedDescending: {
				srand(clock());
				int dist = std::max((int)((double)size * .025), 2),
					end = size - dist;

				if (ito == InitTypeOrder::FrayedAscending) {
					for (int i = 0; i < end; i++)
						std::swap(v[(rand() % dist + i) % size], v[(rand() % dist + i) % size]);
				}
				else if (ito == InitTypeOrder::FrayedDescending) {
					{
					//	ScopedVar<DrawOption> scoped_draw_options(options.draw_options, DrawOption::None);
						for (int i = 0; i < end; i++)
							std::swap(v[(rand() % dist + i) % size], v[(rand() % dist + i) % size]);
					}
					std::reverse(v.begin(), v.end());
				}
				break;
			}
			case InitTypeOrder::SawtoothAscending:
			case InitTypeOrder::SawtoothDescending: {
				int num_teeth = 10;
				double elems_per_tooth = (double)size / (double)num_teeth;

				if (ito == SawtoothAscending) {
					for (int i = 0; i < num_teeth; i++) {
						std::reverse(v.begin() + (i * elems_per_tooth), v.begin() + ((i + 1) * elems_per_tooth));
					}
				}
				else if (ito == SawtoothDescending) {
					for (int i = 0; i < num_teeth / 2; i++) {
						std::swap_ranges(v.begin() + (i * elems_per_tooth), v.begin() + ((i + 1) * elems_per_tooth), v.begin() + ((num_teeth - i - 1) * elems_per_tooth));
					}
				}

				break;
			}
			case InitTypeOrder::VeryNearSorted: {
				double percent_out_of_order = 10.0;
				unsigned num_out_of_order = (unsigned)((double)v.size() / percent_out_of_order);
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<size_t> dis(1, size);
				for (unsigned i = 0; i < num_out_of_order; ++i)
					std::swap(v[dis(gen) % v.size()], v[dis(gen) % v.size()]);

				break;
			}
			case InitTypeOrder::Reversed: {
				std::reverse(v.begin(), v.end());
				break;
			}
			case InitTypeOrder::PipeOrgan: {
				unsigned int num_splits = 1;

				std::vector<unsigned> proxy(v.size());
				for (int s = 0; s < num_splits; s++) {
					for (unsigned i = 0; i < proxy.size(); ++i) proxy[i] = i + 1;
					for (unsigned i = 1; i < v.size(); ++i) {
						unsigned j = 0;
						while (j != i) {
							unsigned half = proxy[i] / 2;
							j = proxy[i] % 2 == 0 ? proxy.size() - half : half;
							if (j != i) {
								std::swap(proxy[i], proxy[j]);
								std::swap(v[i], v[j]);
							}
						}
					}
				}

				break;
			}
			case InitTypeOrder::FrayedFront:
			case InitTypeOrder::FrayedMiddle:
			case InitTypeOrder::FrayedEnd: {
				double percent = .20;
				size_t num = (unsigned)((double)size * percent);

				std::random_device rd;
				std::mt19937 gen(rd());

				if (ito == InitTypeOrder::FrayedFront) {
					std::shuffle(v.begin(), v.begin() + num, gen);
					//this loop is for drawing the rest of the list (it has no other purpose)
					for (auto i = v.begin() + num; i != v.end(); ++i)
						iter_swap(i, i);
				}
				if (ito == InitTypeOrder::FrayedMiddle) {
					auto mid = v.begin() + ((v.end() - v.begin()) >> 1);
					auto hnum = num / 2;
					auto l = mid - hnum;
					auto r = mid + hnum;

					//this loop is for drawing the rest of the list (it has no other purpose)
					for (auto i = v.begin(); i != l; ++i)
						iter_swap(i, i);
					std::shuffle(l, r, gen);
					//this loop is for drawing the rest of the list (it has no other purpose)
					for (auto i = r; i != v.end(); ++i)
						iter_swap(i, i);
					
				}
				else if (ito == InitTypeOrder::FrayedEnd) {
					//this loop is for drawing the rest of the list (it has no other purpose)
					for (auto i = v.begin(); i != v.end() - num; ++i)
						iter_swap(i, i);
					std::shuffle(v.end() - num, v.end(), gen);
				}
				break;
			}
			case InitTypeOrder::RandomFront:
			case InitTypeOrder::RandomMiddle:
			case InitTypeOrder::RandomEnd: {
				double percent_unsorted = .20;
				size_t num_sorted = (size_t)((double)size * (1.0 - percent_unsorted));

				std::random_device rd;
				std::mt19937 gen(rd());

				std::vector<size_t> rand_indices(size);	for (size_t i = 0; i < size; ++i) rand_indices[i] = i; 
				std::shuffle(rand_indices.begin(), rand_indices.end(), gen);
				rand_indices.resize(num_sorted);

				if (ito == InitTypeOrder::RandomFront) {
					std::sort(rand_indices.begin(), rand_indices.end(), std::greater<>());

					for (int i = 0; i < num_sorted; i++) 
						std::swap(v[size - i - 1], v[rand_indices[i]]);

					std::shuffle(v.begin(), v.begin() + size - num_sorted, gen);
				}
				else if (ito == InitTypeOrder::RandomEnd) {
					std::sort(rand_indices.begin(), rand_indices.end(), std::less<>());

					for (int i = 0; i < num_sorted; i++) 
						std::swap(v[i], v[rand_indices[i]]);

					std::shuffle(v.begin() + num_sorted, v.begin() + size, gen);
				}
				else if (ito == InitTypeOrder::RandomMiddle) {
					std::sort(rand_indices.begin(), rand_indices.end(), std::less<>());
					size_t hnum_sorted = num_sorted / 2;
					std::reverse(rand_indices.begin() + hnum_sorted, rand_indices.end());

					for (int i = 0; i < hnum_sorted; i++) {
						std::swap(v[i], v[rand_indices[i]]);
						std::swap(v[size - i - 1], v[rand_indices[i + hnum_sorted]]);
					}

					auto mid = v.begin() + ((v.end() - v.begin()) >> 1);
					std::shuffle(v.begin() + hnum_sorted, v.end() - hnum_sorted, gen);
				}

				break;
			}
			case InitTypeOrder::Shuffled: {
				std::random_device rd;
				std::mt19937 gen(rd());

				std::shuffle(v.begin(), v.end(), gen);
				break;
			}
			case InitTypeOrder::Alternating: {
				std::vector<bool> visited(size, false);
				visited[0] = true;

				//cyclically place values in alternating order
				unsigned num_inplace, i, to_i, last_unvisited; num_inplace = i = to_i = last_unvisited = 1;
				unsigned mid = size / 2;
				unsigned sizex2 = size * 2;
				auto val_to_move = v[i];
				while (num_inplace < size) {
					unsigned ix2 = i * 2;
					to_i = i < mid ? ix2 : sizex2 - ix2 - 1;
					if (visited[to_i]) {
						for (unsigned visit = last_unvisited; visit < visited.size(); ++visit) {
							if (!visited[visit]) {
								i = last_unvisited = visit;
								val_to_move = v[i];
								break;
							} 
						}
						continue;
					}
					auto next_val = v[to_i];
					v[to_i] = val_to_move;
					val_to_move = next_val;
					visited[to_i] = true;
					++num_inplace;
					i = to_i;
				}

				break;
			}
			case WorstCaseQuickSortMedianOf3: {
				auto p = v;
				std::iota(p.begin(), p.end(), 0);
				auto i = 0u;
				for (; i < v.size(); i += 2) {
					auto const iPivot0 = i;
					auto const iPivot1 = (i + v.size() - 1) / 2;
					v[p[iPivot1]] = i + 1;
					v[p[iPivot0]] = i;
					std::swap(p[iPivot1], p[i + 1]);
				}
				if (v.size() > 0 && i == v.size())
					v[v.size() - 1] = i - 1;

				break;
			}
			default: {
				throw(std::exception("Unhandled init order case"));
			}
		}
		//end scramble elements
	}


};











