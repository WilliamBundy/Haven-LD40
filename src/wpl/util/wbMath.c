
static inline
f32 minf(f32 x, f32 y)
{
	vf128 a = _mm_set_ss(x);
	vf128 b = _mm_set_ss(y);
	return _mm_cvtss_f32(_mm_min_ps(a, b));
}

static inline
f32 maxf(f32 x, f32 y)
{
	vf128 a = _mm_set_ss(x);
	vf128 b = _mm_set_ss(y);
	return _mm_cvtss_f32(_mm_max_ps(a, b));
}

#ifndef WirmphtEnabled
struct Vec2
{
	f32 x, y;
};

struct Vec2i
{
	i16 x, y;
};

struct Rect2
{
	f32 x, y, w, h;
};

struct Rect2i
{
	i16 x, y, w, h;
};

union Color
{
	u32 rgba;
	struct {
		u8 a, b, g, r;
	};
};

struct Vec4
{
	f32 x, y, z, w;
};
#endif

#define v20 v2(0, 0)
static inline
Vec2 v2(f32 x, f32 y)
{
	Vec2 v;
	v.x = x;
	v.y = y;
	return v;
}

Vec2 v216(f32 x, f32 y)
{
	return v2(x * 16, y * 16);
}

static inline
Vec2 v2Add(Vec2 a, Vec2 b)
{
	Vec2 v;
	v.x = a.x + b.x;
	v.y = a.y + b.y;
	return v;
}

static inline
Vec2 v2AddScaled(Vec2 a, Vec2 b, f32 f)
{
	Vec2 v;
	v.x = a.x + b.x * f;
	v.y = a.y + b.y * f;
	return v;
}

static inline
Vec2 v2Negate(Vec2 a)
{
	Vec2 v;
	v.x = -a.x;
	v.y = -a.y;
	return v;
}

static inline
Vec2 v2Sub(Vec2 a, Vec2 b)
{
	Vec2 v;
	v.x = a.x - b.x;
	v.y = a.y - b.y;
	return v;
}

static inline
Vec2 v2Scale(Vec2 a, f32 f)
{
	Vec2 v;
	v.x = a.x * f;
	v.y = a.y * f;
	return v;
}

static inline
Vec2 v2Mul(Vec2 a, Vec2 b)
{
	Vec2 v;
	v.x = a.x * b.x;
	v.y = a.y * b.y;
	return v;
}

static inline
f32 v2Mag2(Vec2 a)
{
	return a.x * a.x + a.y * a.y;
}

static inline
f32 v2Mag(Vec2 a)
{
	return sqrtf(v2Mag2(a));
}

static inline
f32 v2Dot(Vec2 a, Vec2 b)
{
	return a.x * b.x + a.y * b.y;
}

static inline
f32 v2Cross(Vec2 a, Vec2 b)
{
	f64 p1 = (f64)a.x * (f64)b.y;
	f64 p2 = (f64)b.x * (f64)a.y;
	return (f32)(p1 - p2);
	//return (a.x * b.y) - (a.y * b.x);
}

static inline
f32 v2CrossOrigin(Vec2 a, Vec2 b, Vec2 o)
{
	a = v2Sub(a, o);
	b = v2Sub(b, o);
	return a.x * b.y - a.y * b.x;
}

static inline
Vec2 v2Normalize(Vec2 a)
{
	Vec2 v;
	f32 mag = v2Mag(a);
	v.x = a.x / mag;
	v.y = a.y / mag;
	return v;
}

static inline
Vec2 v2Orient(Vec2 where, Vec2 to)
{
	return v2Normalize(v2Sub(to, where));
}

static inline
Vec2 v2FromAngle(f32 angle, f32 mag)
{
	Vec2 v;
	v.x = cosf(angle) * mag;
	v.y = sinf(angle) * mag;
	return v;
}

static inline
f32 v2ToAngle(Vec2 a)
{
	return atan2f(a.y, a.x);
}

static inline
f32 v2DistToAngle(Vec2 a, Vec2 b)
{
	Vec2 v = v2Sub(b, a);
	return atan2f(v.y, v.x);
}

static inline
f32 v2Dist2(Vec2 a, Vec2 b)
{
	Vec2 v = v2Sub(b, a);
	return v2Dot(v, v);
}

f32 v2Dist(Vec2 a, Vec2 b)
{
	Vec2 v = v2Sub(b, a);
	return sqrtf(v2Dot(v, v));
}



static inline 
Vec2 v2Perpendicular(Vec2 a)
{
	Vec2 v;
	v.x = -a.y;
	v.y = a.x;
	return v;
}

static inline
Vec2 v2Abs(Vec2 a)
{
	Vec2 v;
	v.x = fabsf(a.x);
	v.y = fabsf(a.y);
	return v;
}

static inline
Vec2 v2Min(Vec2 a, Vec2 b)
{
	Vec2 v;
	v.x = minf(a.x, b.x);
	v.y = minf(a.y, b.y);
	return v;
}

static inline
Vec2 v2Max(Vec2 a, Vec2 b)
{
	Vec2 v;
	v.x = maxf(a.x, b.x);
	v.y = maxf(a.y, b.y);
	return v;
}



static inline
Vec2 v2SwapXY(Vec2 a)
{
	Vec2 v;
	v.x = a.y;
	v.y = a.x;
	return v;
}

static inline
Vec2 v2Rotate(Vec2 a, Vec2 rot)
{
	Vec2 v;
	v.x = rot.x * a.x + rot.y * a.y;
	v.y = -rot.y * a.x + rot.x * a.y;
	return v;
}

static inline
Vec2 v2RotateOrigin(Vec2 a, Vec2 rot, Vec2 orig)
{
	Vec2 v;
	a.x -= orig.x;
	a.y -= orig.y;
	v.x = rot.x * a.x + rot.y * a.y;
	v.y = -rot.y * a.x + rot.x * a.y;
	return v2Add(v, orig);
}


static inline
Vec2 v2Floor(Vec2 a)
{
	Vec2 v;
	v.x = floorf(a.x);
	v.y = floorf(a.y);
	return v;
}

//warning: uses float ==
//for assignment comparisons only
static inline
i32 v2Eq(Vec2 a, Vec2 b)
{
	return a.x == b.x && a.y == b.y;
}

static inline
i32 v2AlmostEq(Vec2 a, Vec2 b, f32 amt)
{
	f32 dx = b.x - a.x;
	if(dx * dx < amt) {
		f32 dy = b.y - a.y;
		if(dy * dy < amt) {
			return 1;
		}
	} 
	return 0;
}


/* "ip" or "In-Place" functions modify the first parameter */

static inline
void v2AddP(Vec2* a, Vec2 b)
{
	a->x += b.x;
	a->y += b.y;
}

static inline
void v2AddScaledP(Vec2* a, Vec2 b, f32 f)
{
	a->x += b.x * f;
	a->y += b.y * f;
}

static inline
void v2NegateP(Vec2* a)
{
	a->x *= -1;
	a->y *= -1;
}

static inline
void v2SubP(Vec2* a, Vec2 b)
{
	a->x -= b.x;
	a->y -= b.y;
}

static inline
void v2ScaleP(Vec2* a, f32 f)
{
	a->x *= f;
	a->y *= f;
}

static inline
void v2MulP(Vec2* a, Vec2 b)
{
	a->x *= b.x;
	a->y *= b.y;
}

static inline
void v2NormalizeP(Vec2* a)
{
	f32 mag = v2Mag(*a);
	a->x /= mag;
	a->y /= mag;
}

static inline
void v2PerpendicularP(Vec2* a)
{
	f32 x = a->x;
	a->x = -a->y;
	a->y = x;
}

static inline
void v2RotateP(Vec2* b, Vec2 rot)
{
	Vec2 a = *b;
	b->x = rot.x * a.x + rot.y * a.y;
	b->y = -rot.y * a.x + rot.x * a.y;
}

static inline
void v2FloorP(Vec2* a)
{
	a->x = floorf(a->x);
	a->y = floorf(a->y);
}

static inline
Vec2 v2WorldToLocal(Vec2 pt, Vec2 offset, f32 scale, Vec2 hWindow)
{
	v2SubP(&pt, offset);
	v2ScaleP(&pt, scale);
	v2SubP(&pt, hWindow);
	return pt;
}
static inline

Vec2 v2LocalToWorld(Vec2 pt, Vec2 offset, f32 scale, Vec2 hWindow)
{
	v2SubP(&offset, hWindow);
	v2AddP(&pt, offset);
	v2ScaleP(&pt, 1.0f / scale);
	return pt;
}

Vec2i v2i(i16 x, i16 y)
{
	Vec2i v;
	v.x = x;
	v.y = y;
	return v;
}

Vec4 v4(f32 x, f32 y, f32 z, f32 w)
{
	Vec4 v;
	v.x = x;
	v.y = y;
	v.z = z;
	v.w = w;
	return v;
}

Rect2 r2(f32 x, f32 y, f32 w, f32 h)
{
	Rect2 r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;
	return r;
}

Rect2i r2i(i16 x, i16 y, i16 w, i16 h)
{
	Rect2i r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;
	return r;
}

Rect2i r2i16(i16 x, i16 y, i16 w, i16 h)
{
	return r2i(x * 16, y * 16, w * 16, h * 16);
}

Vec2 v2f(Vec2i v)
{
	return v2(v.x, v.y);
}

Rect2 r2f(Rect2i r)
{
	return r2(r.x, r.y, r.w, r.h);
}

Rect2i r2fi(Rect2 r)
{
	return r2i((i16)r.x, (i16)r.y, (i16)r.w, (i16)r.h);
}

Rect2 r2v(Vec2 pos, Vec2 size)
{
	return r2(pos.x, pos.y, size.x, size.y);
}

i32 r2Contains(Rect2 r, Vec2 p)
{
	return p.x > r.x &&
		p.y > r.y &&
		p.y < (r.y + r.h) &&
		p.x < (r.x + r.w);
}


Color colorRgba(u32 rgba)
{
	Color color = {rgba};
	return color;
}

Color colorRgb(u32 rgb)
{
	return colorRgba(rgb << 8 | 0xFF);
}

Vec4 colorToV4(Color c)
{
	Vec4 v;
	v.x = (f32)c.r / 255.0;
	v.y = (f32)c.g / 255.0;
	v.z = (f32)c.b / 255.0;
	v.w = (f32)c.a / 255.0;
	return v;
}

void v4MulP(Vec4* v, Vec4 b)
{
	Vec4 a = {
		v->x * b.x,
		v->y * b.y,
		v->z * b.z,
		v->w * b.w,
	};
	*v = a;
}

void v4ScaleP(Vec4* v, f32 scale)
{
	Vec4 a = {
		v->x * scale,
		v->y * scale,
		v->z * scale,
		v->w * scale,
	};
	*v = a;
}

void v4ScaleClampP(Vec4* v, f32 scale, f32 minimum, f32 maximum)
{
	Vec4 a = {
		minf(maxf(v->x * scale, minimum), maximum),
		minf(maxf(v->y * scale, minimum), maximum),
		minf(maxf(v->z * scale, minimum), maximum),
		minf(maxf(v->w * scale, minimum), maximum),
	};
	*v = a;
}

void v4ScaleColorP(Vec4* v, f32 scale, f32 minimum, f32 maximum)
{
	Vec4 a = {
		v->x * scale,
		v->y * scale,
		v->z * scale,
		v->w * scale,
	};
	f32 biggest = maxf(a.x, maxf(a.y, a.z));
	if(biggest > maximum) {
		v4ScaleP(&a, maximum/biggest);
	}
	*v = a;
}


const Color colorWhite = {0xFFFFFFFF};
const Color colorBlack = {0x000000FF};
const Color colorDarkGrey = {0x333333FF};
const Color colorGrey = {0x999999FF};
const Color colorLightGrey = {0xDDDDDDFF};
const Color colorBlue = {0x0000FFFF};
const Color colorGreen = {0x00FF00FF};
const Color colorRed = {0xFF0000FF};
const Color colorYellow = {0xFFFF00FF};



i32 lineIntersect(Vec2 a1, Vec2 a2, Vec2 b1, Vec2 b2, Vec2* result)
{
	f32 dax = a2.x - a1.x;
	f32 dbx = b2.x - b1.x;

	i32 flipped = 0;
	if(dax * dbx == 0) {
		if(dax == 0 && dbx == 0) {
			//two vertical lines
			if(a1.x == b1.x) {
				if(result) *result = a1;
				return 1;
			}
		} else {
			f32 day = a2.y - a1.y;
			f32 dby = b2.y - b1.y;
			if(dax == 0 && dby == 0) {
				//a is vertical
				//b is horizontal
				if(result) *result = v2(a1.x, b1.y);
				return 1;
			} else if(dbx == 0 && day == 0) {
				//a is horiz
				//b is vertical
				if(result) *result = v2(b1.x, a1.y);
				return 1;
			}
		}

		flipped = 1;
		a1 = v2SwapXY(a1);
		b1 = v2SwapXY(b1);
		a2 = v2SwapXY(a2);
		b2 = v2SwapXY(b2);
	}

	f32 am = (a2.y - a1.y) / (a2.x - a1.x);
	f32 bm = (b2.y - b1.y) / (b2.x - b1.x);

	f32 x = am * a1.x - bm * b1.x + b1.y - a1.y;
	x /= am - bm;
	f32 y = am * (x - a1.x) + a1.y;

	if(result) {
		if(flipped) {
			f32 t = x;
			x = y;
			y = t;
		}
		result->x = x;
		result->y = y;
	}
	return 1;
}

 
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
_PS_CONST(cephes_FOPI, 1.27323954473516); // 4 / M_PI

/* since sin_ps and cos_ps are almost identical, sincos_ps could replace both of them..
   it is almost as fast, and gives you a free cosine with your sine */
void wbtm_sse2_sincos_ps(vf128 x, vf128 *s, vf128 *c) 
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
