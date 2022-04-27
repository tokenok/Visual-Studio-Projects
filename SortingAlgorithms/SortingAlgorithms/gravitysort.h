#pragma once

template<class T, class C>
inline void gravitysort(T _First, T _Last, C compare) {		
	std::vector<unsigned> falling(_First, _Last);

	unsigned landed = 0;
	while (landed < falling.size()) {
		for (unsigned i = landed; i < falling.size(); ++i) {
			if (compare(falling[i], 1)) {
				iter_swap(_First + landed, _First + i);
				std::swap(falling[i], falling[landed]);
				++landed;
			}
			else
				--falling[i];
		}
	}
}

template<class T>
inline void gravitysort(T _First, T _Last) {
	gravitysort(_First, _Last, std::less<>());
}






















