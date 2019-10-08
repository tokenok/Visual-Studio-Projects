#include "bignum.h"
#include <sstream>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>
#include <queue>
#define sdb bignum

//private
sdb sdb::zero = 0ul;

sdb::buffer_type sdb::conv_ulong(unsigned long n) {
	if (n == 0)
		return buffer_type(1, 0);

	buffer_type t;
	t.reserve(sizeof(n) * 3);

	while (n) {
		t.push_back(n % 10);
		n /= 10;
	}

	return t;
}
sdb::buffer_type sdb::conv_float(double x) {
	return conv_ulong((unsigned long)(std::abs(x)));
}
sdb::buffer_type sdb::conv_string(const string& s) {
	buffer_type t(1, 0);
	t.reserve(s.size());

	unsigned i = 0;
	if (s[0] == '+' || s[0] == '-')
		i = 1;

	while (std::isdigit(s[i]))
		t.push_back(s[i++] - '0');

	std::reverse(t.begin(), t.end());
	return t;
}

sdb::buffer_type& sdb::remove_zero(buffer_type& b) {
	while (b.size() > 1 && b[b.size() - 1] == 0)
		b = b.substr(0, b.size() - 1);
	return b;
}

sdb::reference sdb::normalize() {
	// leading 0
	remove_zero(buffer);

	// +-0
	if (buffer.size() == 1 && buffer[0] == 0)
		sign_ = false;

	return *this;
}

void sdb::overflow(unsigned start) {
	for (unsigned i = start; i < buffer.size() && buffer[i] >= 10; i++) {
		buffer[i] %= 10;
		if (i == buffer.size() - 1)
			buffer.push_back(1);
		else
			buffer[i + 1]++;
	}
}
void sdb::underflow(unsigned start) {
	for (unsigned i = start; i < buffer.size() && buffer[i] < 0; i++) {
		buffer[i] += 10;
		if (i == buffer.size() - 1)
			return; // something weird happened
		buffer[i + 1]--;
	}
}
// constructors
sdb::sdb(unsigned long n): sign_(false), buffer(conv_ulong(n)) {
	normalize();
}
sdb::sdb(signed long n) : sign_(n < 0), buffer(conv_ulong(std::abs(n))) {
	normalize();
}
sdb::sdb(int n) : sign_(n < 0), buffer(conv_ulong(std::abs(n))) {
	normalize();
}
sdb::sdb(double x) : sign_(x < 0), buffer(conv_float(x)) {
	normalize();
}
sdb::sdb(const string& s) : sign_(false), buffer(conv_string(s)) {
	if (s[0] == '-')
		sign_ = true;
	normalize();
}
sdb::sdb(const char* s) : sign_(false), buffer(conv_string(s)) {
	if (s[0] == '-')
		sign_ = true;
	normalize();
}

// assignment
sdb::reference   sdb::operator= (unsigned long n) {
	sign_ = false;
	buffer = conv_ulong(n);
	return normalize();
}
sdb::reference   sdb::operator= (signed long n) {
	sign_ = n < 0;
	buffer = conv_ulong(std::abs(n));
	return normalize();
}
sdb::reference   sdb::operator= (int n) {
	return *this = long(n);
}
sdb::reference   sdb::operator= (string s) {
	sign_ = s[0] == '-';
	buffer = conv_string(s);
	return normalize();
}
sdb::reference   sdb::operator= (const char* s) {
	return *this = string(s);
}
sdb::reference   sdb::operator= (double x) {
	sign_ = x < 0;
	buffer = conv_float(x);
	return normalize();
}

// conversion
signed long sdb::slong() const {
	// out of range ?

	signed long t = 0, e = 1;

	for (unsigned long i = 0; i < buffer.size(); i++, e *= 10)
		t += buffer[i] * e;

	return t * sign();
}
unsigned long sdb::ulong() const {
	unsigned long t = 0, e = 1;

	for (unsigned long i = 0; i < buffer.size(); i++, e *= 10)
		t += buffer[i] * e;

	return t;
}
sdb::string sdb::str() const {
	string r(buffer.size() + 1, 0);
	r[0] = sign_ ? '-' : '+';
	for (unsigned i = 1; i <= buffer.size(); i++)
		r[i] = buffer[buffer.size() - i] + '0';

	if (!sign_)
		r.erase(0, 1);

	return r;
}

// sign
sdb::value sdb::operator-() const {
	if (operator== (zero))
		return *this;

	sdb t(*this);
	t.sign_ = !t.sign_;
	return t;
}
signed sdb::sign() const {
	if (operator== (zero))
		return 0;
	return sign_ ? -1 : 1;
}
sdb::reference sdb::neg() {
	if (operator!= (zero))
		sign_ = !sign_;
	return *this;
}
//arithmetic
sdb::reference sdb::operator++ () {
	if (sign_) {
		abs();
		operator-- ();
		return neg();
	}

	buffer[0]++; // *should* always exist

	overflow(0);

	return normalize();
}
sdb::reference sdb::operator-- () {
	if (sign_ || operator==(zero)) {
		abs();
		operator++ ();
		return neg();
	}

	buffer[0]--;

	underflow(0);

	return normalize();
}
sdb::reference sdb::operator+= (const_reference o) {
	if (sign_) {
		abs();
		operator-= (o);
		return neg();
	}
	else if (o.sign_) {
		return operator-= (-o);
	}

	if (buffer.size() < o.buffer.size())
		buffer.resize(o.buffer.size());

	for (unsigned i = 0; i < o.buffer.size(); i++) {
		buffer[i] += o.buffer[i];
		if (buffer[i] >= 10) {
			buffer[i] -= 10;
			if (i >= buffer.size() - 1)
				buffer.push_back(1);
			else
				buffer[i + 1]++;
		}
	}
	overflow(o.buffer.size() - 1);
	return normalize();
}

sdb::reference   sdb::operator-= (const_reference o) {
	if (sign_) {
		abs();
		operator+= (o);
		return neg();
	}
	else if (o.sign_) {
		return operator+= (-o);
	}
	else if (*this < o) {
		sdb t = o;
		t -= *this;
		return *this = -t;
	}

	for (unsigned i = 0; i < o.buffer.size(); i++) {
		buffer[i] -= o.buffer[i];
		if (buffer[i] < 0) {
			buffer[i] += 10;
			buffer[i + 1]--; // [i+1] exists because o <= *this ( or so I hope )
		}
	}

	underflow(o.buffer.size() - 1);

	return normalize();
}

sdb::reference sdb::multiply_by_digit(digit_type d) {
	if (d == 0)
		return *this = zero;
	if (d != 1) {
		for (unsigned i = 0; i < buffer.size(); i++)
			buffer[i] *= d;

		// check/fix overflow
		for (unsigned i = 0; i < buffer.size(); i++)
			if (buffer[i] >= 10) {
				digit_type n = buffer[i] / 10;
				buffer[i] %= 10;
				if (i >= buffer.size() - 1)
					buffer.push_back(n);
				else
					buffer[i + 1] += n;
			}
	}
	return *this;
}
sdb::reference sdb::operator*= (const_reference o) {
	if (o.buffer.size() == 1)
		return multiply_by_digit(o.buffer[0]);

	sign_ = sign_ != o.sign_;

	std::queue<sdb> partial;

	for (unsigned i = 0; i < o.buffer.size(); i++) {
		sdb copy = *this;
		copy.multiply_by_digit(o.buffer[i]);
		partial.push(copy);
	}

	sdb temp;
	*this = zero;
	for (unsigned i = 0; !partial.empty(); i++) {
		temp = partial.front();
		partial.pop();
		temp.buffer = buffer_type(i, 0) + temp.buffer;
		*this += temp;
	}
	return normalize();
}
sdb::reference sdb::operator/= (value o) {
	if (o == zero)
		throw std::domain_error("Division by zero");

	bool rsign = false;

	if (sign_) {
		abs();
		rsign = true;
	}
	if (o.sign_) {
		o.abs();
		rsign = !rsign;
	}

	sdb q = zero;
	while (*this >= 0) {
		operator-= (o);
		++q;
	}
	// now, *this + o = r
	--q;

	buffer = q.buffer;
	sign_ = rsign;

	return normalize();
}

sdb::value sdb::operator+ (const_reference o) {
	return sdb(*this) += o;
}
sdb::value sdb::operator- (const_reference o) {
	return sdb(*this) -= o;
}
sdb::value sdb::operator* (const_reference o) {
	return sdb(*this) *= o;
}
sdb::value sdb::operator/ (const_reference o) {
	return sdb(*this) /= o;
}

// comparison
bool sdb::operator<  (const_reference o) const {
	if (&o == &zero)
		return sign_;

	if (sign_ != o.sign_)
		return sign_;
	/*if (sign_ && o.sign_)
		return ::abs(*this) > ::abs(o);*/

	if (buffer.size() != o.buffer.size())
		return buffer.size() < o.buffer.size();

	for (long i = buffer.size() - 1; i >= 0; i--)
		if (buffer[i] != o.buffer[i])
			return buffer[i] < o.buffer[i];

	return false; // equal
}
bool sdb::operator<= (const_reference o) const {
	if (&o == &zero)
		return sign_;

	if (sign_ != o.sign_)
		return sign_;
	/*if (sign_ && o.sign_)
		return ::abs(*this) >= ::abs(o);*/

	if (buffer.size() != o.buffer.size())
		return buffer.size() < o.buffer.size();

	for (long i = buffer.size() - 1; i >= 0; i--)
		if (buffer[i] != o.buffer[i])
			return buffer[i] <= o.buffer[i];

	return true;
}
bool sdb::operator== (const_reference o) const {
	if (&o != &zero && sign_ != o.sign_)
		return false;

	if (buffer.size() != o.buffer.size())
		return false;

	for (long i = buffer.size() - 1; i >= 0; i--)
		if (buffer[i] != o.buffer[i])
			return false;

	return true;
}
bool sdb::operator!= (const_reference o) const {
	return !operator== (o);
}
bool sdb::operator>= (const_reference o) const {
	return !operator< (o);
}
bool sdb::operator>  (const_reference o) const {
	return !operator<= (o);
}


std::ostream& operator<< (std::ostream& os, const bignum& sdbn) {
	return os << sdbn.str();
}

std::istream& operator>> (std::istream& is, bignum& sdbn) {
	std::string t;
	is >> t;
	sdbn = t;
	return is;
}