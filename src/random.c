
static inline 
u64 splitmix64(u64* x)
{
	*x += UINT64_C(0x9E3779B97F4A7C15);
	u64 z = *x;
	z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
	z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
	return z ^ (z >> 31);	
}

static inline 
u64 rotateLeft(const u64 t, i64 k)
{
	return (t << k) | (t >> (64 - k));
}

typedef struct RandomState RandomState;
struct RandomState
{
	u64 x, y;
};

u64 getRandomU64(RandomState* r)
{
	u64 a = r->x;
	u64 b = r->y;
	u64 result = a + b;

	b ^= a;
	r->x = rotateLeft(a, 55) ^ b ^ (b << 14);
	r->y = rotateLeft(b, 36);
	return result;
}

void initRandom(RandomState* r, u64 seed) 
{
	splitmix64(&seed);
	r->x = splitmix64(&seed);
	r->y = splitmix64(&seed);
	getRandomU64(r);
	getRandomU64(r);
	getRandomU64(r);
	getRandomU64(r);
	getRandomU64(r);
	getRandomU64(r);
	getRandomU64(r);
	getRandomU64(r);
}

f64 getRandomF64(RandomState* r)
{
	return (double)getRandomU64(r) / (double)UINT64_MAX;
}

f32 getRandom(RandomState* r)
{
	return (f32)getRandomF64(r);
}

isize getRandomRange(RandomState* r, isize mn, isize mx)
{
	return (isize)(getRandomF64(r) * (mx - mn) + mn);
}

f64 getRandRangeF64(RandomState* r, f64 mn, f64 mx)
{
	return (isize)(getRandomF64(r) * (mx - mn) + mn);
}
