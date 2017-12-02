typedef union vf32x4 vf32x4;
typedef struct MemoryInfo MemoryInfo;
typedef struct MemoryArena MemoryArena;
typedef struct MemoryPool MemoryPool;
typedef struct wbi__TaggedHeapArena wbi__TaggedHeapArena;
typedef struct TaggedHeap TaggedHeap;
struct SYSTEM_INFO_;
typedef struct sysinfo sysinfo;
typedef struct info info;
typedef struct info info;
typedef struct wplWindowDef wplWindowDef;
typedef struct wplWindow wplWindow;
typedef struct wplInputState wplInputState;
typedef struct wplState wplState;
typedef struct wplSprite wplSprite;
typedef struct wplVertex wplVertex;
typedef struct wplShader wplShader;
typedef struct wplTexture wplTexture;
typedef struct wplRenderGroup wplRenderGroup;
typedef struct wplGlyph wplGlyph;
typedef struct wplFont wplFont;
typedef enum SpriteFlags SpriteFlags;
typedef enum ButtonState ButtonState;
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
typedef size_t usize;
typedef ptrdiff_t isize;
typedef isize isize;
typedef void myhandle;
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef DWORD* DWORD_PTR;
typedef void* LPVOID;
typedef int BOOL;
typedef unsigned long long* PULONGLONG;
typedef usize off_t;
typedef short unsigned int wbi__u16;
typedef unsigned int wbi__u32;
typedef long int wbi__i64;
typedef unsigned long int wbi__u64;
typedef const char* string;

union vf32x4
{
	vf128 v;
	f32 f[4];
};

struct MemoryInfo
{
	usize totalMemory;
	usize commitSize;
	usize pageSize;
	isize commitFlags;
};

struct MemoryArena
{
	const char *name;
	void *start;
	void *head;
	void *end;
	void *tempStart;
	void *tempHead;
	MemoryInfo info;
	isize align;
	isize flags;
};

struct MemoryPool
{
	usize elementSize;
	isize count;
	isize capacity;
	void *slots;
	const char *name;
	void **freeList;
	MemoryArena *alloc;
	isize lastFilled;
	isize flags;
};

struct wbi__TaggedHeapArena
{
	isize tag;
	wbi__TaggedHeapArena *next;
	void *head;
	void *end;
	char buffer;
};

struct TaggedHeap
{
	const char *name;
	MemoryPool pool;
	wbi__TaggedHeapArena *arenas[WB_ALLOC_TAGGEDHEAP_MAX_TAG_COUNT];
	MemoryInfo info;
	usize arenaSize;
	usize align;
	isize flags;
};

typedef struct SYSTEM_INFO_
{
	union {
		DWORD dwOemId;
		struct {
			WORD wProcessorArchitecture;
			WORD wReserved;
		};
	};
	DWORD dwPageSize;
	LPVOID lpMinimumApplicationAddress;
	LPVOID lpMaximumApplicationAddress;
	DWORD_PTR dwActiveProcessorMask;
	DWORD dwNumberOfProcessors;
	DWORD dwProcessorType;
	DWORD dwAllocationGranularity;
	WORD wProcessorLevel;
	WORD wProcessorRevision;
} SYSTEM_INFO;

struct sysinfo
{
	wbi__i64 uptime;
	wbi__u64 loads[3];
	wbi__u64 totalram;
	wbi__u64 freeMemory;
	wbi__u64 sharedMemory;
	wbi__u64 bufferMemory;
	wbi__u64 totalSwap;
	wbi__u64 freeSwap;
	wbi__u16 procs;
	wbi__u16 pad;
	wbi__u64 totalHigh;
	wbi__u64 freeHigh;
	wbi__u32 memUnit;
(null);
	char _f sizeof long sizeof *int[20];
(null);
};

struct info
{
	int mib[2];
	usize size;
	usize eightByte;
	size sizeof usize;
	mib CTL_HW[0];
	mib HW_MEMSIZE[1];
	sysctl mib;
	sysctl (null);
	sysctl eightByte;
	sysctl size;
	sysctl NULL;
	sysctl (null);
	info totalram eightByte;
(null);
	mib CTL_HW[0];
	mib HW_PAGESIZE[1];
	size sizeof int;
	sysctl mib;
	sysctl (null);
	sysctl fourByte;
	sysctl size;
	sysctl NULL;
	sysctl (null);
	info freeMemory wbi__u64 fourByte;
(null);
	return;
};

struct info
{
	int mib[2];
	usize size;
	usize eightByte;
	size sizeof usize;
	mib CTL_HW[0];
	mib HW_MEMSIZE[1];
	sysctl mib;
	sysctl (null);
	sysctl eightByte;
	sysctl size;
	sysctl NULL;
	sysctl (null);
	info totalram eightByte;
(null);
	mib CTL_HW[0];
	mib HW_PAGESIZE[1];
	size sizeof int;
	sysctl mib;
	sysctl (null);
	sysctl fourByte;
	sysctl size;
	sysctl NULL;
	sysctl (null);
	info freeMemory wbi__u64 fourByte;
(null);
	return;
};

struct wplWindowDef
{
	string title;
	i64 width;
	i64 height;
	i64 posCentered;
	i64 posUndefined;
	i64 monitorIndex;
	i64 x;
	i64 y;
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
	u8 *basePath;
	const u8 *vertShader;
	const u8 *fragShader;
	void *windowHandle;
};

struct wplInputState
{
	i8 *keyboard;
	i8 keycodes[1024];
	i8 scancodes[1024];
	i8 mouse[16];
	f32 mouseWheel;
};

struct wplState
{
	wplInputState *input;
	i64 width;
	i64 height;
	i64 hasFocus;
	i64 mouseX;
	i64 mouseY;
	i64 exitEvent;
};

struct wplSprite
{
	i32 flags;
	u32 color;
	f32 x;
	f32 y;
	f32 w;
	f32 h;
	f32 cx;
	f32 cy;
	i16 tx;
	i16 ty;
	i16 tw;
	i16 th;
	f32 angle;
};

struct wplVertex
{
	f32 x;
	f32 y;
	f32 u;
	f32 v;
	u32 color;
};

struct wplShader
{
	u32 vert;
	u32 frag;
	u32 program;
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
	i64 w;
	i64 h;
	u8 *pixels;
	u32 glIndex;
};

struct wplRenderGroup
{
	wplTexture *texture;
	wplShader *shader;
	u32 vao;
	u32 vbo;
	i32 blank;
	i32 clearOnDraw;
	f32 scale;
	f32 offsetX;
	f32 offsetY;
	u32 tint;
	wplSprite *sprites;
	wplVertex *verts;
	i32 *indices;
	i32 *vertCounts;
	i64 count;
	i64 capacity;
	i64 lastFilled;
};

struct wplGlyph
{
	int index;
	int advanceWidth;
	int leftSideBearing;
	int x1;
	int y1;
	int x2;
	int y2;
	int w;
	int h;
	int sw;
	int sh;
	u8 *bitmap;
	void *userdata;
};

struct wplFont
{
	f32 scale;
	i32 kern[256][256];
};

WB_ALLOC_BACKEND_API void* wbi__allocateVirtualSpace(usize size);
WB_ALLOC_BACKEND_API void* wbi__commitMemory(void* addr, usize size, isize flags);
WB_ALLOC_BACKEND_API void wbi__decommitMemory(void* addr, usize size);
WB_ALLOC_BACKEND_API void wbi__freeAddressSpace(void* addr, usize size);
WB_ALLOC_API MemoryInfo getMemoryInfo();
WB_ALLOC_BACKEND_API int __cdecl sysinfo(struct sysinfo* info);
WB_ALLOC_BACKEND_API void* wbi__allocateVirtualSpace(usize size);
WB_ALLOC_BACKEND_API void* wbi__commitMemory(void* addr, usize size, isize flags);
WB_ALLOC_BACKEND_API void wbi__decommitMemory(void* addr, usize size);
WB_ALLOC_BACKEND_API void wbi__freeAddressSpace(void* addr, usize size);
WB_ALLOC_API MemoryInfo getMemoryInfo();
WB_ALLOC_API isize alignTo(usize x, usize align);
WB_ALLOC_API void arenaFixedSizeInit(MemoryArena* arena, void* buffer, isize size, isize flags);
WB_ALLOC_API void arenaInit(MemoryArena* arena, MemoryInfo info, isize flags);
WB_ALLOC_API void* arenaPushEx(MemoryArena* arena, isize size, WB_ALLOC_EXTENDED_INFO extended);
WB_ALLOC_API void* arenaPush(MemoryArena* arena, isize size);
WB_ALLOC_API void arenaPop(MemoryArena* arena);
WB_ALLOC_API MemoryArena* arenaBootstrap(MemoryInfo info, isize flags);
WB_ALLOC_API MemoryArena* arenaFixedSizeBootstrap(void* buffer, usize size, isize flags);
WB_ALLOC_API void arenaStartTemp(MemoryArena* arena);
WB_ALLOC_API void arenaEndTemp(MemoryArena* arena);
WB_ALLOC_API void arenaClear(MemoryArena* arena);
WB_ALLOC_API void arenaDestroy(MemoryArena* arena);
WB_ALLOC_API void poolInit(MemoryPool* pool, MemoryArena* alloc, usize elementSize, isize flags);
WB_ALLOC_API MemoryPool* poolBootstrap(MemoryInfo info, isize elementSize, isize flags);
WB_ALLOC_API MemoryPool* poolFixedSizeBootstrap(isize elementSize, void* buffer, usize size, isize flags);
WB_ALLOC_API void* poolRetrieve(MemoryPool* pool);
WB_ALLOC_API void poolRelease(MemoryPool* pool, void* ptr);
WB_ALLOC_API isize calcTaggedHeapSize(isize arenaSize, isize arenaCount, isize bootstrapped);
WB_ALLOC_API void taggedInit(TaggedHeap* heap, MemoryArena* arena, isize internalArenaSize, isize flags);
WB_ALLOC_API TaggedHeap* taggedBootstrap(MemoryInfo info, isize arenaSize, isize flags);
WB_ALLOC_API TaggedHeap* taggedFixedSizeBootstrap(isize arenaSize, void* buffer, isize bufferSize, isize flags);
WB_ALLOC_API void wbi__taggedArenaInit(TaggedHeap* heap, wbi__TaggedHeapArena* arena, isize tag);
WB_ALLOC_API void wbi__taggedArenaSortBySize(wbi__TaggedHeapArena** array, isize count);
WB_ALLOC_API void* taggedAlloc(TaggedHeap* heap, isize tag, usize size);
WB_ALLOC_API void taggedFree(TaggedHeap* heap, isize tag);
int main(int argc, char** argv);
