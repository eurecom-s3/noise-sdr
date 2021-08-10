// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of fldigi
//
// fldigi is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

//Android #include <config.h>

#include "pskcoeff.h"

// Linux PSK31 modem driver for soundcard -- Filter coefficients
//
// these FIR coefs are those used by G0TJZ in his TMC320C50 code
//
// Hansi Reiser, DL9RDZ, 20 April 1998
//

#include <math.h>


// 64-tap raised-cosine FIR
// implements
//  u[n] = (1.0 - cos(2PI * n / 64))/128.0
// used in gmfsk, twpsk etc.

double gmfir1c[64] = {
	0.000000, //0
	0.000038, //1
	0.000150, //2
	0.000336, //3
	0.000595, //4
	0.000922, //5
	0.001317, //6
	0.001773, //7
	0.002288, //8
	0.002856, //9
	0.003472, //10
	0.004130, //11
	0.004823, //12
	0.005545, //13
	0.006288, //14
	0.007047, //15
	0.007812, //16
	0.008578, //17
	0.009337, //18
	0.010080, //19
	0.010802, //20
	0.011495, //21
	0.012153, //22
	0.012769, //23
	0.013337, //24
	0.013852, //25
	0.014308, //26
	0.014703, //27
	0.015030, //28
	0.015289, //29
	0.015475, //30
	0.015587, //31
	0.015625, //32
	0.015587, //33
	0.015475, //34
	0.015289, //35
	0.015030, //36
	0.014703, //37
	0.014308, //38
	0.013852, //39
	0.013337, //40
	0.012769, //41
	0.012153, //42
	0.011495, //43
	0.010802, //44
	0.010080, //45
	0.009337, //46
	0.008578, //47
	0.007813, //48
	0.007047, //49
	0.006288, //50
	0.005545, //51
	0.004823, //52
	0.004130, //53
	0.003472, //54
	0.002856, //55
	0.002288, //56
	0.001773, //57
	0.001317, //58
	0.000922, //59
	0.000595, //60
	0.000336, //61
	0.000150, //62
	0.000038, //63
};

// 4-bit receive filter for 31.25 baud BPSK
// Designed by G3PLX
//

double gmfir2c[64] = {
	0.000625000,
	0.000820912,
	0.001374651,
	0.002188141,
	0.003110600,
	0.003956273,
	0.004526787,
	0.004635947,
	0.004134515,
	0.002932456,
	0.001016352,
	-0.001539947,
	-0.004572751,
	-0.007834665,
	-0.011009254,
	-0.013733305,
	-0.015625000,
	-0.016315775,
	-0.015483216,
	-0.012882186,
	-0.008371423,
	-0.001933193,
	0.006315933,
	0.016124399,
	0.027115485,
	0.038807198,
	0.050640928,
	0.062016866,
	0.072333574,
	0.081028710,
	0.087617820,
	0.091728168,
	0.093125000,
	0.091728168,
	0.087617820,
	0.081028710,
	0.072333574,
	0.062016866,
	0.050640928,
	0.038807198,
	0.027115485,
	0.016124399,
	0.006315933,
	-0.001933193,
	-0.008371423,
	-0.012882186,
	-0.015483216,
	-0.016315775,
	-0.015625000,
	-0.013733305,
	-0.011009254,
	-0.007834665,
	-0.004572751,
	-0.001539947,
	0.001016352,
	0.002932456,
	0.004134515,
	0.004635947,
	0.004526787,
	0.003956273,
	0.003110600,
	0.002188141,
	0.001374651,
	0.000820912
};


// sync filter
// weighting for sync samples
// sum of all weights = 1.0

double syncfilt[16] = {
	-0.097545161,
	-0.093796555,
	-0.086443400,
	-0.075768274,
	-0.062181416,
	-0.046204960,
	-0.028452874,
	-0.009607360,
	0.009607360,
	0.028452874,
	0.046204960,
	0.062181416,
	0.075768274,
	0.086443400,
	0.093796555,
	0.097545161
};

// experimental filters (higher precision)
// identical to the G0TJZ filter but with double precision
void raisedcosfilt(double *firc)
{
	for (int i = 0; i < 64; i++)
		firc[i] = (1.0 - cos(M_PI * i / 32.0))/128.0;
}

void wsincfilt(double *firc, double fc, bool blackman)
{
	double normalize = 0;
// sin(x-tau)/(x-tau)	
	for (int i = 0; i < 64; i++)
		if (i == 32)
			firc[i] = 2.0 * M_PI * fc;
		else
			firc[i] = sin(2*M_PI*fc*(i - 32))/(i-32);
// blackman window
	if (blackman)
		for (int i = 0; i < 64; i++)
			firc[i] = firc[i] * (0.42 - 0.5 * cos(2*M_PI*i/64) + 0.08 * cos(4*M_PI*i/64));
// hamming window
	else
		for (int i = 0; i < 64; i++)
			firc[i] = firc[i] * (0.54 - 0.46 * cos(2*M_PI*i/64));
// normalization factor
	for (int i = 0; i < 64; i++)
		normalize += firc[i];
// normalize the filter
	for (int i = 0; i < 64; i++)
		firc[i] /= normalize;
}


//Android: 32 taps version of the filter instead of the 64
//Key factor in CPU load reduction (for "slowcpu" option).
void wsincfilt32(double *firc, double fc, bool blackman)
    {
        double normalize = 0;
// Math.sin(x-tau)/(x-tau)
        for (int i = 0; i < 32; i++)
            if (i == 16)
                firc[i] = 2.0 * M_PI * fc;
            else
                firc[i] = (sin(2*M_PI*fc*(i - 16)))/(i-16);
// blackman window
        if (blackman)
            for (int i = 0; i < 32; i++)
                firc[i] = firc[i] * (0.42 - 0.5 * cos(2*M_PI*i/32) + 0.08 * cos(4*M_PI*i/32));
// hamming window
        else
            for (int i = 0; i < 32; i++)
                firc[i] = firc[i] * (0.54 - 0.46 * cos(2*M_PI*i/32));
// normalization factor
        for (int i = 0; i < 32; i++)
            normalize += firc[i];
// normalize the filter
        for (int i = 0; i < 32; i++)
            firc[i] /= normalize;
    }

