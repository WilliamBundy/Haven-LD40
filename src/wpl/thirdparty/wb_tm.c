/* 
 * This file has been modified and extended by William Bundy
 * and differs significantly from the original.
 * Changes:
 * 	- Removed non-SSE2 path. All 64 bit computers have SSE2 now.
 * 	- Added wrappers making it easy to call with a single f32.
 * 	- Added a couple functions not present in the original.
*/

/* SIMD (SSE1+MMX or SSE2) implementation of sin, cos, exp and log

   Inspired by intel Approximate Math library, and based on the
   corresponding algorithms of the cephes math library

   The default is to use the SSE1 version. If you define USE_SSE2 the
   the SSE2 intrinsics will be used in place of the MMX intrinsics. Do
   not expect any significant performance improvement with SSE2.
*/

/* Copyright (C) 2007  Julien Pommier

   This software is provided 'as-is', without any express or implied
   warranty.  In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
   3. This notice may not be removed or altered from any source distribution.

   (this is the zlib license)
*/
#pragma once


#define WBTM_API
#define WBTM_IMPLEMENTATION
#define WBTM_CRT_NO_REPLACE

#ifndef WBTM_NO_INCLUDE 
#include <intrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#endif

typedef __m128 vf128;
typedef __m128i vi128;
typedef float f32;
typedef int i32;


#ifdef WBTM_STATIC_IMPLEMENTATION
#define WBTM_IMPLEMENTATION
#define WBTM_API static inline
#endif 
#ifndef WBTM_API
#ifndef WBTM_IMPLEMENTATION
#define WBTM_API extern
#else 
#define WBTM_API
#endif
#endif

#ifndef WBTM_STATIC_IMPLEMENTATION

WBTM_API vf128 wbtm_sse2_log_ps(vf128 x);
WBTM_API vf128 wbtm_sse2_exp_ps(vf128 x);
WBTM_API vf128 wbtm_sse2_sin_ps(vf128 x);
WBTM_API vf128 wbtm_sse2_cos_ps(vf128 x);
WBTM_API void wbtm_sse2_sincos_ps(vf128 x, vf128* s, vf128* c);
WBTM_API vf128 wbtm_sse2_atan2_ps(vf128 y, vf128 x);
WBTM_API vf128 wbtm_sse2_sqrt_ps(vf128 x);
WBTM_API vf128 wbtm_sse2_rsqrt_ps(vf128 x);
WBTM_API vf128 wbtm_sse2_ldexp_ps(vf128 x, vi128 e);
WBTM_API vf128 wbtm_sse2_pow_ps(vf128 x, vf128 e);
WBTM_API vf128 wbtm_sse2_min_ps(vf128 x, vf128 y);
WBTM_API vf128 wbtm_sse2_max_ps(vf128 x, vf128 y);
WBTM_API vf128 wbtm_sse2_clamp_ps(vf128 minx, vf128 maxx, vf128 value);
WBTM_API vf128 wbtm_sse2_abs_ps(vf128 x);

WBTM_API f32 wbtm_logf(f32 x);
WBTM_API f32 wbtm_expf(f32 x);
WBTM_API f32 wbtm_sinf(f32 x);
WBTM_API f32 wbtm_cosf(f32 x);
WBTM_API void wbtm_sincosf(f32 x, f32* s, f32* c);
WBTM_API f32 wbtm_tanf(f32 x);
WBTM_API f32 wbtm_atan2f(f32 y, f32 x);
WBTM_API f32 wbtm_rsqrtf(f32 x);
WBTM_API f32 wbtm_sqrtf(f32 x);
WBTM_API f32 wbtm_ldexpf(f32 x, int e);
WBTM_API f32 wbtm_ipowf(f32 x, int e);
WBTM_API f32 wbtm_powf(f32 x, f32 y);
WBTM_API i32 wbtm_floorf(f32 x);
WBTM_API i32 wbtm_ceilf(f32 x);
WBTM_API f32 wbtm_minf(f32 x, f32 y);
WBTM_API f32 wbtm_maxf(f32 x, f32 y);
WBTM_API f32 wbtm_clampf(f32 low, f32 high, f32 value);
WBTM_API f32 wbtm_absf(f32 x);
WBTM_API i32 wbtm_abs(i32 x);
#endif

#ifndef WBTM_CRT_NO_REPLACE
#define cos wbtm_cosf
#define sin wbtm_sinf
#define atan2 wbtm_atan2
#define exp wbtm_expf
#define log wbtm_logf
#define ldexp wbtm_ldexpf
#define pow wbtm_powf
#define floor wbtm_floorf
#define ceil wbtm_ceilf
#define fabs wbtm_absf
#define clamp wbtm_clampf
#define sqrt wbtm_sqrtf
#define rsqrt wbtm_rsqrtf

#define cosf wbtm_cosf
#define sinf wbtm_sinf
#define expf wbtm_expf
#define atan2f wbtm_atan2f
#define logf wbtm_logf
#define ldexpf wbtm_ldexpf
#define powf wbtm_powf
#define floorf wbtm_floorf
#define ceilf wbtm_ceilf
#define fabsf wbtm_absf
#define clampf wbtm_clampf
#define minf wbtm_minf
#define maxf wbtm_maxf
#define sqrtf wbtm_sqrtf
#define rsqrtf wbtm_rsqrtf

#define abs wbtm_abs
#endif

#ifdef WBTM_IMPLEMENTATION

#ifdef _MSC_VER 
#define ALIGN16_BEG __declspec(align(16))
#define ALIGN16_END 
#else 
#define ALIGN16_BEG
#define ALIGN16_END __attribute__((aligned(16)))
#endif


#define _PS_CONST(Name, Val) static const ALIGN16_BEG f32 _ps_##Name[4] ALIGN16_END = { Val, Val, Val, Val }
#define _PI32_CONST(Name, Val) static const ALIGN16_BEG i32 _pi32_##Name[4] ALIGN16_END = { Val, Val, Val, Val }
#define _PS_CONST_TYPE(Name, Type, Val) static const ALIGN16_BEG Type _ps_##Name[4] ALIGN16_END = { Val, Val, Val, Val }

_PS_CONST(0, 0.0f);
_PS_CONST(1, 1.0f);
_PS_CONST(neg1, -1.0f);
_PS_CONST(2, 2.0f);
_PS_CONST(0p5, 0.5f);
_PS_CONST(3, 3);
_PS_CONST(full_mask, 0xFFFFFFF);
/* the smallest non denormalized f32 number */
_PS_CONST_TYPE(min_norm_pos, i32, 0x00800000);
_PS_CONST_TYPE(mant_mask, i32, 0x7f800000);
_PS_CONST_TYPE(inv_mant_mask, i32, ~0x7f800000);

_PS_CONST_TYPE(sign_mask, i32, (i32)0x80000000);
_PS_CONST_TYPE(inv_sign_mask, i32, ~0x80000000);
_PS_CONST_TYPE(1_mask, i32, 1);

_PI32_CONST(1, 1);
_PI32_CONST(inv1, ~1);
_PI32_CONST(2, 2);
_PI32_CONST(4, 4);
_PI32_CONST(0x7f, 0x7f);

_PS_CONST(cephes_SQRTHF, 0.707106781186547524);
_PS_CONST(cephes_log_p0, 7.0376836292E-2);
_PS_CONST(cephes_log_p1, - 1.1514610310E-1);
_PS_CONST(cephes_log_p2, 1.1676998740E-1);
_PS_CONST(cephes_log_p3, - 1.2420140846E-1);
_PS_CONST(cephes_log_p4, + 1.4249322787E-1);
_PS_CONST(cephes_log_p5, - 1.6668057665E-1);
_PS_CONST(cephes_log_p6, + 2.0000714765E-1);
_PS_CONST(cephes_log_p7, - 2.4999993993E-1);
_PS_CONST(cephes_log_p8, + 3.3333331174E-1);
_PS_CONST(cephes_log_q1, -2.12194440e-4);
_PS_CONST(cephes_log_q2, 0.693359375);

_PS_CONST(exp_hi,	88.3762626647949f);
_PS_CONST(exp_lo,	-88.3762626647949f);

_PS_CONST(cephes_LOG2EF, 1.44269504088896341);
_PS_CONST(cephes_exp_C1, 0.693359375);
_PS_CONST(cephes_exp_C2, -2.12194440e-4);

_PS_CONST(cephes_exp_p0, 1.9875691500E-4);
_PS_CONST(cephes_exp_p1, 1.3981999507E-3);
_PS_CONST(cephes_exp_p2, 8.3334519073E-3);
_PS_CONST(cephes_exp_p3, 4.1665795894E-2);
_PS_CONST(cephes_exp_p4, 1.6666665459E-1);
_PS_CONST(cephes_exp_p5, 5.0000001201E-1);

_PS_CONST(minus_cephes_DP1, -0.78515625);
_PS_CONST(minus_cephes_DP2, -2.4187564849853515625e-4);
_PS_CONST(minus_cephes_DP3, -3.77489497744594108e-8);
_PS_CONST(sincof_p0, -1.9515295891E-4);
_PS_CONST(sincof_p1,  8.3321608736E-3);
_PS_CONST(sincof_p2, -1.6666654611E-1);
_PS_CONST(coscof_p0,  2.443315711809948E-005);
_PS_CONST(coscof_p1, -1.388731625493765E-003);
_PS_CONST(coscof_p2,  4.166664568298827E-002);
_PS_CONST(cephes_FOPI, 1.27323954473516); // 4 / WB_PI

_PS_CONST(wb_atan2_1, -0.0464964749);
_PS_CONST(wb_atan2_2, 0.15931422);
_PS_CONST(wb_atan2_3, 0.327622764);
_PS_CONST(wb_atan2_4, 1.57079637);
_PS_CONST(pi, 3.141592653589793f);
#define WB_PI 3.141592653589793f
#define sse_select(a, b, cond) _mm_or_ps(_mm_and_ps(a, cond), _mm_andnot_ps(cond, b))
WBTM_API vf128 wbtm_sse2_abs_ps(vf128 x)
{
	return _mm_and_ps(x, *(vf128*)_ps_inv_sign_mask);
}

WBTM_API vf128 wbtm_sse2_min_ps(vf128 x, vf128 y)
{
	return _mm_min_ps(x, y);
}

WBTM_API vf128 wbtm_sse2_max_ps(vf128 x, vf128 y)
{
	return _mm_max_ps(x, y);
}

WBTM_API vf128 wbtm_sse2_clamp_ps(vf128 minx, vf128 maxx, vf128 value)
{
	return _mm_min_ps(_mm_max_ps(value, minx), maxx);
}

WBTM_API vf128 wbtm_sse2_rsqrt_ps(vf128 x)
{
	vf128 nr = _mm_rsqrt_ps(x);
	vf128 muls = _mm_mul_ps(_mm_mul_ps(x, nr), nr);
	return _mm_mul_ps(_mm_mul_ps(*(vf128*)_ps_0p5, nr), _mm_sub_ps(*(vf128*)_ps_3, muls));
}

WBTM_API vf128 wbtm_sse2_sqrt_ps(vf128 x)
{
	vf128 inv = wbtm_sse2_rsqrt_ps(x);
	return _mm_mul_ps(x, inv);
}

WBTM_API vf128 wbtm_sse2_atan_ps(vf128 xx)
{
	vf128 mask, mask2, y = _mm_set1_ps(0);
	vf128 one = _mm_set1_ps(1.0f);
	vf128 signs = _mm_and_ps(xx, _mm_castsi128_ps(_mm_set1_epi32((unsigned int)0x80000000)));
	vf128 x = _mm_and_ps(xx, _mm_castsi128_ps(_mm_set1_epi32(~0x80000000)));

	{
		vf128 tx1, tx2;
		mask = _mm_cmpgt_ps(x, _mm_set1_ps(2.414213562373095f));
		tx1 = _mm_div_ps(_mm_set1_ps(-1.0f), x);

		mask2 = _mm_cmpgt_ps(x, _mm_set1_ps(0.4142135623730950f));
		tx2 = _mm_div_ps(
				_mm_sub_ps(x, one), 
				_mm_add_ps(x, one));

		x = _mm_or_ps(_mm_and_ps(tx1, mask), _mm_andnot_ps(mask,
					_mm_or_ps(
						_mm_and_ps(tx2, mask2), 
						_mm_andnot_ps(mask2, x))));
		y = _mm_or_ps(_mm_and_ps(_mm_set1_ps(WB_PI/2), mask), _mm_andnot_ps(mask,
					_mm_or_ps(
						_mm_and_ps(_mm_set1_ps(WB_PI/4), mask2),
						_mm_andnot_ps(mask2, y))));
	}

	vf128 z = _mm_mul_ps(x, x);
	vf128 u = _mm_sub_ps(_mm_mul_ps(_mm_set1_ps(8.05374449538e-2), z), 
			_mm_set1_ps(1.38776856032E-1));
	u = _mm_add_ps(_mm_mul_ps(u, z), _mm_set1_ps(1.99777106478E-1));
	u = _mm_sub_ps(_mm_mul_ps(u, z), _mm_set1_ps(3.33329491539E-1));
	u = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(u, z), x), x);
	y = _mm_add_ps(y, u);
	return _mm_mul_ps(y, _mm_or_ps(signs, one));
}

WBTM_API vf128 wbtm_sse2_atan2_ps(vf128 y, vf128 x)
{
	vf128 zero = _mm_setzero_ps(), xlt0, ylt0, xeq0, yeq0;
	vf128 z = wbtm_sse2_atan_ps(_mm_div_ps(y, x));

	xlt0 = _mm_cmplt_ps(x, zero);
	xeq0 = _mm_cmpeq_ps(x, zero);
	ylt0 = _mm_cmplt_ps(y, zero);
	yeq0 = _mm_cmpeq_ps(y, zero);

	z = sse_select(
			sse_select(
				sse_select(
					zero,
					_mm_set1_ps(WB_PI/2),
					yeq0),
				_mm_set1_ps(-WB_PI/2),
				ylt0),
			z,
			xeq0);

	z = sse_select(
			sse_select(
				_mm_set1_ps(WB_PI/2),
				zero,
				xlt0),
			z, 
			yeq0);

	vf128 w = sse_select(
			sse_select(
				_mm_set1_ps(-WB_PI),
				_mm_set1_ps(WB_PI),
				ylt0
				),
			zero,
			xlt0);
	return _mm_add_ps(z, w);

}

/*
WBTM_API vf128 wbtm_sse2_atan2_ps(vf128 y, vf128 x)
{
	vf128 ya = _mm_and_ps(y, *(vf128*)_ps_inv_sign_mask);
	vf128 xa = _mm_and_ps(x, *(vf128*)_ps_inv_sign_mask);
	vf128 mmin = _mm_min_ps(ya, xa);
	vf128 mmax = _mm_max_ps(ya, xa);
	vf128 a = _mm_div_ps(mmin, mmax);
	vf128 s = _mm_mul_ps(a, a);
	vf128 r = _mm_add_ps(_mm_mul_ps(s, *(vf128*)_ps_wb_atan2_1), *(vf128*)_ps_wb_atan2_2);
	r = _mm_sub_ps(_mm_mul_ps(r, s), *(vf128*)_ps_wb_atan2_3);
	r = _mm_add_ps(a, _mm_mul_ps(a, _mm_mul_ps(s, r)));

	vf128 zero = *(vf128*)_ps_0;
	vf128 one = *(vf128*)_ps_1;
	vf128 two = *(vf128*)_ps_2;
	vf128 mask = _mm_and_ps(_mm_cmpgt_ps(ya, ya), one);
	vf128 vals = _mm_mul_ps(_mm_sub_ps(*(vf128*)_ps_wb_atan2_4, _mm_mul_ps(r, two)), mask);
	r = _mm_add_ps(vals, r);

	mask = _mm_and_ps(_mm_cmplt_ps(x, zero), one);
	vals = _mm_mul_ps(_mm_sub_ps(*(vf128*)_ps_pi, _mm_mul_ps(r, two)), mask);

	r = _mm_add_ps(vals, r);
		
	mask = _mm_cmplt_ps(y, zero);
	vf128 neg_mask = _mm_andnot_ps(mask, *(vf128*)_ps_full_mask);
	mask = _mm_and_ps(mask, one);
	neg_mask = _mm_and_ps(neg_mask, one);
	mask = _mm_add_ps(neg_mask, _mm_mul_ps(mask, *(vf128*)_ps_neg1));

	r = _mm_mul_ps(r, mask);
	return r;
}
*/

/* natural logarithm computed for 4 simultaneous f32 
   return NaN for x <= 0
   */

WBTM_API vf128 wbtm_sse2_log_ps(vf128 x) 
{
	vi128 emm0;
	vf128 one = *(vf128*)_ps_1;

	vf128 invalid_mask = _mm_cmple_ps(x, _mm_setzero_ps());

	x = _mm_max_ps(x, *(vf128*)_ps_min_norm_pos);  /* cut off denormalized stuff */

	emm0 = _mm_srli_epi32(_mm_castps_si128(x), 23);
	/* keep only the fractional part */
	x = _mm_and_ps(x, *(vf128*)_ps_inv_mant_mask);
	x = _mm_or_ps(x, *(vf128*)_ps_0p5);

	emm0 = _mm_sub_epi32(emm0, *(vi128*)_pi32_0x7f);
	vf128 e = _mm_cvtepi32_ps(emm0);

	e = _mm_add_ps(e, one);

	vf128 mask = _mm_cmplt_ps(x, *(vf128*)_ps_cephes_SQRTHF);
	vf128 tmp = _mm_and_ps(x, mask);
	x = _mm_sub_ps(x, one);
	e = _mm_sub_ps(e, _mm_and_ps(one, mask));
	x = _mm_add_ps(x, tmp);


	vf128 z = _mm_mul_ps(x,x);

	vf128 y = *(vf128*)_ps_cephes_log_p0;
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(vf128*)_ps_cephes_log_p1);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(vf128*)_ps_cephes_log_p2);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(vf128*)_ps_cephes_log_p3);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(vf128*)_ps_cephes_log_p4);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(vf128*)_ps_cephes_log_p5);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(vf128*)_ps_cephes_log_p6);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(vf128*)_ps_cephes_log_p7);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(vf128*)_ps_cephes_log_p8);
	y = _mm_mul_ps(y, x);

	y = _mm_mul_ps(y, z);

	tmp = _mm_mul_ps(e, *(vf128*)_ps_cephes_log_q1);
	y = _mm_add_ps(y, tmp);

	tmp = _mm_mul_ps(z, *(vf128*)_ps_0p5);
	y = _mm_sub_ps(y, tmp);

	tmp = _mm_mul_ps(e, *(vf128*)_ps_cephes_log_q2);
	x = _mm_add_ps(x, y);
	x = _mm_add_ps(x, tmp);
	x = _mm_or_ps(x, invalid_mask); // negative arg will be NAN
	return x;
}

WBTM_API vf128 wbtm_sse2_exp_ps(vf128 x) 
{
	vf128 tmp = _mm_setzero_ps(), fx;
	vi128 emm0;
	vf128 one = *(vf128*)_ps_1;

	x = _mm_min_ps(x, *(vf128*)_ps_exp_hi);
	x = _mm_max_ps(x, *(vf128*)_ps_exp_lo);

	/* express exp(x) as exp(g + n*log(2)) */
	fx = _mm_mul_ps(x, *(vf128*)_ps_cephes_LOG2EF);
	fx = _mm_add_ps(fx, *(vf128*)_ps_0p5);

	/* how to perform a floorf with SSE: just below */
	emm0 = _mm_cvttps_epi32(fx);
	tmp  = _mm_cvtepi32_ps(emm0);

	/* if greater, substract 1 */
	vf128 mask = _mm_cmpgt_ps(tmp, fx);    
	mask = _mm_and_ps(mask, one);
	fx = _mm_sub_ps(tmp, mask);

	tmp = _mm_mul_ps(fx, *(vf128*)_ps_cephes_exp_C1);
	vf128 z = _mm_mul_ps(fx, *(vf128*)_ps_cephes_exp_C2);
	x = _mm_sub_ps(x, tmp);
	x = _mm_sub_ps(x, z);

	z = _mm_mul_ps(x,x);

	vf128 y = *(vf128*)_ps_cephes_exp_p0;
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(vf128*)_ps_cephes_exp_p1);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(vf128*)_ps_cephes_exp_p2);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(vf128*)_ps_cephes_exp_p3);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(vf128*)_ps_cephes_exp_p4);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(vf128*)_ps_cephes_exp_p5);
	y = _mm_mul_ps(y, z);
	y = _mm_add_ps(y, x);
	y = _mm_add_ps(y, one);

	/* build 2^n */
	emm0 = _mm_cvttps_epi32(fx);
	emm0 = _mm_add_epi32(emm0, *(vi128*)_pi32_0x7f);
	emm0 = _mm_slli_epi32(emm0, 23);
	vf128 pow2n = _mm_castsi128_ps(emm0);
	y = _mm_mul_ps(y, pow2n);
	return y;
}


/* evaluation of 4 sines at onces, using only SSE1+MMX intrinsics so
   it runs also on old athlons XPs and the pentium III of your grand
   mother.

   The code is the exact rewriting of the cephes sinf function.
   Precision is excellent as long as x < 8192 (I did not bother to
   take into account the special handling they have for greater values
   -- it does not return garbage for arguments over 8192, though, but
   the extra precision is missing).

   Note that it is such that sinf((f32)WB_PI) = 8.74e-8, which is the
   surprising but correct result.

   Performance is also surprisingly good, 1.33 times faster than the
   macos vsinf SSE2 function, and 1.5 times faster than the
   __vrs4_sinf of amd's ACML (which is only available in 64 bits). Not
   too bad for an SSE1 function (with no special tuning) !
   However the latter libraries probably have a much better handling of NaN,
   Inf, denormalized and other special arguments..

   On my core 1 duo, the execution of this function takes approximately 95 cycles.

   From what I have observed on the experiments with intel AMath lib, switching to an
   SSE2 version would improve the perf by only 10%.

   Since it is based on SSE intrinsics, it has to be compiled at -O2 to
   deliver full speed.
   */
WBTM_API vf128 wbtm_sse2_sin_ps(vf128 x) 
{ 
	vf128 xmm1, xmm2 = _mm_setzero_ps(), xmm3, sign_bit, y;

	vi128 emm0, emm2;

	sign_bit = x;
	/* take the absolute value */
	x = _mm_and_ps(x, *(vf128*)_ps_inv_sign_mask);
	/* extract the sign bit (upper one) */
	sign_bit = _mm_and_ps(sign_bit, *(vf128*)_ps_sign_mask);

	/* scale by 4/Pi */
	y = _mm_mul_ps(x, *(vf128*)_ps_cephes_FOPI);

	/* store the i32eger part of y in mm0 */
	emm2 = _mm_cvttps_epi32(y);
	/* j=(j+1) & (~1) (see the cephes sources) */
	emm2 = _mm_add_epi32(emm2, *(vi128*)_pi32_1);
	emm2 = _mm_and_si128(emm2, *(vi128*)_pi32_inv1);
	y = _mm_cvtepi32_ps(emm2);

	/* get the swap sign flag */
	emm0 = _mm_and_si128(emm2, *(vi128*)_pi32_4);
	emm0 = _mm_slli_epi32(emm0, 29);
	/* get the polynom selection mask 
	   there is one polynom for 0 <= x <= Pi/4
	   and another one for Pi/4<x<=Pi/2

	   Both branches will be computed.
	   */
	emm2 = _mm_and_si128(emm2, *(vi128*)_pi32_2);
	emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());

	vf128 swap_sign_bit = _mm_castsi128_ps(emm0);
	vf128 poly_mask = _mm_castsi128_ps(emm2);
	sign_bit = _mm_xor_ps(sign_bit, swap_sign_bit);

	/* The magic pass: "Extended precision modular arithmetic" 
	   x = ((x - y * DP1) - y * DP2) - y * DP3; */
	xmm1 = *(vf128*)_ps_minus_cephes_DP1;
	xmm2 = *(vf128*)_ps_minus_cephes_DP2;
	xmm3 = *(vf128*)_ps_minus_cephes_DP3;
	xmm1 = _mm_mul_ps(y, xmm1);
	xmm2 = _mm_mul_ps(y, xmm2);
	xmm3 = _mm_mul_ps(y, xmm3);
	x = _mm_add_ps(x, xmm1);
	x = _mm_add_ps(x, xmm2);
	x = _mm_add_ps(x, xmm3);

	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
	y = *(vf128*)_ps_coscof_p0;
	vf128 z = _mm_mul_ps(x,x);

	y = _mm_mul_ps(y, z);
	y = _mm_add_ps(y, *(vf128*)_ps_coscof_p1);
	y = _mm_mul_ps(y, z);
	y = _mm_add_ps(y, *(vf128*)_ps_coscof_p2);
	y = _mm_mul_ps(y, z);
	y = _mm_mul_ps(y, z);
	vf128 tmp = _mm_mul_ps(z, *(vf128*)_ps_0p5);
	y = _mm_sub_ps(y, tmp);
	y = _mm_add_ps(y, *(vf128*)_ps_1);

	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

	vf128 y2 = *(vf128*)_ps_sincof_p0;
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_add_ps(y2, *(vf128*)_ps_sincof_p1);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_add_ps(y2, *(vf128*)_ps_sincof_p2);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_mul_ps(y2, x);
	y2 = _mm_add_ps(y2, x);

	/* select the correct result from the two polynoms */  
	xmm3 = poly_mask;
	y2 = _mm_and_ps(xmm3, y2); //, xmm3);
	y = _mm_andnot_ps(xmm3, y);
	y = _mm_add_ps(y,y2);
	/* update the sign */
	y = _mm_xor_ps(y, sign_bit);
	return y;
}

/* almost the same as sin_ps */
WBTM_API vf128 wbtm_sse2_cos_ps(vf128 x) 
{
	vf128 xmm1, xmm2 = _mm_setzero_ps(), xmm3, y;
	vi128 emm0, emm2;
	/* take the absolute value */
	x = _mm_and_ps(x, *(vf128*)_ps_inv_sign_mask);

	/* scale by 4/Pi */
	y = _mm_mul_ps(x, *(vf128*)_ps_cephes_FOPI);

	/* store the i32eger part of y in mm0 */
	emm2 = _mm_cvttps_epi32(y);
	/* j=(j+1) & (~1) (see the cephes sources) */
	emm2 = _mm_add_epi32(emm2, *(vi128*)_pi32_1);
	emm2 = _mm_and_si128(emm2, *(vi128*)_pi32_inv1);
	y = _mm_cvtepi32_ps(emm2);

	emm2 = _mm_sub_epi32(emm2, *(vi128*)_pi32_2);

	/* get the swap sign flag */
	emm0 = _mm_andnot_si128(emm2, *(vi128*)_pi32_4);
	emm0 = _mm_slli_epi32(emm0, 29);
	/* get the polynom selection mask */
	emm2 = _mm_and_si128(emm2, *(vi128*)_pi32_2);
	emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());

	vf128 sign_bit = _mm_castsi128_ps(emm0);
	vf128 poly_mask = _mm_castsi128_ps(emm2);

	/* The magic pass: "Extended precision modular arithmetic" 
	   x = ((x - y * DP1) - y * DP2) - y * DP3; */
	xmm1 = *(vf128*)_ps_minus_cephes_DP1;
	xmm2 = *(vf128*)_ps_minus_cephes_DP2;
	xmm3 = *(vf128*)_ps_minus_cephes_DP3;
	xmm1 = _mm_mul_ps(y, xmm1);
	xmm2 = _mm_mul_ps(y, xmm2);
	xmm3 = _mm_mul_ps(y, xmm3);
	x = _mm_add_ps(x, xmm1);
	x = _mm_add_ps(x, xmm2);
	x = _mm_add_ps(x, xmm3);

	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
	y = *(vf128*)_ps_coscof_p0;
	vf128 z = _mm_mul_ps(x,x);

	y = _mm_mul_ps(y, z);
	y = _mm_add_ps(y, *(vf128*)_ps_coscof_p1);
	y = _mm_mul_ps(y, z);
	y = _mm_add_ps(y, *(vf128*)_ps_coscof_p2);
	y = _mm_mul_ps(y, z);
	y = _mm_mul_ps(y, z);
	vf128 tmp = _mm_mul_ps(z, *(vf128*)_ps_0p5);
	y = _mm_sub_ps(y, tmp);
	y = _mm_add_ps(y, *(vf128*)_ps_1);

	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

	vf128 y2 = *(vf128*)_ps_sincof_p0;
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_add_ps(y2, *(vf128*)_ps_sincof_p1);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_add_ps(y2, *(vf128*)_ps_sincof_p2);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_mul_ps(y2, x);
	y2 = _mm_add_ps(y2, x);

	/* select the correct result from the two polynoms */  
	xmm3 = poly_mask;
	y2 = _mm_and_ps(xmm3, y2); //, xmm3);
	y = _mm_andnot_ps(xmm3, y);
	y = _mm_add_ps(y,y2);
	/* update the sign */
	y = _mm_xor_ps(y, sign_bit);

	return y;
}

/* since sin_ps and cos_ps are almost identical, sincos_ps could replace both of them..
   it is almost as fast, and gives you a free cosine with your sine */
WBTM_API void wbtm_sse2_sincos_ps(vf128 x, vf128 *s, vf128 *c) 
{
	vf128 xmm1, xmm2, xmm3 = _mm_setzero_ps(), sign_bit_sin, y;
	vi128 emm0, emm2, emm4;
	sign_bit_sin = x;
	/* take the absolute value */
	x = _mm_and_ps(x, *(vf128*)_ps_inv_sign_mask);
	/* extract the sign bit (upper one) */
	sign_bit_sin = _mm_and_ps(sign_bit_sin, *(vf128*)_ps_sign_mask);

	/* scale by 4/Pi */
	y = _mm_mul_ps(x, *(vf128*)_ps_cephes_FOPI);

	/* store the i32eger part of y in emm2 */
	emm2 = _mm_cvttps_epi32(y);

	/* j=(j+1) & (~1) (see the cephes sources) */
	emm2 = _mm_add_epi32(emm2, *(vi128*)_pi32_1);
	emm2 = _mm_and_si128(emm2, *(vi128*)_pi32_inv1);
	y = _mm_cvtepi32_ps(emm2);

	emm4 = emm2;

	/* get the swap sign flag for the sine */
	emm0 = _mm_and_si128(emm2, *(vi128*)_pi32_4);
	emm0 = _mm_slli_epi32(emm0, 29);
	vf128 swap_sign_bit_sin = _mm_castsi128_ps(emm0);

	/* get the polynom selection mask for the sine*/
	emm2 = _mm_and_si128(emm2, *(vi128*)_pi32_2);
	emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());
	vf128 poly_mask = _mm_castsi128_ps(emm2);

	/* The magic pass: "Extended precision modular arithmetic" 
	   x = ((x - y * DP1) - y * DP2) - y * DP3; */
	xmm1 = *(vf128*)_ps_minus_cephes_DP1;
	xmm2 = *(vf128*)_ps_minus_cephes_DP2;
	xmm3 = *(vf128*)_ps_minus_cephes_DP3;
	xmm1 = _mm_mul_ps(y, xmm1);
	xmm2 = _mm_mul_ps(y, xmm2);
	xmm3 = _mm_mul_ps(y, xmm3);
	x = _mm_add_ps(x, xmm1);
	x = _mm_add_ps(x, xmm2);
	x = _mm_add_ps(x, xmm3);

	emm4 = _mm_sub_epi32(emm4, *(vi128*)_pi32_2);
	emm4 = _mm_andnot_si128(emm4, *(vi128*)_pi32_4);
	emm4 = _mm_slli_epi32(emm4, 29);
	vf128 sign_bit_cos = _mm_castsi128_ps(emm4);

	sign_bit_sin = _mm_xor_ps(sign_bit_sin, swap_sign_bit_sin);


	/* Evaluate the first polynom  (0 <= x <= Pi/4) */
	vf128 z = _mm_mul_ps(x,x);
	y = *(vf128*)_ps_coscof_p0;

	y = _mm_mul_ps(y, z);
	y = _mm_add_ps(y, *(vf128*)_ps_coscof_p1);
	y = _mm_mul_ps(y, z);
	y = _mm_add_ps(y, *(vf128*)_ps_coscof_p2);
	y = _mm_mul_ps(y, z);
	y = _mm_mul_ps(y, z);
	vf128 tmp = _mm_mul_ps(z, *(vf128*)_ps_0p5);
	y = _mm_sub_ps(y, tmp);
	y = _mm_add_ps(y, *(vf128*)_ps_1);

	/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

	vf128 y2 = *(vf128*)_ps_sincof_p0;
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_add_ps(y2, *(vf128*)_ps_sincof_p1);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_add_ps(y2, *(vf128*)_ps_sincof_p2);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_mul_ps(y2, x);
	y2 = _mm_add_ps(y2, x);

	/* select the correct result from the two polynoms */  
	xmm3 = poly_mask;
	vf128 ysin2 = _mm_and_ps(xmm3, y2);
	vf128 ysin1 = _mm_andnot_ps(xmm3, y);
	y2 = _mm_sub_ps(y2,ysin2);
	y = _mm_sub_ps(y, ysin1);

	xmm1 = _mm_add_ps(ysin1,ysin2);
	xmm2 = _mm_add_ps(y,y2);

	/* update the sign */
	*s = _mm_xor_ps(xmm1, sign_bit_sin);
	*c = _mm_xor_ps(xmm2, sign_bit_cos);
}

WBTM_API vf128 wbtm_sse2_pow_ps(vf128 x, vf128 e)
{
	x = wbtm_sse2_log_ps(x);
	x = _mm_mul_ps(x, e);
	return wbtm_sse2_exp_ps(x);
}

WBTM_API vf128 wbtm_sse2_ldexp_ps(vf128 x, vi128 e)
{
	e = _mm_sll_epi32(_mm_set1_epi32(2), e);
	return _mm_mul_ps(x, _mm_cvtepi32_ps(e));
}

WBTM_API f32 wbtm_logf(f32 x)
{
	vf128 v = _mm_set_ss(x);
	return _mm_cvtss_f32(wbtm_sse2_log_ps(v));
}

WBTM_API f32 wbtm_expf(f32 x)
{
	vf128 v = _mm_set_ss(x);
	return _mm_cvtss_f32(wbtm_sse2_exp_ps(v));
}

WBTM_API f32 wbtm_sinf(f32 x)
{
	vf128 v = _mm_set_ss(x);
	return _mm_cvtss_f32(wbtm_sse2_sin_ps(v));
}

WBTM_API f32 wbtm_cosf(f32 x)
{
	vf128 v = _mm_set_ss(x);
	return _mm_cvtss_f32(wbtm_sse2_cos_ps(v));
}

WBTM_API void wbtm_sincosf(f32 x, f32* s, f32* c)
{
	vf128 v = _mm_set_ss(x);
	vf128 sin, cos;
	wbtm_sse2_sincos_ps(v, &sin, &cos);
	*s = _mm_cvtss_f32(sin);
	*c = _mm_cvtss_f32(cos);
}

WBTM_API f32 wbtm_tanf(f32 x)
{
	vf128 v = _mm_set_ss(x);
	vf128 sin, cos;
	wbtm_sse2_sincos_ps(v, &sin, &cos);
	return _mm_cvtss_f32(_mm_div_ps(sin, cos));
}

WBTM_API f32 wbtm_ldexpf(f32 x, i32 e)
{
	vf128 v = _mm_set_ss(x);
	vi128 s = _mm_set_epi32(e, 0, 0, 0);
	return _mm_cvtss_f32(wbtm_sse2_ldexp_ps(v, s));
}

WBTM_API f32 wbtm_atan2f(f32 y, f32 x)
{
	vf128 vy, vx;
	vy = _mm_set_ss(y);
	vx = _mm_set_ss(x);
	return _mm_cvtss_f32(wbtm_sse2_atan2_ps(vy, vx));
}

WBTM_API f32 wbtm_rsqrtf(f32 x)
{
	vf128 v = _mm_set_ss(x);
	v = wbtm_sse2_rsqrt_ps(v);
	return _mm_cvtss_f32(v);
}

WBTM_API f32 wbtm_sqrtf(f32 x)
{
	vf128 v = _mm_set_ss(x);
	vf128 s = wbtm_sse2_sqrt_ps(v);
	return _mm_cvtss_f32(s);
}

WBTM_API f32 wbtm_ipowf(f32 x, i32 e)
{
	f32 r = 1;
	while(e) {
		if(e & 1) {
			r *= x;
			e--;
		}
		x *= x;
		e /= 2;
	}
	return r;
}

WBTM_API f32 wbtm_powf(f32 x, f32 y)
{
	vf128 a = _mm_set_ss(x);
	vf128 b = _mm_set_ss(y);
	return _mm_cvtss_f32(wbtm_sse2_pow_ps(a, b));
}

WBTM_API i32 wbtm_floorf(f32 x)
{
	return (int)x + (x < 0 ? -1 : 0);
}

WBTM_API i32 wbtm_ceilf(f32 x)
{
	return (int)x + (x < 0 ? 0 : 1);
}

WBTM_API f32 wbtm_minf(f32 x, f32 y)
{
	vf128 a = _mm_set_ss(x);
	vf128 b = _mm_set_ss(y);
	return _mm_cvtss_f32(wbtm_sse2_min_ps(a, b));
}

WBTM_API f32 wbtm_maxf(f32 x, f32 y)
{
	vf128 a = _mm_set_ss(x);
	vf128 b = _mm_set_ss(y);
	return _mm_cvtss_f32(wbtm_sse2_max_ps(a, b));
}

WBTM_API f32 wbtm_clampf(f32 low, f32 high, f32 value)
{
	vf128 minx = _mm_set_ss(low);
	vf128 maxx = _mm_set_ss(high);
	vf128 val = _mm_set_ss(value);
	return _mm_cvtss_f32(wbtm_sse2_clamp_ps(minx, maxx, val));
}

WBTM_API int wbtm_abs(int v)
{
	int mask = v >> (sizeof(int) * 8 - 1);
	return (v + mask) - mask;
}

WBTM_API f32 wbtm_absf(f32 x)
{
	vf128 v = _mm_set_ss(x);
	return _mm_cvtss_f32(wbtm_sse2_abs_ps(v));
}

WBTM_API f32 wb_atanf(f32 x)
{
	vf128 v = _mm_set_ss(x);
	vf128 w = wbtm_sse2_atan_ps(v);
	f32 q = _mm_cvtss_f32(w);
	return q;
}

WBTM_API f32 wb_atan2f(f32 y, f32 x)
{
	int code = 0;
	if(x < 0) {
		code = 2;
	}
	if(y < 0) {
		code |= 1;
	}

	if(x == 0) {
		if(code & 1) {
			return -WB_PI / 2;
		}
		if(y == 0) {
			return 0;
		}
		return WB_PI / 2;
	}

	if(y == 0) {
		if(code & 2) {
			return WB_PI;
		}
		return 0;
	}

	f32 w = 0;
	if(x < 0) {
		if(y < 0) {
			w = -WB_PI;
		} else {
			w = WB_PI;
		}
	}

	return w + wb_atanf(y/x);
}


#endif
