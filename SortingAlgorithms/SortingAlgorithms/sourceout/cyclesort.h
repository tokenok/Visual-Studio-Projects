#pragma once

template<class T, class C>
void cyclesort_(T a, int n, C compare) {
	using std::swap;
	int writes = 0;
	for (int c_start = 0; c_start <= n - 2; c_start++) {
		typename T::value_type item = a[c_start];
		int pos = c_start;
		for (int i = c_start + 1; i < n; i++)
			if (a[i] < item)
				pos++;
		if (pos == c_start)
			continue;
		while (item == a[pos])
			pos += 1;
		if (pos != c_start) {
			swap(item, a[pos]);
			writes++;
		}
		while (pos != c_start) {
			pos = c_start;
			for (int i = c_start + 1; i < n; i++)
				if (a[i] < item)
					pos += 1;
			while (item == a[pos])
				pos += 1;
			if (item != a[pos]) {
				swap(item, a[pos]);
				writes++;
			}
		}
	}
}

template<class T, class C>
void cyclesort(T _First, T _Last, C compare) {
	cyclesort_(_First, std::distance(_First, _Last), compare);
}

template<class T>
void cyclesort(T _First, T _Last) {
	cyclesort(_First, _Last, std::less<>());
}


























