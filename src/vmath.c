
typedef struct Vec2 Vec2;
struct Vec2 
{
	f32 x, y;
};

static inline
Vec2 v2(f32 x, f32 y)
{
	Vec2 v;
	v.x = x;
	v.y = y;
	return v;
}

Vec2 v2Add(Vec2 a, Vec2 b)
{
	return v2(a.x + b.x, a.y + b.y);
}

typedef struct Rect2i Rect2i;
struct Rect2i
{
	i16 x, y, w, h;
};
