// ----------------------------------------------------------------------------
// complex.h  --  Complex arithmetic
//
// Copyright (C) 2006-2008
//		Dave Freese, W1HKJ
// Copyright (C) 2008
//		Stelios Bounanos, M0GLD
//
// This file is part of fldigi.
//
// Fldigi is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fldigi.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#ifndef _COMPLEX_H
#define _COMPLEX_H

#include <cmath>

class cmplx {
public:
	double re;
	double im;
	cmplx(double r = 0.0, double i = 0.0)
	    : re(r), im(i) { }

	double real() { return re; };
	void real(double R) {re = R;};
	double imag() { return im; };
	void imag(double I) {im = I;};

// Z = X * Y
	cmplx& operator*=(const cmplx& y) {
		double temp = re * y.re - im * y.im;
		im = re * y.im + im * y.re;
		re = temp;
		return *this;
	}
	cmplx operator*(const cmplx& y) const {
		return cmplx(re * y.re - im * y.im,  re * y.im + im * y.re);
	}

// Z = X * y
	cmplx& operator*=(double y) {
		re *= y;
		im *= y;
		return *this;
	}
	cmplx operator*(double y) const {
		return cmplx(re * y, im * y);
	}

// Z = X + Y
	cmplx& operator+=(const cmplx& y) {
		re += y.re;
		im += y.im;
		return *this;
        }
	cmplx operator+(const cmplx& y) const {
		return cmplx(re + y.re,  im + y.im);
	}

// Z = X - Y
	cmplx& operator-=(const cmplx& y) {
		re -= y.re;
		im -= y.im;
		return *this;
	}
	cmplx operator-(const cmplx& y) const {
		return cmplx(re - y.re,  im - y.im);
	}

// Z = X / Y
	cmplx& operator/=(const cmplx& y) {
		double temp, denom = y.re*y.re + y.im*y.im;
		if (denom == 0.0) denom = 1e-10;
		temp = (re * y.re + im * y.im) / denom;
		im = (im * y.re - re * y.im) / denom;
		re = temp;
		return *this;
	}
	cmplx operator/(const cmplx& y) const {
		double denom = y.re*y.re + y.im*y.im;
		if (denom == 0.0) denom = 1e-10;
		return cmplx((re * y.re + im * y.im) / denom,  (im * y.re - re * y.im) / denom);
	}

// Z = (cmplx conjugate of X) * Y
// Z1 = x1 + jy1, or Z1 = |Z1|exp(jP1)
// Z2 = x2 + jy2, or Z2 = |Z2|exp(jP2)
// Z = (x1 - jy1) * (x2 + jy2)
// or Z = |Z1|*|Z2| exp (j (P2 - P1))
	cmplx& operator%=(const cmplx& y) {
		double temp = re * y.re + im * y.im;
		im = re * y.im - im * y.re;
		re = temp;
		return *this;
	}
	cmplx operator%(const cmplx& y) const {
		cmplx z;
		z.re = re * y.re + im * y.im;
		z.im = re * y.im - im * y.re;
		return z;
	}

// n = |Z| * |Z|
	double norm() const {
		return (re * re + im * im);
	}

// n = |Z|
	double mag() const {
		return sqrt(norm());
	}

// Z = x + jy
// Z = |Z|exp(jP)
// arg returns P
	double arg() const {
		return atan2(im, re);
	}

};

inline 	cmplx cmac (const cmplx *a, const cmplx *b, int ptr, int len) {
		cmplx z;
		ptr %= len;
		for (int i = 0; i < len; i++) {
			z += a[i] * b[ptr];
			ptr = (ptr + 1) % len;
		}
		return z;
	}


#endif
