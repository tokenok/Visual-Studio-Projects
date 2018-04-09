#ifndef bignum_H
#define bignum_H
#include <string>
#include <iosfwd>

class bignum {
	private:

	typedef signed char                     digit_type;
	typedef std::basic_string<digit_type>   buffer_type;
	typedef bignum           value;
	typedef bignum&          reference;
	typedef const bignum&    const_reference;
	typedef std::string                     string;

	static bignum zero;

	static buffer_type& remove_zero(buffer_type&);

	bool sign_;
	buffer_type buffer;

	static buffer_type conv_ulong(unsigned long);
	static buffer_type conv_float(double);
	static buffer_type conv_string(const string&);

	void overflow(unsigned begin = 0);
	void underflow(unsigned begin = 0);
	reference normalize();
	reference multiply_by_digit(digit_type);


	public:

	// constructors
	bignum(unsigned long = 0ul);
	bignum(signed long);
	bignum(int);
	bignum(double);
	bignum(const string&);
	bignum(const char*);

	// assignment
	reference   operator= (unsigned long);
	reference   operator= (signed long);
	reference   operator= (int);
	reference   operator= (string);
	reference   operator= (const char*);
	reference   operator= (double);

	// conversion
	signed long slong() const;
	unsigned long ulong() const;
	string      str() const;

	// sign
	signed      sign() const;
	reference   abs() { return sign_ = false, *this; }
	friend value abs(value n) { return n.abs(); }
	reference   neg();
	value       operator-() const;

	// arithmetic
	reference   operator+= (const_reference);
	reference   operator-= (const_reference);
	reference   operator*= (const_reference);
	reference   operator/= (value);

	reference operator++ ();
	reference operator-- ();

	value       operator+ (const_reference);
	value       operator- (const_reference);
	value       operator* (const_reference);
	value       operator/ (const_reference);

	// comparison
	bool        operator<  (const_reference) const;
	bool        operator<= (const_reference) const;
	bool        operator== (const_reference) const;
	bool        operator!= (const_reference) const;
	bool        operator>= (const_reference) const;
	bool        operator>  (const_reference) const;

};

// stream
std::ostream& operator<< (std::ostream& os, const bignum& sdbn);
std::istream& operator>> (std::istream& is, const bignum& sdbn);

#endif // bignum_H 