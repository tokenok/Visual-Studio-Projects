#define ENABLE_REGIONSSORT 0

#if ENABLE_REGIONSSORT

#ifndef _S_GRAPHBASED_INCLUDED
#define _S_GRAPHBASED_INCLUDED
#include <math.h>

#include <atomic>
#include <thread>

//#define ILP_SORT
#define MAXP 100
#define MAXBITS 100
#ifndef MAX_RADIX
#define MAX_RADIX 8

#define PARALLEL_FOR_THRESHOLD 4000

#endif
#if MAX_RADIX > 8
typedef uint16_t radixCustomType;
#elif MAX_RADIX <= 8 
typedef uint8_t radixCustomType;
#endif

#define BUCKETS (1 << MAX_RADIX)
#define MAXDEPTH (MAXBITS/MAX_RADIX)

#ifndef BLOCK_DIVIDE
#define BLOCK_DIVIDE 800000
#endif

#ifndef K_BOUND
#define K_BOUND 20000
#endif

#define SERIAL_THRESHOLD 20000


#define INSERTION_COARSEN

#define INSERTION_THRESHOLD 32

#define newA(__E,__n) (__E*) malloc((__n)*sizeof(__E))

typedef int sizeT;
typedef int intT;
typedef unsigned char uint8_t;
typedef unsigned int uintT;

#define parallel_for_swap for 
#define parallel_for for
#define parallel_for_1 for

static int getWorkers() { return 1; }

#define cilk_spawn
#define cilk_sync

long global_K;
long global_N;

namespace utils {
template <class E>
struct addF { E operator() (const E& a, const E& b) const { return a + b; } };
template <class E>
struct identityF { E operator() (const E& x) { return x; } };
template <class T>
static int log2Up(T i) {
	int a = 0;
	T b = i - 1;
	while (b != 0) { b = b >> 1; a++; }
	return a;
}
template <class E1, class E2>
struct firstF { E1 operator() (std::pair<E1, E2> a) { return a.first; } };
}

#define _BSIZE 2048
#define _SCAN_LOG_BSIZE 10
#define _SCAN_BSIZE (1 << _SCAN_LOG_BSIZE)

// This code is part of the Problem Based Benchmark Suite (PBBS)
// Copyright (c) 2011 Guy Blelloch and the PBBS team
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights (to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

template <class T>
struct _seq {
	T* A;
	long n;
	_seq() { A = NULL; n = 0; }
	_seq(T* _A, long _n) : A(_A), n(_n) {}
	void del() { free(A); }
};

template <class E>
void brokenCompiler__(intT n, E* x, E v) {
	parallel_for(intT i = 0; i<n; i++) x[i] = v;
}

template <class E>
static E* newArray(intT n, E v) {
	E* x = (E*)malloc(n * sizeof(E));
	brokenCompiler__(n, x, v);
	return x;
}

namespace sequence {

template <class intT>
struct boolGetA {
	bool* A;
	boolGetA(bool* AA) : A(AA) {}
	intT operator() (intT i) { return (intT)A[i]; }
};

template <class ET, class intT>
struct getA {
	ET* A;
	getA(ET* AA) : A(AA) {}
	ET operator() (intT i) { return A[i]; }
};

template <class IT, class OT, class intT, class F>
struct getAF {
	IT* A;
	F f;
	getAF(IT* AA, F ff) : A(AA), f(ff) {}
	OT operator () (intT i) { return f(A[i]); }
};

#define nblocks(_n,_bsize) (1 + ((_n)-1)/(_bsize))

#define granular_for(_i, _st, _ne, _thresh, _body) { \
  if ((_ne - _st) > _thresh) { \
    {parallel_for(intT _i=_st; _i < _ne; _i++) { \
      _body \
    }} \
  } else { \
    {for (intT _i=_st; _i < _ne; _i++) { \
      _body \
    }} \
  } \
  }

#define blocked_for(_i, _s, _e, _bsize, _body)  {	\
    intT _ss = _s;					\
    intT _ee = _e;					\
    intT _n = _ee-_ss;					\
    intT _l = nblocks(_n,_bsize);			\
    parallel_for (intT _i = 0; _i < _l; _i++) {		\
      intT _s = _ss + _i * (_bsize);			\
      intT _e = std::min(_s + (_bsize), _ee);			\
      _body						\
	}						\
  }

template <class OT, class intT, class F, class G>
OT reduceSerial(intT s, intT e, F f, G g) {
	OT r = g(s);
	for (intT j = s + 1; j < e; j++) r = f(r, g(j));
	return r;
}

template <class OT, class intT, class F, class G>
OT reduce(intT s, intT e, F f, G g) {
	intT l = nblocks(e - s, _SCAN_BSIZE);
	if (l <= 1) return reduceSerial<OT>(s, e, f, g);
	OT *Sums = newA(OT, l);
	blocked_for(i, s, e, _SCAN_BSIZE,
				Sums[i] = reduceSerial<OT>(s, e, f, g););
	OT r = reduce<OT>((intT)0, l, f, getA<OT, intT>(Sums));
	free(Sums);
	return r;
}

template <class OT, class intT, class F>
OT reduce(OT* A, intT n, F f) {
	return reduce<OT>((intT)0, n, f, getA<OT, intT>(A));
}

template <class OT, class intT, class F>
OT reduce(OT* A, intT s, intT e, F f) {
	return reduce<OT>(s, e, f, getA<OT, intT>(A));
}

template <class OT, class intT>
OT plusReduce(OT* A, intT n) {
	return reduce<OT>((intT)0, n, utils::addF<OT>(), getA<OT, intT>(A));
}

template <class intT>
intT sum(bool *In, intT n) {
	return reduce<intT>((intT)0, n, utils::addF<intT>(), boolGetA<intT>(In));
}

// g is the map function (applied to each element)
// f is the reduce function
// need to specify OT since it is not an argument
template <class OT, class IT, class intT, class F, class G>
OT mapReduce(IT* A, intT n, F f, G g) {
	return reduce<OT>((intT)0, n, f, getAF<IT, OT, intT, G>(A, g));
}

template <class ET, class intT, class F, class G>
intT maxIndexSerial(intT s, intT e, F f, G g) {
	ET r = g(s);
	intT k = 0;
	for (intT j = s + 1; j < e; j++) {
		ET v = g(j);
		if (f(v, r)) { r = v; k = j; }
	}
	return k;
}

template <class ET, class intT, class F, class G>
intT maxIndex(intT s, intT e, F f, G g) {
	intT l = nblocks(e - s, _SCAN_BSIZE);
	if (l <= 2) return maxIndexSerial<ET>(s, e, f, g);
	else {
		intT *Idx = newA(intT, l);
		blocked_for(i, s, e, _SCAN_BSIZE,
					Idx[i] = maxIndexSerial<ET>(s, e, f, g););
		intT k = Idx[0];
		for (intT j = 1; j < l; j++)
			if (f(g(Idx[j]), g(k))) k = Idx[j];
		free(Idx);
		return k;
	}
}

template <class ET, class intT, class F>
intT maxIndex(ET* A, intT n, F f) {
	return maxIndex<ET>((intT)0, n, f, getA<ET, intT>(A));
}

template <class ET, class intT, class F, class G>
ET scanSerial(ET* Out, intT s, intT e, F f, G g, ET zero, bool inclusive, bool back) {
	ET r = zero;

	if (inclusive) {
		if (back) for (long i = e - 1; i >= s; i--) Out[i] = r = f(r, g(i));
		else for (intT i = s; i < e; i++) Out[i] = r = f(r, g(i));
	}
	else {
		if (back)
			for (long i = e - 1; i >= s; i--) {
				ET t = g(i);
				Out[i] = r;
				r = f(r, t);
			}
		else
			for (intT i = s; i < e; i++) {
				ET t = g(i);
				Out[i] = r;
				r = f(r, t);
			}
	}
	return r;
}

template <class ET, class intT, class F>
ET scanSerial(ET *In, ET* Out, intT n, F f, ET zero) {
	return scanSerial(Out, (intT)0, n, f, getA<ET, intT>(In), zero, false, false);
}

// back indicates it runs in reverse direction
template <class ET, class intT, class F, class G>
ET scan(ET* Out, intT s, intT e, F f, G g, ET zero, bool inclusive, bool back) {
	intT n = e - s;
	intT l = nblocks(n, _SCAN_BSIZE);
	if (l <= 2) return scanSerial(Out, s, e, f, g, zero, inclusive, back);
	ET *Sums = newA(ET, nblocks(n, _SCAN_BSIZE));
	blocked_for(i, s, e, _SCAN_BSIZE,
				Sums[i] = reduceSerial<ET>(s, e, f, g););
	ET total = scan(Sums, (intT)0, l, f, getA<ET, intT>(Sums), zero, false, back);
	blocked_for(i, s, e, _SCAN_BSIZE,
				scanSerial(Out, s, e, f, g, Sums[i], inclusive, back););
	free(Sums);
	return total;
}

template <class ET, class intT, class F>
ET scan(ET *In, ET* Out, intT n, F f, ET zero) {
	return scan(Out, (intT)0, n, f, getA<ET, intT>(In), zero, false, false);
}

template <class ET, class intT, class F>
ET scanBack(ET *In, ET* Out, intT n, F f, ET zero) {
	return scan(Out, (intT)0, n, f, getA<ET, intT>(In), zero, false, true);
}

template <class ET, class intT, class F>
ET scanI(ET *In, ET* Out, intT n, F f, ET zero) {
	return scan(Out, (intT)0, n, f, getA<ET, intT>(In), zero, true, false);
}

template <class ET, class intT, class F>
ET scanIBack(ET *In, ET* Out, intT n, F f, ET zero) {
	return scan(Out, (intT)0, n, f, getA<ET, intT>(In), zero, true, true);
}

template <class ET, class intT>
ET plusScan(ET *In, ET* Out, intT n) {
	return scan(Out, (intT)0, n, utils::addF<ET>(), getA<ET, intT>(In),
		(ET)0, false, false);
}

template <class intT>
intT enumerate(bool *In, intT* Out, intT n) {
	return scan(Out, (intT)0, n, utils::addF<intT>(), boolGetA<intT>(In),
		(intT)0, false, false);
}


#define _F_BSIZE (2*_SCAN_BSIZE)

// sums a sequence of n boolean flags
// an optimized version that sums blocks of 4 booleans by treating
// them as an integer
// Only optimized when n is a multiple of 512 and Fl is 4byte aligned
template <class intT>
intT sumFlagsSerial(bool *Fl, intT n) {
	intT r = 0;
	if (n >= 128 && (n & 511) == 0 && ((long)Fl & 3) == 0) {
		int* IFl = (int*)Fl;
		for (int k = 0; k < (n >> 9); k++) {
			int rr = 0;
			for (int j = 0; j < 128; j++) rr += IFl[j];
			r += (rr & 255) + ((rr >> 8) & 255) + ((rr >> 16) & 255) + ((rr >> 24) & 255);
			IFl += 128;
		}
	}
	else for (intT j = 0; j < n; j++) r += Fl[j];
	return r;
}

template <class intT>
inline bool checkBit(long* Fl, intT i) {
	return Fl[i / 64] & ((long)1 << (i % 64));
}

template<class intT>
intT sumBitFlagsSerial(long* Fl, intT s, intT e) {
	intT res = 0;
	while (s % 64 && s < e) {
		if (checkBit(Fl, s)) ++res;
		s++;
	}
	if (s == e)
		return res;
	while (e % 64) {
		if (checkBit(Fl, e - 1)) ++res;
		e--;
	}
	// Do the rest with popcount
	intT be = e / 64;
	intT bs = s / 64;
	for (intT i = bs; i < be; ++i) {
		res += _popcnt64(Fl[i]);
	}
	return res;
}


template <class ET, class intT, class F>
_seq<ET> packSerial(ET* Out, bool* Fl, intT s, intT e, F f) {
	if (Out == NULL) {
		intT m = sumFlagsSerial(Fl + s, e - s);
		Out = newA(ET, m);
	}
	intT k = 0;
	for (intT i = s; i < e; i++) if (Fl[i]) Out[k++] = f(i);
	return _seq<ET>(Out, k);
}

template <class ET, class intT, class F>
void packSerial01(ET* Out0, ET* Out1, long* Fl, intT s, intT e, F f) {
	if (Out0 == NULL) {
		intT m = e - s - sumBitFlagsSerial(Fl, e, s);
		Out0 = newA(ET, m);
	}
	if (Out1 == NULL) {
		intT m = sumBitFlagsSerial(Fl, e, s);
		Out1 = newA(ET, m);
	}
	intT k0 = 0;
	intT k1 = 0;
	for (intT i = s; i < e; ++i) {
		if (checkBit(Fl, i))
			Out1[k1++] = f(i);
		else
			Out0[k0++] = f(i);
	}
}

template <class ET, class intT, class F>
void packSerial0(ET* Out, long* Fl, intT s, intT e, F f) {
	if (Out == NULL) {
		intT m = e - s - sumBitFlagsSerial(Fl, e, s);
		Out = newA(ET, m);
	}
	intT k = 0;
	for (intT i = s; i < e; i++) {
		if (!checkBit(Fl, i)) {
			Out[k++] = f(i);
		}
	}
}

template <class ET, class intT, class F>
void packSerial1(ET* Out, long* Fl, intT s, intT e, F f) {
	if (Out == NULL) {
		intT m = sumBitFlagsSerial(Fl, e, s);
		Out = newA(ET, m);
	}
	intT k = 0;
	for (intT i = s; i < e; i++) if (checkBit(Fl, i)) Out[k++] = f(i);
}

template <class T>
T prefixSumSerial(T* data, intT s, intT e) {
	T res = 0;
	for (intT i = s; i < e; ++i) {
		res += data[i];
		data[i] = res - data[i];
	}
	return res;
}

template <class T>
void addSerial(T* data, intT s, intT e, T val) {
	for (intT i = s; i < e; ++i)
		data[i] += val;
}

template <class T>
T prefixSum(T* data, intT s, intT e) {
	intT l = nblocks(e - s, _SCAN_BSIZE);
	if (l <= 1) return prefixSumSerial(data, s, e);
	T* sums = newA(T, l);
	blocked_for(i, s, e, _SCAN_BSIZE,
				sums[i] = prefixSumSerial<T>(data, s, e););
	T res = prefixSumSerial(sums, 0, l);
	blocked_for(i, s, e, _SCAN_BSIZE,
				addSerial(data, s, e, sums[i]););
	return res;
}

template <class ET, class intT, class F>
_seq<ET> pack(ET* Out, bool* Fl, intT s, intT e, F f) {
	intT l = nblocks(e - s, _F_BSIZE);
	if (l <= 1) return packSerial(Out, Fl, s, e, f);
	intT *Sums = newA(intT, l);
	blocked_for(i, s, e, _F_BSIZE, Sums[i] = sumFlagsSerial(Fl + s, e - s););
	intT m = plusScan(Sums, Sums, l);
	if (Out == NULL) Out = newA(ET, m);
	blocked_for(i, s, e, _F_BSIZE, packSerial(Out + Sums[i], Fl, s, e, f););
	free(Sums);
	return _seq<ET>(Out, m);
}

template <class ET, class intT, class F>
void splitSerial(ET* OutFalse, ET* OutTrue, bool* Fl, intT s, intT e, F f) {
	intT kT = 0;
	intT kF = 0;
	for (intT i = s; i < e; i++)
		if (Fl[i]) OutTrue[kT++] = f(i);
		else OutFalse[kF++] = f(i);
}

// Given a boolean array, splits so false (0) elements are at the bottom
// and true (1) elements are at the top of the output (of lenght e-s).
// As usual s is a start index, e is an end index and
// f is a function of type [s,e-1) -> ET
template <class ET, class intT, class F>
int split(ET* Out, bool*  Fl, intT s, intT e, F f) {
	intT l = nblocks(e - s, _F_BSIZE);
	intT *SumsTrue = newA(intT, l);
	blocked_for(i, s, e, _F_BSIZE, SumsTrue[i] = sumFlagsSerial(Fl + s, e - s););
	intT numTrue = plusScan(SumsTrue, SumsTrue, l);
	intT numFalse = (e - s) - numTrue;
	ET* OutTrue = Out + numFalse;
	blocked_for(i, s, e, _F_BSIZE,
				splitSerial(Out + _F_BSIZE * i - SumsTrue[i],
							OutTrue + SumsTrue[i],
							Fl, s, e, f););
	free(SumsTrue);
	return numFalse;
}

template <class ET, class intT, class F>
std::pair<_seq<ET>, _seq<ET> > pack2(ET* Out, bool* Fl1, bool* Fl2,
								intT s, intT e, F f) {
	intT l = nblocks(e - s, _F_BSIZE);
	intT *Sums1 = newA(intT, l);
	intT *Sums2 = newA(intT, l);
	blocked_for(i, s, e, _F_BSIZE,
				Sums1[i] = sumFlagsSerial(Fl1 + s, e - s);
	Sums2[i] = sumFlagsSerial(Fl2 + s, e - s););
	intT m1 = plusScan(Sums1, Sums1, l);
	intT m2 = plusScan(Sums2, Sums2, l);
	ET* Out1;
	ET* Out2;
	if (Out == NULL) {
		Out1 = newA(ET, m1);
		Out2 = newA(ET, m2);
	}
	else {
		Out1 = Out;
		Out2 = Out + m1;
	}
	blocked_for(i, s, e, _F_BSIZE,
				packSerial(Out1 + Sums1[i], Fl1, s, e, f);
	packSerial(Out2 + Sums2[i], Fl2, s, e, f););
	free(Sums1); free(Sums2);
	return std::pair<_seq<ET>, _seq<ET> >(_seq<ET>(Out1, m1), _seq<ET>(Out2, m2));
}
// Custom pack2 to be used with bitvector as flags (used for example for wavelet trees)
template <class ET, class intT, class F>
std::pair<_seq<ET>, _seq<ET> > pack2(ET* Out, long* Fl, intT s, intT e, F f) {
	// If interval empty
	if (s >= e)
		return std::pair<_seq<ET>, _seq<ET> >(_seq<ET>(Out, 0), _seq<ET>(Out, 0));
	intT l = nblocks(e - s, _F_BSIZE);
	intT *Sums1 = newA(intT, l);
	intT *Sums2 = newA(intT, l);
	blocked_for(i, s, e, _F_BSIZE,
				Sums2[i] = sumBitFlagsSerial(Fl, s, e); // count ones
	Sums1[i] = (e - s - Sums2[i]);); // calculate zeros
	intT m1 = plusScan(Sums1, Sums1, l);
	intT m2 = plusScan(Sums2, Sums2, l);
	ET* Out1;
	ET* Out2;
	if (Out == NULL) {
		Out1 = newA(ET, m1);
		Out2 = newA(ET, m2);
	}
	else {
		Out1 = Out;
		Out2 = Out + m1;
	}
	blocked_for(i, s, e, _F_BSIZE,
				packSerial01(Out1 + Sums1[i], Out2 + Sums2[i], Fl, s, e, f););
	//packSerial0(Out1+Sums1[i], Fl, s, e, f);
	//packSerial1(Out2+Sums2[i], Fl, s, e, f););
	free(Sums1); free(Sums2);
	return std::pair<_seq<ET>, _seq<ET> >(_seq<ET>(Out1, m1), _seq<ET>(Out2, m2));
}

template <class ET, class intT>
intT pack(ET* In, ET* Out, bool* Fl, intT n) {
	return pack(Out, Fl, (intT)0, n, getA<ET, intT>(In)).n;
}

template <class ET, class intT>
intT split(ET* In, ET* Out, bool* Fl, intT n) {
	return split(Out, Fl, (intT)0, n, getA<ET, intT>(In));
}

template <class ET, class intT>
std::pair<intT, intT> pack2(ET* In, ET* Out, bool* Fl1, bool* Fl2, intT n) {
	std::pair<_seq<ET>, _seq<ET> > r;
	r = pack2(Out, Fl1, Fl2, (intT)0, n, getA<ET, intT>(In));
	return std::pair<intT, intT>(r.first.n, r.second.n);
}

// Custom pack which takes an input and one flag array and puts all elements where 0 is set on the left side and alle the other elements on to the right
template <class ET, class intT>
intT pack2Bit(ET* In, ET* Out, long* Flags, intT s, intT e) {
	std::pair<_seq<ET>, _seq<ET> > r;
	r = pack2(Out, Flags, s, e, getA<ET, intT>(In));
	return r.first.n;
}

template <class ET, class intT>
_seq<ET> pack(ET* In, bool* Fl, intT n) {
	return pack((ET*)NULL, Fl, (intT)0, n, getA<ET, intT>(In));
}

template <class OT, class intT, class F>
_seq<OT> packMap(bool* Fl, intT n, F& f) {
	return pack((OT*)NULL, Fl, (intT)0, n, f);
}

template <class intT>
intT packIndex(intT* Out, bool* Fl, intT n) {
	return pack(Out, Fl, (intT)0, n, utils::identityF<intT>()).n;
}

template <class intT>
_seq<intT> packIndex(bool* Fl, intT n) {
	return pack((intT *)NULL, Fl, (intT)0, n, utils::identityF<intT>());
}

template <class ET, class intT, class PRED>
intT filterSerial(ET* In, ET* Out, intT n, PRED p) {
	intT k = 0;
	for (intT i = 0; i < n; i++)
		if (p(In[i])) Out[k++] = In[i];
	return k;
}

template <class ET, class intT, class PRED>
intT filter(ET* In, ET* Out, intT n, PRED p) {
	if (n < _F_BSIZE)
		return filterSerial(In, Out, n, p);
	bool *Fl = newA(bool, n);
	parallel_for(intT i = 0; i < n; i++) Fl[i] = (bool)p(In[i]);
	intT  m = pack(In, Out, Fl, n);
	free(Fl);
	return m;
}

// Avoids reallocating the bool array
template <class ET, class intT, class PRED>
intT filter(ET* In, ET* Out, bool* Fl, intT n, PRED p) {
	if (n < _F_BSIZE)
		return filterSerial(In, Out, n, p);
	parallel_for(intT i = 0; i < n; i++) Fl[i] = (bool)p(In[i]);
	intT  m = pack(In, Out, Fl, n);
	return m;
}

template <class ET, class intT, class PRED>
_seq<ET> filter(ET* In, intT n, PRED p) {
	bool *Fl = newA(bool, n);
	parallel_for(intT i = 0; i < n; i++) Fl[i] = (bool)p(In[i]);
	_seq<ET> R = pack(In, Fl, n);
	free(Fl);
	return R;
}

// Faster for a small number in output (about 40% or less)
// Destroys the input.   Does not need a bool array.
template <class ET, class intT, class PRED>
intT filterf(ET* In, ET* Out, intT n, PRED p) {
	intT b = _F_BSIZE;
	if (n < b)
		return filterSerial(In, Out, n, p);
	intT l = nblocks(n, b);
	b = nblocks(n, l);
	intT *Sums = newA(intT, l + 1);
	{
		parallel_for(intT i = 0; i < l; i++) {
			intT s = i * b;
			intT e = min(s + b, n);
			intT k = s;
			for (intT j = s; j < e; j++)
				if (p(In[j])) In[k++] = In[j];
			Sums[i] = k - s;
		}
	}
	intT m = plusScan(Sums, Sums, l);
	Sums[l] = m;
	{
		parallel_for(intT i = 0; i < l; i++) {
			ET* I = In + i * b;
			ET* O = Out + Sums[i];
			for (intT j = 0; j < Sums[i + 1] - Sums[i]; j++) {
				O[j] = I[j];
			}
		}
	}
	free(Sums);
	return m;
}

}

class SimpleBlock {
public:
	sizeT start;
	sizeT size;
	long counts[BUCKETS];
	long bucketEnds[BUCKETS];
	SimpleBlock() {

	}
	void init(sizeT offset, sizeT end) {
		this->start = offset;
		this->size = end - offset;
		for (int i = 0; i < BUCKETS; i++) {
			counts[i] = 0;
		}
	}
};

template <class E, class F>
void sortSimpleBlock(E* A, SimpleBlock *block, F extract) {

	struct metaData meta(extract._offset, true, false, block->counts);
	ska_sort(A + block->start, A + block->start + block->size, extract._f, meta);
	block->bucketEnds[0] = block->start + block->counts[0];
	for (int i = 1; i < BUCKETS; i++) {
		block->bucketEnds[i] = block->counts[i] + block->bucketEnds[i - 1];
	}
}

class Edge {
public:
	sizeT amount;
	sizeT start;
	int from;
	int to;

	Edge() {
		this->start = 0;
		this->amount = 0;
		this->from = 0;
		this->to = 0;
	}

	Edge(sizeT start, sizeT amount, int from, int to) {
		this->start = start;
		this->amount = amount;
		this->from = from;
		this->to = to;
	}



	void setEmpty() {
		amount = 0;
	}

	bool isTrivial() {
		return (from == to) || (amount <= 0);
	}

	void printEdge() {
#ifdef LONG_ARRAY
		printf("Edge: start: %ld, amount %ld, from %d -> to %d\n", start, amount, from, to);
#else
		printf("Edge: start: %d, amount %d, from %d -> to %d\n", start, amount, from, to);
#endif
	}

	bool compareEdge(Edge *anotherEdge) {
		if (this->from != anotherEdge->from) {
			return false;
		}
		if (this->to != anotherEdge->to) {
			return false;
		}
		if (this->amount != anotherEdge->amount) {
			return false;
		}
		if (this->start != anotherEdge->start) {
			return false;
		}
		return true;
	}
};

#define MAX_PARALLEL_EDGES (MAXP * BUCKETS + BUCKETS + 5)

struct Triangle {

	int from;
	int to;
	sizeT offset1;
	sizeT offset2;
	sizeT amount;
	Triangle() {

	}

	void setEmpty() {
		amount = 0;
	}
	bool isEmpty() {
		return amount == 0;
	}

	bool isTrivial() {
		return amount == 0 || to == from;
	}
	void printTriangle() {
		if (isEmpty()) {
			printf("Empty Triangle\n");
		}
#ifndef LONG_ARRAY
		printf("Triangle: (%d -> %d) amount %d \n", from, to, amount);
		printf("offset1 : %d offset2 %d\n", offset1, offset2);
#endif
	}
};

//template<class sizeT>
void makeTriangle(Triangle  *triangle, Edge  *firstEdge, Edge * secondEdge, sizeT firstRemaining, sizeT secondRemaining) {

	triangle->from = firstEdge->from;

	triangle->to = secondEdge->to;

	triangle->offset1 = firstEdge->start + firstEdge->amount - firstRemaining;
	triangle->offset2 = secondEdge->start + secondEdge->amount - secondRemaining;
	sizeT amount = std::min(firstRemaining, secondRemaining);
	triangle->amount = amount;
}

void makeTrianglefromDynamicEdge(Triangle  *triangle, Edge  *firstEdge, Edge * secondEdge, sizeT amount) {
	triangle->from = firstEdge->from;
	triangle->to = secondEdge->to;
	triangle->offset1 = firstEdge->start;
	triangle->offset2 = secondEdge->start;
	triangle->amount = amount;
}

template <class E>
void serialExecuteTriangle(E *A, Triangle  *triangle) {
	E* start = A + triangle->offset2;
	std::swap_ranges(start, start + triangle->amount, A + triangle->offset1);
}

template <class E>
void parallelExecuteTriangle(E *A, Triangle  *triangle) {
	parallel_for_swap(sizeT index = 0; index < triangle->amount; index++) {
		sizeT from = triangle->offset2 + index;
		sizeT to = triangle->offset1 + index;
		E temp = A[from];
		A[from] = A[to];
		A[to] = temp;
	}

}

template <class E>
void executeTriangle(E *A, Triangle  *triangle) {

	if (triangle->amount < PARALLEL_FOR_THRESHOLD) {
		serialExecuteTriangle(A, triangle);
	}
	else {
		parallelExecuteTriangle(A, triangle);
	}

}

void getPrefixSum(std::vector<uint8_t> &input, std::vector<uint8_t> &res) {
	long sum = 0;
	for (int i = 0; i < input.size(); i++) {
		res[i] = sum;
		sum += input[i];
	}
	res[input.size()] = sum;
}

void getPrefixSum(uint8_t *input, uint8_t *res, int size) {
	long sum = 0;
	for (int i = 0; i < size; i++) {
		res[i] = sum;
		sum += input[i];
	}
	res[size] = sum;
}

void getSerialPrefixSum(Edge  ** input, sizeT *res, int size) {
	sizeT sum = 0;
	for (int i = 0; i < size; i++) {
		res[i] = sum;
		sum += input[i]->amount;
	}
	res[size] = sum;
}


void getParallelPrefixSum(Edge  ** input, sizeT *res, int size) {
	parallel_for_swap(int i = 0; i < size; i++) {
		res[i] = input[i]->amount;
	}
	long total = sequence::plusScan(res, res, size);
	res[size] = total;
}


void getSerialPrefixSumReal(Edge  * input, sizeT *res, int size) {
	sizeT sum = 0;
	for (int i = 0; i < size; i++) {
		res[i] = sum;
		sum += input[i].amount;
	}
	res[size] = sum;
}

void getParallelPrefixSumReal(Edge  * input, sizeT *res, int size) {
	parallel_for_swap(int i = 0; i < size; i++) {
		res[i] = input[i].amount;
	}
	sizeT total = sequence::plusScan(res, res, size);
	res[size] = total;
}

void getPrefixSum(std::vector<Edge*> &input, std::vector<sizeT> &res) {
	sizeT sum = 0;
	for (int i = 0; i < input.size(); i++) {
		res.push_back(sum);
		sum += input[i]->amount;
	}
	res.push_back(sum);
}

void getPrefixSumLong(sizeT *input, sizeT *res, int size) {
	long sum = 0;
	for (int i = 0; i < size; i++) {
		res[i] = sum;
		sum += input[i];
	}
	res[size] = sum;
}

void getPrefixSum(sizeT offset, sizeT* count, sizeT* prefixSum, int length) {
	prefixSum[0] = offset + count[0];
	for (int i = 1; i < length; i++) {
		prefixSum[i] = count[i] + prefixSum[i - 1];
	}
}

void printPrefixSum(std::vector<sizeT> ps) {
#ifdef LONG_ARRAY
	printf("ps size %lu\n", ps.size());
	for (int i = 0; i < ps.size(); i++) {
		printf("%ld |", ps[i]);
	}
#else
	printf("ps size %lu\n", ps.size());
	for (int i = 0; i < ps.size(); i++) {
		printf("%d |", ps[i]);
	}
#endif
	printf("\n");
}

class CycleGraph {

public:
	std::vector<Edge > graphFromEdges[BUCKETS];


	CycleGraph() {
		for (int bucket = 0; bucket < BUCKETS; bucket++) {
			graphFromEdges[bucket].clear();
		}
	}

	void addEdge(Edge  *e) {
		if (e->isTrivial()) {
			return;
		}
		graphFromEdges[e->from].push_back(*e);
	}

	Edge * getNextEdge(int node) {
		int sz = graphFromEdges[node].size();
		while (sz && graphFromEdges[node][sz - 1].amount == 0) {
			sz--;
		}
		if (sz == 0) {
			return NULL;
		}
		graphFromEdges[node].resize(sz);
		return &graphFromEdges[node][sz - 1];
	}

	bool hasEdges(int node) {
		return getNextEdge(node) != NULL;
	}
	class CyclePlan {
	public:
		int length;
		sizeT starts[BUCKETS];
		sizeT amount;
		CyclePlan() {
			length = 0;
		}
		template <class E>
		void executeCycle(E* A) {
			if (amount < PARALLEL_FOR_THRESHOLD) {
				for (sizeT pos = 0; pos < amount; pos++) {
					E save = A[starts[length - 1] + pos];
					for (int i = length - 1; i > 0; i--) {
						A[starts[i] + pos] = A[starts[i - 1] + pos];
					}
					A[starts[0] + pos] = save;
				}
			}
			else {
				parallel_for_swap(sizeT pos = 0; pos < amount; pos++) {
					E save = A[starts[length - 1] + pos];
					for (int i = length - 1; i > 0; i--) {
						A[starts[i] + pos] = A[starts[i - 1] + pos];
					}
					A[starts[0] + pos] = save;
				}
			}
		}
	};
	class Cycle {
	public:
		Edge * path[BUCKETS];
		int visited[BUCKETS];
		int pathIndex;
		sizeT amount;
		int cycle_start;
		int cycle_length;
		int currentNode;
		CyclePlan cyclePlan;
		Cycle(int node) {
			currentNode = node;
			pathIndex = 0;
			cycle_start = 0;
			cycle_length = 0;
			amount = 0;
			for (int i = 0; i < BUCKETS; i++) {
				visited[i] = -1;
			}
		}

		bool getNextCycle(CycleGraph *graphcycle, CyclePlan *cyclePlan) {
			if (!graphcycle->hasEdges(currentNode)) {
				//we are done with this node.
				return false;
			}

			do {
				visited[currentNode] = pathIndex;
				path[pathIndex] = graphcycle->getNextEdge(currentNode);
				currentNode = path[pathIndex]->to;
				pathIndex++;
			} while (visited[currentNode] == -1);



			cycle_start = visited[currentNode];
			amount = path[cycle_start]->amount;
			for (int i = cycle_start; i < pathIndex; i++) {
				amount = std::min(amount, path[i]->amount);
				visited[path[i]->from] = -1;
				cyclePlan->starts[i - cycle_start] = path[i]->start;
			}
			cyclePlan->amount = amount;
			cyclePlan->length = pathIndex - cycle_start;
			cycle_length = pathIndex - cycle_start;
			pathIndex = cycle_start;
			return true;
		}

		void consumeCycle() {
			parallel_for(int i = cycle_start; i < cycle_start + cycle_length; i++) {
				path[i]->amount -= amount;
				path[i]->start += amount;
			}
		}

	};

	void createCycleGraph(int buckets, sizeT P, SimpleBlock *blocks, const long *countryEnds) {
		int currentCountry = 0;
		sizeT currentRegionStart = 0;
		for (sizeT currentBlock = 0; currentBlock < P; currentBlock++) {
			for (int currentValue = 0; currentValue < BUCKETS; currentValue++) {
				while (currentCountry < BUCKETS) {
					if (blocks[currentBlock].bucketEnds[currentValue] <= countryEnds[currentCountry]) {
						//if country is a superset of the current bucket inside current block.
						//both sides of comparison are exclusive.

						sizeT currentRegionEnd = blocks[currentBlock].bucketEnds[currentValue];
						Edge  edge = Edge(currentRegionStart, currentRegionEnd - currentRegionStart, currentCountry, currentValue);
						addEdge(&edge);
						currentRegionStart = currentRegionEnd;
						break;
					}
					else {
						// some parts of the bucket is not included in the country.
						// Region will include the intersection of the country and the bucket.
						sizeT currentRegionEnd = countryEnds[currentCountry];
						Edge  edge = Edge(currentRegionStart, currentRegionEnd - currentRegionStart, currentCountry, currentValue);
						addEdge(&edge);
						currentRegionStart = currentRegionEnd;
						currentCountry++;
					}
				}
			}
		}

	}
	void printGraph() {
		printf("No Printing Function!\n");
	}

	void verifyGraph(int node) {
		std::vector<Edge *> toEdges;
		std::vector<Edge *> fromEdges;
		sizeT toSum = 0;
		for (int i = 0; i < toEdges.size(); i++) {
			if (toEdges[i]->isTrivial())
				continue;
			toSum += toEdges[i]->amount;
		}
		sizeT fromSum = 0;
		for (int i = 0; i < fromEdges.size(); i++) {
			if (fromEdges[i]->isTrivial())
				continue;
			fromSum += fromEdges[i]->amount;
		}
		if (toSum != fromSum) {
			printf("Graph Invariants don't hold!\n");
			std::vector<sizeT> toPrefixSum;
			std::vector<sizeT> fromPrefixSum;
			getPrefixSum(toEdges, toPrefixSum);
			getPrefixSum(fromEdges, fromPrefixSum);
			printPrefixSum(toPrefixSum);
			printPrefixSum(fromPrefixSum);
			while (1);
		}

	}


};

class EdgeListGraph {

public:
	std::vector< Edge > graphToEdges[BUCKETS];
	std::vector< Edge > graphFromEdges[BUCKETS];
	int * order;
	int * rank;

	EdgeListGraph(int P, int *rank, int *order) {
		for (int bucket = 0; bucket < BUCKETS; bucket++) {
			graphToEdges[bucket].clear();
			graphFromEdges[bucket].clear();
		}
		this->rank = rank;
		this->order = order;
	}

	void addEdge(Edge  *e) {
		if (e->isTrivial()) {
			return;
		}


		if (rank[e->to] < rank[e->from]) {
			graphToEdges[e->to].push_back(*e);
		}
		else {
			graphFromEdges[e->from].push_back(*e);
		}
	}

	void getFromEdgesSubgraph(int node, std::vector< Edge *> &res) {
		res.clear();
		for (int i = 0; i < graphFromEdges[node].size(); i++) {
			res.push_back(&graphFromEdges[node][i]);
		}
	}

	void getToEdgesSubgraph(int node, std::vector< Edge  *> &res) {
		res.clear();
		for (int i = 0; i < graphToEdges[node].size(); i++) {
			res.push_back(&graphToEdges[node][i]);
		}
	}

	void printGraph() {
		printf("Printing Graph!\n");
		printf("Edges Counts\n");
		std::vector < Edge  *> parallelFromEdges;
		std::vector < Edge  *> parallelToEdges;
		for (int node = 0; node < BUCKETS; node++) {
			printf("vector for node %d\n", node);
			this->getFromEdgesSubgraph(node, parallelFromEdges);
			this->getToEdgesSubgraph(node, parallelToEdges);
		}

	}


	void createParallelGraph(long buckets, long P, SimpleBlock *blocks, const long *countryEnds) {
		int currentCountry = 0;
		long currentRegionStart = 0;
		for (int currentBlock = 0; currentBlock < P; currentBlock++) {
			for (int currentValue = 0; currentValue < BUCKETS; currentValue++) {

				while (currentCountry < BUCKETS) {
					if (blocks[currentBlock].bucketEnds[currentValue] <= countryEnds[currentCountry]) {
						//if country is a superset of the current bucket inside current block.
						//both sides of comparison are exclusive.

						long currentRegionEnd = blocks[currentBlock].bucketEnds[currentValue];
						Edge  edge = Edge(currentRegionStart, currentRegionEnd - currentRegionStart, currentCountry, currentValue);



						addEdge(&edge);
						currentRegionStart = currentRegionEnd;
						break;
					}
					else {
						// some parts of the bucket is not included in the country.
						// Region will include the intersection of the country and the bucket.
						long currentRegionEnd = countryEnds[currentCountry];

						Edge  edge = Edge(currentRegionStart, currentRegionEnd - currentRegionStart, currentCountry, currentValue);
						addEdge(&edge);
						currentRegionStart = currentRegionEnd;
						currentCountry++;
					}
				}
			}
		}

	}

	static sizeT getAmount(Edge  * e) {
		return e->amount;
	}

	static sizeT getAmount(Edge  &e) {
		return e.amount;
	}

	static  Edge * getL(Edge  *e) {
		return e;
	}
	static  Edge * getL(Edge  &e) {
		return &e;
	}

	void matchEdgesToTriangle(Edge * fromEdge, Edge  * toEdge, Triangle * result) {
		sizeT toRemaining = toEdge->amount;
		sizeT fromRemaining = fromEdge->amount;
		sizeT min;
		if (toRemaining < fromRemaining) {
			min = toRemaining;
		}
		else {
			min = fromRemaining;
		}
		makeTrianglefromDynamicEdge(result, toEdge, fromEdge, min);
		toEdge->amount -= min;
		toEdge->start += min;
		fromEdge->amount -= min;
		fromEdge->start += min;
	}


	template<class T>
	int extractTriangles(std::vector<T> &fromRegions, int fromSize, std::vector<T> &toRegions, int toSize, Triangle * triangles, int triangles_count) {

		if (fromSize == 0 || toSize == 0) {
			return triangles_count;
		}

		int fromP = 0;
		int toP = 0;

		while (true) {

			for (; toP < toSize && !getAmount(toRegions[toP]); toP++) {

			}

			for (; fromP < fromSize && !getAmount(fromRegions[fromP]); fromP++) {

			}

			if (toP == toSize || fromP == fromSize) {
				break;
			}

			matchEdgesToTriangle(getL(fromRegions[fromP]), getL(toRegions[toP]), &triangles[triangles_count]);
			triangles_count = triangles_count + 1;
		}

		return triangles_count;
	}


	int extractTriangles(std::vector< Edge > &fromRegions, std::vector< Edge > &toRegions, Triangle * triangles, int triangles_count) {
		return extractTriangles(fromRegions, (int)fromRegions.size(), toRegions, (int)toRegions.size(), triangles, triangles_count);

	}

	int extractTriangles(std::vector< Edge *> &fromRegions, std::vector< Edge *> &toRegions, Triangle * triangles, int triangles_count) {
		return extractTriangles(fromRegions, (int)fromRegions.size(), toRegions, (int)toRegions.size(), triangles, triangles_count);

	}

	int getEdgesCount(int node) {
		return graphToEdges[node].size() + graphFromEdges[node].size();
	}

	void deleteNode(int node) {
		std::vector< Edge >().swap(graphToEdges[node]);
		std::vector< Edge >().swap(graphFromEdges[node]);
	}



	void extractNode2(int node, Triangle * triangles, int* triangles_count) {
		std::vector< Edge > fromRegions;
		fromRegions = graphFromEdges[node];
		std::vector< Edge > toRegions;
		toRegions = graphToEdges[node];


#ifdef EXTRACT_2CYCLES
		///////////////////////EXTRACTING two-cycles START/////////////////////////
		vector< Edge *> edgesPerNodeFrom[BUCKETS];
		vector< Edge *> edgesPerNodeTo[BUCKETS];
		for (int i = 0; i < fromRegions.size(); i++) {
			edgesPerNodeFrom[fromRegions[i].to].push_back(&fromRegions[i]);
		}

		for (int i = 0; i < toRegions.size(); i++) {
			edgesPerNodeTo[toRegions[i].from].push_back(&toRegions[i]);
		}

		if (toRegions.size() < BUCKETS - (rank[node] + 1)) {
			for (int i = 0; i < toRegions.size(); i++) {
				int bucket = toRegions[i].from;
				*triangles_count = extractTriangles(edgesPerNodeFrom[bucket], edgesPerNodeTo[bucket], triangles, *triangles_count);
				edgesPerNodeFrom[bucket].clear();
				edgesPerNodeTo[bucket].clear();
			}

		}
		else {
			if (fromRegions.size() < BUCKETS - (rank[node] + 1)) {
				for (int i = 0; i < fromRegions.size(); i++) {
					int bucket = fromRegions[i].to;
					*triangles_count = extractTriangles(edgesPerNodeFrom[bucket], edgesPerNodeTo[bucket], triangles, *triangles_count);
					edgesPerNodeFrom[bucket].clear();
					edgesPerNodeTo[bucket].clear();
				}
			}
			else {
				for (int index = rank[node] + 1; index < BUCKETS; index++) {
					int bucket = order[index];
					*triangles_count = extractTriangles(edgesPerNodeFrom[bucket], edgesPerNodeTo[bucket], triangles, *triangles_count);
				}
			}
		}
		///////////////////////EXTRACTING two-cycles END/////////////////////////
#endif
		*triangles_count = extractTriangles(fromRegions, toRegions, triangles, *triangles_count);
	}
	void consumeTriangle(Triangle  *triangle) {

		if (triangle->isTrivial()) {
			return;
		}

		Edge  edge(triangle->offset1, triangle->amount, triangle->from, triangle->to);

		if (rank[triangle->to] < rank[triangle->from]) {
			graphToEdges[triangle->to].push_back(edge);
		}
		else {
			graphFromEdges[triangle->from].push_back(edge);
		}
	}

	~EdgeListGraph() {
		for (int bucket = 0; bucket < BUCKETS; bucket++) {
			graphToEdges[bucket].clear();
			graphFromEdges[bucket].clear();
		}
	}

};


template <class E, class F>
void radixSortOneLevel(E* A, sizeT n, int doneOffset, F f, sizeT processors, int depth);

struct SortedBucket {
	int bucket;
	sizeT size;
};

sizeT sortedBucketSize(SortedBucket sb) {
	return -sb.size;
}

template <class E, class F>
void insertion_sort(E* A, long n, F f) {
	for (int i = 0; i < n; i++) {
		E x = A[i];
		int j = i - 1;
		while (j >= 0 && f(A[j]) > f(x)) {
			A[j + 1] = A[j];
			j--;
		}
		A[j + 1] = x;
	}
	return;
}

template< class _Type, typename F>
inline void insertionSortSimilarToSTLnoSelfAssignment(_Type* a, unsigned long a_size, F extract) {

	for (unsigned long i = 1; i < a_size; i++) {
		if (a[i] < a[i - 1])		// no need to do (j > 0) compare for the first iteration
		{
			_Type currentElement = a[i];
			auto compare = extract(currentElement);
			a[i] = a[i - 1];
			unsigned long j;
			for (j = i - 1; j > 0 && compare < extract(a[j - 1]); j--) {
				a[j] = a[j - 1];
			}
			a[j] = currentElement;	// always necessary work/write
		}
		  // Perform no work at all if the first comparison fails - i.e. never assign an element to itself!
	}
}

template <class E, class F, class Z>
bool multiBitSwapBasedSort(E *A, sizeT n, int buckets, sizeT K, long *internalCounts, F extract, Z f, int depth, int doneOffset) {

	if (n < SERIAL_THRESHOLD || K == 1) {
#ifdef INSERTION_COARSEN
		if (n < INSERTION_THRESHOLD) {
			insertionSortSimilarToSTLnoSelfAssignment(A, n, f);
			return false;
		}
#endif
		// if this is the last recursion, we don't care about counts of buckets
		if (extract._offset == 0) {
			internalCounts = NULL;
		}

#ifdef ILP_SORT  
		if (n > 1024) {
			struct metaData meta(extract._offset, true, false, internalCounts);
			ska_sort(A, A + n, f, meta);
			return meta.done;
		}
		else {
			_RadixSort_Unsigned_PowerOf2Radix_1(A, internalCounts, (long)n, BUCKETS, extract);
		}
		return true;
#else 
		return true;
#endif 


	}


	SimpleBlock *blocks = new SimpleBlock[K];
	parallel_for_1(int i = 0; i < K; i++) {
		unsigned long start = (long)i * n / K;
		unsigned long end = ((long)(i + 1) * n) / K;
		blocks[i].init(start, end);
		sortSimpleBlock(A, &blocks[i], extract);
	}

	parallel_for_1(int bucket = 0; bucket < BUCKETS; bucket++) {
		internalCounts[bucket] = 0;
		for (sizeT i = 0; i < K; i++) {
			internalCounts[bucket] += blocks[i].counts[bucket];
		}
	}

	long countryEnds[BUCKETS];

	countryEnds[0] = internalCounts[0];
	for (int i = 1; i < BUCKETS; i++) {
		countryEnds[i] = internalCounts[i] + countryEnds[i - 1];
	}

	int order[BUCKETS];
	int rank[BUCKETS];
	SortedBucket sbs[BUCKETS];
	for (int i = 0; i < BUCKETS; i++) {
		sbs[i].bucket = i;
		sbs[i].size = internalCounts[i];
	}
	insertion_sort(sbs, BUCKETS, sortedBucketSize);
	sizeT nextKs[BUCKETS];
	int countNonZero = 0;
	sizeT local = K;

	const long max_triangles = (K * BUCKETS + BUCKETS + 1) + BUCKETS;
	Triangle  * triangles = new Triangle[max_triangles];

	for (int i = 0; i < BUCKETS; i++) {
		order[i] = sbs[i].bucket;
		rank[order[i]] = i;
		sizeT count = internalCounts[order[i]];
		sizeT nextK = (sizeT)ceil((1.0 * (global_K * count) / global_N));
		nextKs[order[i]] = nextK;
		if (count) {
			countNonZero++;
		}
	}

#ifdef CYCLE
	CycleGraph cycleGraph;
	cycleGraph.createCycleGraph(BUCKETS, K, blocks, countryEnds);
	vector<CycleGraph::CyclePlan> cyclePlan(BUCKETS * K + BUCKETS + 1);

	for (int index = 0; index < countNonZero; index++) {
		int node = order[index];
		CycleGraph::Cycle cycle(node);
		int planc = 0;
		while (cycle.getNextCycle(&cycleGraph, &cyclePlan[planc++])) {
			cycle.consumeCycle();
		}
		parallel_for_1(int i = 0; i < planc - 1; i++) {
			cyclePlan[i].executeCycle(A);
		}
		sizeT startOfCountry;
		if (node == 0) {
			startOfCountry = 0;
		}
		else {
			startOfCountry = countryEnds[node - 1];
		}
		sizeT nextK = nextKs[node];
		if (internalCounts[node] < INSERTION_THRESHOLD) {
			radixSortOneLevel(A + startOfCountry, internalCounts[node], doneOffset, f, nextK, depth + 1);
		}
		else {
			cilk_spawn radixSortOneLevel(A + startOfCountry, internalCounts[node], doneOffset, f, nextK, depth + 1);
		}
	}
	cilk_sync;
	delete blocks;
	return false;
#else


	EdgeListGraph graph(K, rank, order);
	graph.createParallelGraph(BUCKETS, K, blocks, countryEnds);

	int triangles_count;


	for (int index = 0; index < countNonZero; index++) {
		int node = order[index];
		triangles_count = 0;
		graph.extractNode2(node, triangles, &triangles_count);
		parallel_for_1(int i = 0; i < triangles_count; i++) {
			executeTriangle(A, &triangles[i]);
		}

		sizeT startOfCountry;
		if (node == 0) {
			startOfCountry = 0;
		}
		else {
			startOfCountry = countryEnds[node - 1];
		}

		sizeT nextK = nextKs[node];
		if (internalCounts[node] < INSERTION_THRESHOLD) {

			radixSortOneLevel(A + startOfCountry, internalCounts[node], doneOffset, f, nextK, depth + 1);
		}
		else {
			cilk_spawn radixSortOneLevel(A + startOfCountry, internalCounts[node], doneOffset, f, nextK, depth + 1);

		}

		graph.deleteNode(node);

		for (int i = 0; i < triangles_count; i++) {
			graph.consumeTriangle(&triangles[i]);
		}

	}

	cilk_sync;

	delete triangles;
	delete blocks;
#endif
	return false;
}
#endif

template <class E, class F, class K>
K findMaxHelper(E* A, sizeT n, F f, sizeT P, K t) {

	K* temp = (K*)malloc(sizeof(E) * (P + 1));
	parallel_for(unsigned long i = 0; i <P; i++) {
		unsigned long start = i * (long)n / P;
		unsigned long end = ((long)(i + 1) * n) / P;
		K local_max = 0;
		for (; start < end; start++) {
			K current = f(A[start]);
			if (current > local_max) {
				local_max = current;
			}

		}
		temp[i] = local_max;
	}

	K global_max = temp[0];

	for (int i = 0; i < P; i++) {
		if (temp[i] > global_max) {
			global_max = temp[i];
		}
	}
	free(temp);
	return global_max;
}

template <class E, class F, class K>
K findMax(E* A, sizeT n, F f, K temp) {
	K maxV = findMaxHelper(A, n, f, (sizeT)1000 * getWorkers(), temp);
	return maxV;
}

int roundUpToRadixMultiple(int num) {
	if (num % MAX_RADIX == 0) {
		return num;
	}
	else {
		return ((num / MAX_RADIX) + 1)  * MAX_RADIX;
	}
}

int roundUpBits(int num) {

	int multipleOfRadix = roundUpToRadixMultiple(num);
#ifdef BITS_HACK
#if MAX_RADIX == 8
	if (multipleOfRadix >= num + 2) {
		return num + 2;
	}
#endif
#endif 
	return multipleOfRadix;
}

// a function to extract "bits" bits starting at bit location "offset"
template <class E, class F>
struct eBits {
	F _f;  int _mask;  intT _offset;
	eBits(int bits, intT offset, F f) : _mask((1 << bits) - 1),
		_offset(offset), _f(f) {}
	inline intT operator() (E p) { return (_mask&(_f(p)) >> _offset); }
};

template <class E, class F>
void radixSortOneLevel(E* A, sizeT n, int doneOffset, F f, sizeT K, int depth) {
	if (doneOffset <= 0 || n <= 1) {
		return;
	}

	int bits = MAX_RADIX;
	int buckets = BUCKETS;

	int start = std::max(doneOffset - bits, 0);


	bool needToRecurse = true;


	long localCounts[BUCKETS];
	needToRecurse = multiBitSwapBasedSort(A, n, buckets, K, localCounts, eBits<E, F>(doneOffset - start, start, f), f, depth, start);

	if (start <= 0)
		return;

	if (needToRecurse) {
		long sum = 0;
		for (int i = 0; i < BUCKETS; i++) {
			radixSortOneLevel(A + sum, localCounts[i], start, f, (sizeT)1, depth + 1);
			sum += localCounts[i];

		}

	}

	}

template <class E, class F>
void radixSortRoutine(sizeT index, E* A, sizeT n, int doneOffset, F f) {
	sizeT K;
#ifdef CYCLE
	K = getWorkers();
#else

	sizeT optimalCache = (n * sizeof(E)) / BLOCK_DIVIDE;
	if (optimalCache >  getWorkers() && optimalCache <= K_BOUND) {
		K = optimalCache;
	}
	else {
		if (optimalCache < getWorkers()) {
			K = getWorkers();
		}
		else {
			K = K_BOUND;
		}
	}

#endif
	global_K = K;
	global_N = n;

	radixSortOneLevel(A, n, doneOffset, f, K, 0);

};

template <class E, class F, class K>
void iSort(E *A, sizeT n, K m, F f) {
	int bits = roundUpBits(utils::log2Up(m));
	radixSortRoutine((sizeT)0, A, n, bits, f);
}

template <class T, class F>
static void parallelIntegerSort(T *A, sizeT n, F f) {
	T temp;
	T maxV = findMax(A, n, f, temp);
	iSort(A, n, maxV + 1, f);
}

template <class T>
static void parallelIntegerSort(T *A, sizeT n) {
	parallelIntegerSort(A, n, utils::identityF<T>());
}

//template <class T1, class T2, class F>
//void parallelIntegerSort(std::pair<T1, T2> *A, sizeT n, F f) {
//	T1 temp;
//	T1 maxV = findMax(A, n, f, temp);
//	iSort(A, n, maxV + 1, f);
//}
//
//template <class T1, class T2>
//void parallelIntegerSort(std::pair<T1, T2> *A, sizeT n) {
//	parallelIntegerSort(A, n, utils::firstF<T1, T2>());
//}

template<class T, class C>
inline void regionssort(T _First, T _Last, C compare) {
	parallelIntegerSort(&_First[0], std::distance(_First, _Last));
}

template<class T>
inline void regionssort(T _First, T _Last) {
	regionssort(_First, _Last, std::less<>());
}

#endif















