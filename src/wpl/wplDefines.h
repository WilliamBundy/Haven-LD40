#pragma once
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef size_t usize;
typedef ptrdiff_t isize;

typedef __m128 vf128;
typedef __m128i vi128;
union vf32x4
{
	vf128 v;
	f32 f[4];
};
typedef union vf32x4 vf32x4;
#define ShuffleToByte(x, y, z, w) (((x)<<6) | ((y)<<4) | ((z)<<2) | w)
#define vfShuffle(a, x, y, z, w) _mm_shuffle_ps(a, a, ShuffleToByte(x, y, z, w))
#define vfShuffle2(a, b, ax, ay, bz, bw) _mm_shuffle_ps(a, b, ShuffleToByte(x, y, z, w))

#define Math_Tau (M_PI*2.0f)
#define Math_DegToRad (Math_Tau / 360.0f)
#define Math_RadToDeg (360.0f / Math_Tau)

