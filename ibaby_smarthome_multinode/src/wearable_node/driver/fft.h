/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * \version 2017.06
 * \date 2017-06-26
 * \author dbHu(wwmhu@outlook.com)
--------------------------------------------- */
/**
 * \file
 * \ingroup	BOARD_EMSK_FFT
 * \brief	header file of Fast fourier transform function
 */

/**
 * \addtogroup	BOARD_EMSK_FFT
 * @{
 */
#ifndef _fft_H_
#define _fft_H_

typedef struct
{ 
  float real;
  float imag;
}compx;

#define dtT             0.02

#define FFT_M 			9
#define FFT_LEN 		(1 << FFT_M)
#define num_taps		FFT_LEN			


#define S16MAX	 32767
#define S16MIN	-32767
#define PI 3.1415926535897932385

typedef struct _Cplx16
{
    int R;
    int I;
} Cplx16;

extern Cplx16 x[FFT_LEN];
extern Cplx16 w[FFT_LEN / 2];

extern void calc_w(Cplx16 *W);
extern void fft(Cplx16 *D, Cplx16 *W);
extern float find_max(Cplx16 *D);

#endif /* _EMSK_FFT_H_ */

/** @} end of group BOARD_EMSK_FFT */





