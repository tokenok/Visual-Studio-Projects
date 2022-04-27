#pragma once

/**
	**  SmoothSort function template + helper functions.
	**
	**    Formal type T should have a comparison operator >= with prototype:
	**
	**      bool T::operator >= (const T &) const throw ();
	**
	**    which should compare its arguments by the given relation
	**     (possibly taking advantage of the type itself).
	**
	**
	**/



	/**  Sort an array in ascending order.  **/
template <typename T, typename C>
void smoothsort(T, unsigned, C) throw ();



namespace

	/**
	 **  Helper function's local namespace (declarations).
	 **
	 **/

{
	class LeonardoNumber

		/**
		 **  Helper class for manipulation of Leonardo numbers
		 **
		 **/

	{
	public:
		/**  Default ctor.  **/
		LeonardoNumber(void) throw () : b(1), c(1) { return; }

		/**  Copy ctor.  **/
		LeonardoNumber(const LeonardoNumber & _l) throw () : b(_l.b), c(_l.c) { return; }

		/**
		 **  Return the "gap" between the actual Leonardo number and the
		 **  preceding one.
		 **/
		unsigned gap(void) const throw () { return b - c; }


		/**  Perform an "up" operation on the actual number.  **/
		LeonardoNumber & operator ++ (void) throw () { unsigned s = b; b = b + c + 1; c = s; return *this; }

		/**  Perform a "down" operation on the actual number.  **/
		LeonardoNumber & operator -- (void) throw () { unsigned s = c; c = b - c - 1; b = s; return *this; }

		/**  Return "companion" value.  **/
		unsigned operator ~ (void) const throw () { return c; }

		/**  Return "actual" value.  **/
		operator unsigned(void) const throw () { return b; }


	private:
		unsigned b;   /**  Actual number.  **/
		unsigned c;   /**  Companion number.  **/
	};


	/**  Perform a "sift up" operation.  **/
	template <typename T, typename C>
	inline void sift(T, unsigned, LeonardoNumber, C) throw ();

	/**  Perform a "semi-trinkle" operation.  **/
	template <typename T, typename C>
	inline void semitrinkle(T, unsigned, unsigned long long, LeonardoNumber, C) throw ();

	/**  Perform a "trinkle" operation.  **/
	template <typename T, typename C>
	inline void trinkle(T, unsigned, unsigned long long, LeonardoNumber, C) throw ();
}

template <typename T, typename C>
void smoothsort(T _m, unsigned _n, C compare) throw ()

/**
 **  Sorts the given array in ascending order.
 **
 **    Usage: smoothsort (<array>, <size>)
 **
 **    Where: <array> pointer to the first element of the array in question.
 **            <size> length of the array to be sorted.
 **
 **
 **/

{
	//	if (!(_m && _n)) return;

	unsigned long long p = 1;
	LeonardoNumber b;

	for (unsigned q = 0; ++q < _n; ++p)
		if (p % 8 == 3) {
			sift<T, C>(_m, q - 1, b, compare);

			++++b; p >>= 2;
		}

		else if (p % 4 == 1) {
			if (q + ~b < _n)  sift<T, C>(_m, q - 1, b, compare);
			else  trinkle<T, C>(_m, q - 1, p, b, compare);

			for (p <<= 1; --b > 1; p <<= 1);
		}

	trinkle<T, C>(_m, _n - 1, p, b, compare);

	for (--p; _n-- > 1; --p)
		if (b == 1)
			for (; !(p % 2); p >>= 1)  ++b;

		else if (b >= 3) {
			if (p)  semitrinkle<T, C>(_m, _n - b.gap(), p, b, compare);

			--b; p <<= 1; ++p;
			semitrinkle<T, C>(_m, _n - 1, p, b, compare);
			--b; p <<= 1; ++p;
		}


	return;
}

namespace

	/**
	 **  Helper function's local namespace (definitions).
	 **
	 **/

{
	template <typename T, typename C>
	inline void sift(T _m, unsigned _r, LeonardoNumber _b, C compare) throw ()

		/**
		 **  Sifts up the root of the stretch in question.
		 **
		 **    Usage: sift (<array>, <root>, <number>)
		 **
		 **    Where:     <array> Pointer to the first element of the array in
		 **                       question.
		 **                <root> Index of the root of the array in question.
		 **              <number> Current Leonardo number.
		 **
		 **
		 **/

	{
		unsigned r2;

		while (_b >= 3) {
			if (!compare(_m[_r - _b.gap()], _m[_r - 1]))
				r2 = _r - _b.gap();
			else { r2 = _r - 1; --_b; }

			if (!compare(_m[_r], _m[r2]))  break;
			else { iter_swap((_m + _r), (_m + r2)); _r = r2; --_b; }
		}


		return;
	}


	template <typename T, typename C>
	inline void semitrinkle(T _m, unsigned _r, unsigned long long _p,
		LeonardoNumber _b, C compare) throw ()

		/**
		 **  Trinkles the roots of the stretches of a given array and root when the
		 **  adjacent stretches are trusty.
		 **
		 **    Usage: semitrinkle (<array>, <root>, <standard_concat>, <number>)
		 **
		 **    Where:           <array> Pointer to the first element of the array in
		 **                             question.
		 **                      <root> Index of the root of the array in question.
		 **           <standard_concat> Standard concatenation's codification.
		 **                    <number> Current Leonardo number.
		 **
		 **
		 **/

	{
		if (!compare(_m[_r - ~_b], _m[_r])) {
			iter_swap((_m + _r), (_m + (_r - ~_b)));
			trinkle<T, C>(_m, _r - ~_b, _p, _b, compare);
		}


		return;
	}


	template <typename T, typename C>
	inline void trinkle(T _m, unsigned _r, unsigned long long _p,
		LeonardoNumber _b, C compare) throw ()

		/**
		 **  Trinkles the roots of the stretches of a given array and root.
		 **
		 **    Usage: trinkle (<array>, <root>, <standard_concat>, <number>)
		 **
		 **    Where:           <array> Pointer to the first element of the array in
		 **                             question.
		 **                      <root> Index of the root of the array in question.
		 **           <standard_concat> Standard concatenation's codification.
		 **                    <number> Current Leonardo number.
		 **
		 **
		 **/

	{
		while (_p) {
			for (; !(_p % 2); _p >>= 1)  ++_b;

			if (!--_p || !compare(_m[_r], _m[_r - _b]))  break;
			else
				if (_b == 1) { iter_swap((_m + _r), (_m + _r - _b)); _r -= _b; }

				else if (_b >= 3) {
					unsigned r2 = _r - _b.gap(), r3 = _r - _b;

					if (!compare(_m[_r - 1], _m[r2])) { r2 = _r - 1; _p <<= 1; --_b; }

					if (!compare(_m[r3], _m[r2])) { iter_swap((_m + _r), (_m + r3)); _r = r3; }

					else { iter_swap((_m + _r), (_m + r2)); _r = r2; --_b; break; }
				}
		}

		sift<T, C>(_m, _r, _b, compare);


		return;
	}
}

template<class T, class C>
inline void smoothsort(T _First, T _Last, C compare) {
	smoothsort(_First, (unsigned)std::distance(_First, _Last), compare);
}

template<class T>
inline void smoothsort(T _First, T _Last) {
	smoothsort(_First, _Last, std::less<>());
}


























