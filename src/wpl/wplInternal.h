typedef struct wplWindowDef wplWindowDef;
typedef struct wplWindow wplWindow;
typedef struct wplInputState wplInputState;
typedef struct wplState wplState;

typedef struct wplSprite wplSprite;
typedef struct wplVertex wplVertex;
typedef struct wplRenderGroup wplRenderGroup;
typedef struct wplShader wplShader;
typedef struct wplTexture wplTexture;

typedef struct wplGlyph wplGlyph;
typedef struct wplKerning wplKerning;

typedef const char* string;

struct wplWindowDef
{
	string title;
	i64 width, height;

	// Position information
	// If centered == 1, 
	// center the window on monitorIndex
	// else, use x, y
	i64 posCentered;
	i64 posUndefined;
	i64 monitorIndex;
	i64 x, y;

	i64 resizeable;
	i64 borderless;
	i64 hidden;
};

struct wplWindow
{
	i64 refreshRate;
	i64 glVersion;
	i64 lastTicks;
	i64 elapsedTicks;
	u8* basePath;
	const u8 *vertShader, *fragShader;
	void* windowHandle;
};

struct wplInputState
{
	i8* keyboard;
	i8 keycodes[1024];
	i8 scancodes[1024];
	i8 mouse[16];
	f32 mouseWheel;
};

struct wplState
{
	wplInputState* input;
	i64 width, height;
	i64 hasFocus;
	i64 mouseX, mouseY;
	i64 exitEvent;
};

/* Graphics */

struct wplSprite
{
	i32 flags;
	u32 color;
	f32 x, y, w, h, cx, cy;
	i16 tx, ty, tw, th;
	f32 angle;
};

struct wplVertex
{
	f32 x, y, u, v;
	u32 color;
};

struct wplShader
{
	u32 vert, frag, program;

	i32 uOrtho;
	i32 uTint;
	i32 uShadow;
	i32 uInvTextureSize;
	i32 uScale;
	i32 uOffset;
	i32 uViewport;
};

struct wplTexture
{
	i64 w, h;
	u8* pixels;
	u32 glIndex;
};

struct wplRenderGroup
{
	wplTexture* texture;
	wplShader* shader;
	u32 vao, vbo;

	i32 blank;
	i32 clearOnDraw;

	f32 scale;
	f32 offsetX, offsetY;
	u32 tint;

	wplSprite* sprites;
	wplVertex* verts;
	i32 *indices, *vertCounts;
	i64 count, capacity, lastFilled;
};

enum SpriteFlags
{
	Anchor_Center = 0,
	Anchor_TopLeft = 1,
	Anchor_TopCenter = 2,
	Anchor_TopRight = 3,
	Anchor_RightCenter = 4,
	Anchor_BottomRight = 5,
	Anchor_BottomCenter = 6,
	Anchor_BottomLeft = 7,
	Anchor_LeftCenter = 8,
	Sprite_Hidden = 1<<4,
	Sprite_NoTexture = 1<<5,
	Sprite_RotateCW = 1<<6,
	Sprite_RotateCCW = 1<<7,
	Sprite_FlipHoriz = 1<<8,
	Sprite_FlipVert = 1<<9,
	Sprite_Circle = 1<<10,
};

enum ButtonState
{
	Button_JustUp = -1,
	Button_Up = 0,
	Button_Down = 1,
	Button_JustDown = 2
};

struct wplGlyph
{
	int index;
	int advanceWidth, leftSideBearing;
	int x1, y1, x2, y2, w, h;
	int sw, sh;
	u8* bitmap;
	void* userdata;
};

struct wplFont
{
	f32 scale;
	i32 kern[256][256];
};


i64 wplCreateWindow(wplWindowDef* def, wplWindow* window);

void wplShowWindow();
i64 wplUpdate(wplWindow* window, wplState* state);

i64 wplKeyIsDown(i64 keycode);
i64 wplKeyIsUp(i64 keycode);
i64 wplKeyIsJustDown(i64 keycode);
i64 wplKeyIsJustUp(i64 keycode);
i64 wplMouseIsDown(i64 btn);
i64 wplMouseIsUp(i64 btn);
i64 wplMouseIsJustDown(i64 btn);
i64 wplMouseIsJustUp(i64 btn);
f32 wplGetMouseWheel();

wplTexture* wplLoadTexture(wplWindow* window, string filename, MemoryArena* arena);
void wplUploadTexture(wplTexture* texture);

void* wplCopyMemory(void *dest, const void *source, i64 size);
void wplCopyMemoryBlock(void* dest, const void* source, 
		i32 sx, i32 sy, i32 sw, i32 sh,
		i32 dx, i32 dy, i32 dw, i32 dh,
		i32 size, i32 border);

void wplWriteImage(string filename, i64 w, i64 h, void* data);
u8* wplReadEntireFile(char* filename, isize* size_out, MemoryArena* arena);

#ifdef WPL_FONT
void wplPopulateKerning(void* info, wplKerning* k, isize start, isize end);
i32 wplGetGlyphIndexFromCodepoint(void* info, i32 codepoint);
void wplPopulateGlyph(void* info, i32 index, wplGlyph* glyph, f32 scale);
void* wplRenderGlyph(void* fontInfo, wplGlyph* glyph, MemoryArena* arena);
void* wplCreateFontContext(void* font, MemoryArena* arena);
void* wplLoadFont(wplWindow* window, string filename, MemoryArena* arena);
f32 wplGetFontScale(void* font, i64 pixels);
#endif








