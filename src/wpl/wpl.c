
#include <stdint.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <intrin.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#define WBGL_IMPLEMENTATION
#define WBGL_GL_VERSION_3_3
#define WBGL_SDL
#include "thirdparty/wb_gl.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "thirdparty/stb_image.h"

#define STBRP_DEF
#define STB_RECT_PACK_IMPLEMENTATION
#include "thirdparty/stb_rect_pack.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "thirdparty/stb_image_write.h"

#ifdef WPL_FONT
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "thirdparty/stb_truetype.h"
#endif

#include "wplDefines.h"

#define WB_ALLOC_IMPLEMENTATION
#define WB_ALLOC_CUSTOM_INTEGER_TYPES
#define WB_ALLOC_BACKEND_API static
#include "thirdparty/wb_alloc.h"

#define WBTM_STATIC_IMPLEMENTATION
#define WBTM_CRT_NO_REPLACE
#include "thirdparty/wb_tm.c"


#undef WBTM_STATIC_IMPLEMENTATION
#undef WBTM_API
#include "wplInternal.h"

#include "wplShaders.h"
#include "wplRender.c"

i64 wplInit()
{
	SDL_SetMainReady();
	int ret = SDL_Init(SDL_INIT_EVERYTHING);
	return ret;
}

i64 wplCreateWindow(wplWindowDef* def, wplWindow* window)
{
	i64 wposx, wposy;

#define GLattr(attr, val) SDL_GL_SetAttribute(SDL_GL_##attr, val)
	GLattr(RED_SIZE, 8);
	GLattr(GREEN_SIZE, 8);
	GLattr(BLUE_SIZE, 8);
	GLattr(ALPHA_SIZE, 8);
	GLattr(DEPTH_SIZE, 24);
	GLattr(STENCIL_SIZE, 8);
	GLattr(DOUBLEBUFFER, 1);
	GLattr(FRAMEBUFFER_SRGB_CAPABLE, 1);
	GLattr(CONTEXT_MAJOR_VERSION, 3);
	GLattr(CONTEXT_MINOR_VERSION, 3);
	GLattr(CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	if(def->posCentered) {
		wposx = SDL_WINDOWPOS_CENTERED_DISPLAY(def->monitorIndex);
		wposy = SDL_WINDOWPOS_CENTERED_DISPLAY(def->monitorIndex);
	} else if(def->posUndefined) {
		wposx = SDL_WINDOWPOS_UNDEFINED_DISPLAY(def->monitorIndex);
		wposy = SDL_WINDOWPOS_UNDEFINED_DISPLAY(def->monitorIndex);
	} else {
		wposx = def->x;
		wposy = def->y;
	}

	if(def->width == 0) {
		def->width = 1280;
	} 

	if(def->height == 0) {
		def->height = 720;
	}

	SDL_Window* windowHandle = SDL_CreateWindow(
			def->title,
			wposx, wposy,
			def->width, def->height,
			(def->resizeable ? SDL_WINDOW_RESIZABLE : 0) |
			(def->borderless ? SDL_WINDOW_BORDERLESS : 0) |
			(def->hidden ? SDL_WINDOW_HIDDEN : 0) |
			SDL_WINDOW_OPENGL);

	printf("%s\n", SDL_GetError());
	window->windowHandle = windowHandle;

	SDL_DisplayMode dm = {0};
	SDL_GetWindowDisplayMode(windowHandle, &dm);
	if(dm.refresh_rate == 0) dm.refresh_rate = 60;
	window->refreshRate = dm.refresh_rate;

	SDL_GLContext glContext = SDL_GL_CreateContext(windowHandle);
	//glContext = NULL;
	window->glVersion = 33;
	if(!glContext) {
		fprintf(stderr, "Error: your computer does not support the minimum"  
				"required version of OpenGL (3.3)\n (support may be introduced later)\n");
		GLattr(CONTEXT_MAJOR_VERSION, 3);
		GLattr(CONTEXT_MINOR_VERSION, 1);
		GLattr(CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
		glContext = SDL_GL_CreateContext(windowHandle);
		if(!glContext) {
			GLattr(CONTEXT_MAJOR_VERSION, 2);
			GLattr(CONTEXT_MINOR_VERSION, 1);
			glContext = SDL_GL_CreateContext(windowHandle);
			if(!glContext) {
				return 1;
			}
			window->glVersion = 21;
		} else {
			window->glVersion = 31;
		}
	}

	SDL_GL_MakeCurrent(windowHandle, glContext);
	{
		struct wbgl_ErrorContext ctx;
		wbgl_load(&ctx);
	}

	switch(window->glVersion) {
		case 21:
			window->vertShader = gl21Vert;
			window->fragShader = gl21Frag;
			break;
		case 31:
			window->vertShader = gl21Vert;
			window->fragShader = gl31Frag;
			break;
		case 33:
			window->vertShader = gl33Vert;
			window->fragShader = gl33Frag;
			break;
	}

	glClearColor(0, 0, 0, 1);

	window->basePath = SDL_GetBasePath();

	return windowHandle == NULL ? 0 : 1;
}

void wplShowWindow(wplWindow* wpl)
{
	SDL_ShowWindow(wpl->windowHandle);
}

wplInputState* wplInput;
void wplInputUpdate()
{
	i8* keys;
	isize i;
	keys = wplInput->keycodes;
	for(i = 0; i < SDL_NUM_SCANCODES; ++i) {
		if(keys[i] == Button_JustDown) {
			keys[i] = Button_Down;
		} else if(keys[i] == Button_JustUp) {
			keys[i] = Button_Up;
		}
	}

	keys = wplInput->scancodes;
	for(i = 0; i < SDL_NUM_SCANCODES; ++i) {
		if(keys[i] == Button_JustDown) {
			keys[i] = Button_Down;
		} else if(keys[i] == Button_JustUp) {
			keys[i] = Button_Up;
		}
	}

	keys = wplInput->mouse;
	for(i = 0; i < 16; ++i) {
		if(keys[i] == Button_JustDown) {
			keys[i] = Button_Down;
		} else if(keys[i] == Button_JustUp) {
			keys[i] = Button_Up;
		}
	}

	wplInput->mouseWheel = 0;
}

i64 wplUpdate(wplWindow* window, wplState* state)
{
	window->lastTicks = SDL_GetTicks();
	wplState lstate;
	SDL_Event event;

	{
		int width, height;
		SDL_GetWindowSize(window->windowHandle, &width, &height);
		state->width = width;
		state->height = height;
		glViewport(0, 0, width, height);
	}

	lstate = *state;
	wplInput = state->input;
	wplInputUpdate();
	while(SDL_PollEvent(&event))  {
		switch(event.type) {
			case SDL_QUIT:
				lstate.exitEvent = 1;
				return 0;

			case SDL_MOUSEBUTTONDOWN:
				wplInput->mouse[event.button.button] = Button_JustDown;
				break;
			case SDL_MOUSEBUTTONUP:
				wplInput->mouse[event.button.button] = Button_JustUp;
				break;

			case SDL_MOUSEWHEEL:
				wplInput->mouseWheel = event.wheel.y;
				break;

			case SDL_KEYDOWN:
				wplInput->scancodes[event.key.keysym.scancode] = Button_JustDown;
				if(event.key.keysym.sym < 1024) {
					wplInput->keycodes[event.key.keysym.sym] = Button_JustDown;
				}
				break;

			case SDL_KEYUP:
				wplInput->scancodes[event.key.keysym.scancode] = Button_JustUp;
				if(event.key.keysym.sym < 1024) {
					wplInput->keycodes[event.key.keysym.sym] = Button_JustUp;
				}
				break;
			case SDL_WINDOWEVENT:
				switch(event.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						{
							int width, height;
							SDL_GetWindowSize(window->windowHandle, &width, &height);
							state->width = width;
							state->height = height;
						}
						break;
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						lstate.hasFocus = 1;
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						lstate.hasFocus = 0;
						break;
				}
				break;
		}
	}

	{
		int mx, my;
		SDL_GetMouseState(&mx, &my);
		lstate.mouseX = mx;
		lstate.mouseY = my;
	}

	glViewport(0, 0, state->width, state->height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	*state = lstate;
	return 1;
}

i64 wplRender(wplWindow* window)
{
	SDL_GL_SwapWindow(window->windowHandle);
	window->elapsedTicks = SDL_GetTicks() - window->lastTicks;
	if(window->elapsedTicks < 16) {
		SDL_Delay(16 - window->elapsedTicks);
	}
	return 0;
}

i64 wplKeyIsDown(i64 keycode)
{
	return wplInput->keyboard[keycode] >= Button_Down;
}

i64 wplKeyIsUp(i64 keycode)
{
	return wplInput->keyboard[keycode] <= Button_Up;
}

i64 wplKeyIsJustDown(i64 keycode)
{
	return wplInput->keyboard[keycode] == Button_JustDown;
}

i64 wplKeyIsJustUp(i64 keycode)
{
	return wplInput->keyboard[keycode] == Button_JustUp;
}

i64 wplMouseIsDown(i64 btn)
{
	return wplInput->mouse[btn] >= Button_Down;
}

i64 wplMouseIsUp(i64 btn)
{
	return wplInput->mouse[btn] <= Button_Up;
}

i64 wplMouseIsJustDown(i64 btn)
{
	return wplInput->mouse[btn] == Button_JustDown;
}

i64 wplMouseIsJustUp(i64 btn)
{
	return wplInput->mouse[btn] == Button_JustUp;
}

f32 wplGetMouseWheel() 
{
	return wplInput->mouseWheel;
}


void wplCopyMemoryBlock(void* dest, const void* source, 
		i32 sx, i32 sy, i32 sw, i32 sh,
		i32 dx, i32 dy, i32 dw, i32 dh,
		i32 size, i32 border)
{
	u8* dst = dest;
	const u8* src = source;
	for(isize i = 0; i < sh; ++i) {
		wplCopyMemory(
			dst + ((i+dy) * dw + dx) * size, 
			src + ((i+sy) * sw + sx) * size,
			sw * size);
	}

	if(border) {
		for(isize i = 0; i < sh; ++i) {
			wplCopyMemory(
					dst + ((i+dy) * dw + (dx-1)) * size, 
					dst + ((i+sy) * dw + sx) * size,
					1 * size);

			wplCopyMemory(
					dst + ((i+dy) * dw + (dx+sw)) * size, 
					src + ((i+sy) * dw + (sx+sw-1)) * size,
					1 * size);
		}

		wplCopyMemory(
			dst + ((dy-1) * (dw) + (dx-1)) * size, 
			dst + ((dy) * (dw) + (dx-1)) * size,
			(sw+2) * size);

		wplCopyMemory(
			dst + ((dy+sh) * (dw) + (dx-1)) * size, 
			dst + ((dy+sh-1) * (dw) + (dx-1)) * size,
			(sw+2) * size);
	}
}

#define wbssecpy_mv(a, b, d) \
		head = _mm_lddqu_si128((vi128*)(src + (a))); \
		head2 = _mm_lddqu_si128((vi128*)(src + (b))); \
		tail = _mm_lddqu_si128((vi128*)(src + size - (b))); \
		tail2 = _mm_lddqu_si128((vi128*)(src + size - (d))); \
		_mm_storeu_si128((vi128*)(dst + (a)), head); \
		_mm_storeu_si128((vi128*)(dst + size - (b)), tail); \
		_mm_storeu_si128((vi128*)(dst + (b)), head2); \
		_mm_storeu_si128((vi128*)(dst + size - (d)), tail2);

void* wplCopyMemory(void *dest, const void *source, i64 size)
{
	u8* dst = dest;
	const u8* src = source;

	if(size <= 512) {
		if(size <= 16) {
			u32 src4;
			u16 src2;
			u8  src1;

			u64 head, tail;
			switch(size) {
				case 0: break;
				case 1: *dst = *src; break;
				case 2: *(u16*)dst = *(u16*)src; break;
				case 3: src2 = *(u16*)src;
						src1 = *(src + 2);
						*(u16*)dst = *(u16*)src;
						*(dst + 2) = src1; 
						break;
				case 4: *(u32*)dst = *(u32*)src; break;
				case 5: src4 = *(u32*)src;
						src1 = *(src + 4); 
						*(u32*)dst = src4;
						*(dst + 4) = src1;
						break;
				case 6: src4 = *(u32*)src;
						src2 = *(u16*)(src + 4); 
						*(u32*)dst = src4;
						*(u16*)(dst + 4) = src2;
						break;
				case 7: src4 = *(u32*)src;
						src2 = *(u16*)(src + 4); 
						src1 = *(src + 6);
						*(u32*)dst = src4;
						*(u16*)(dst + 4) = src2;
						*(dst + 6) = src1;
						break;
				case 8: *(u64*)dst = *(u64*)src; break;
				default: head = *(u64*)src;
						 tail = *(u64*)(src + size - 8);
						 *(u64*)dst = head;
						 *(u64*)(dst + size - 8) = tail;
						 break;
			}
		} else if(size <= 32) {
			vi128 head = _mm_lddqu_si128((vi128*)src);
			vi128 tail = _mm_lddqu_si128((vi128*)(src + size - 16));
			_mm_storeu_si128((vi128*)dst, head);
			_mm_storeu_si128((vi128*)(dst + size - 16), tail);
		} else if(size <= 64) {
			vi128 head, head2, tail, tail2;
			wbssecpy_mv( 0, 16, 32);
		} else if(size <= 128) {
			vi128 head, head2, tail, tail2;
			wbssecpy_mv( 0, 16, 32);
			wbssecpy_mv(32, 48, 64);
		} else if(size <= 256) {
			vi128 head, head2, tail, tail2;
			wbssecpy_mv( 0, 16, 32);
			wbssecpy_mv(32, 48, 64);
			wbssecpy_mv(64, 80, 96);
			wbssecpy_mv(96, 112, 128);
		} else if(size <= 512) {
			vi128 head, head2, tail, tail2;
			wbssecpy_mv(0, 16, 32);
			wbssecpy_mv(32, 48, 64);
			wbssecpy_mv(64, 80, 96);
			wbssecpy_mv(96, 112, 128);
			wbssecpy_mv(128, 144, 160);
			wbssecpy_mv(160, 176, 192);
			wbssecpy_mv(192, 208, 224);
			wbssecpy_mv(224, 240, 256);
		}
	} else if(size <= (1 << 21)) {
		__movsb(dst, src, size);
	} else {
		i32 dstalign = (((usize)dst + 15) & -16) == (usize)dst;
		i32 srcalign = (((usize)src + 15) & -16) == (usize)src;
		const vi128* srcm = (vi128*)src;
		vi128* dstm = (vi128*)dst;
		i64 itercount = size >> 4;
		usize end = (usize)src + size - 32;
		if(dstalign && srcalign && size > 32768) {
			switch(itercount % 8) {
				case 0:		_mm_stream_si128(dstm++, _mm_load_si128(srcm++));
				case 7:		_mm_stream_si128(dstm++, _mm_load_si128(srcm++));
				case 6:		_mm_stream_si128(dstm++, _mm_load_si128(srcm++));
				case 5:		_mm_stream_si128(dstm++, _mm_load_si128(srcm++));
				case 4:		_mm_stream_si128(dstm++, _mm_load_si128(srcm++));
				case 3:		_mm_stream_si128(dstm++, _mm_load_si128(srcm++));
				case 2:		_mm_stream_si128(dstm++, _mm_load_si128(srcm++));
				case 1:		_mm_stream_si128(dstm++, _mm_load_si128(srcm++));
			}

			while((usize)srcm + 128 < end) {
				_mm_stream_si128(dstm,     _mm_load_si128(srcm));
				_mm_stream_si128(dstm + 1, _mm_load_si128(srcm + 1));
				_mm_stream_si128(dstm + 2, _mm_load_si128(srcm + 2));
				_mm_stream_si128(dstm + 3, _mm_load_si128(srcm + 3));
				_mm_stream_si128(dstm + 4, _mm_load_si128(srcm + 4));
				_mm_stream_si128(dstm + 5, _mm_load_si128(srcm + 5));
				_mm_stream_si128(dstm + 6, _mm_load_si128(srcm + 6));
				_mm_stream_si128(dstm + 7, _mm_load_si128(srcm + 7));
				dstm += 8;
				srcm += 8;
			}
		} else {
			switch(itercount % 8) {
				case 0:		_mm_storeu_si128(dstm++, _mm_lddqu_si128(srcm++));
				case 7:		_mm_storeu_si128(dstm++, _mm_lddqu_si128(srcm++));
				case 6:		_mm_storeu_si128(dstm++, _mm_lddqu_si128(srcm++));
				case 5:		_mm_storeu_si128(dstm++, _mm_lddqu_si128(srcm++));
				case 4:		_mm_storeu_si128(dstm++, _mm_lddqu_si128(srcm++));
				case 3:		_mm_storeu_si128(dstm++, _mm_lddqu_si128(srcm++));
				case 2:		_mm_storeu_si128(dstm++, _mm_lddqu_si128(srcm++));
				case 1:		_mm_storeu_si128(dstm++, _mm_lddqu_si128(srcm++));
			}
			while((usize)srcm + 128 < end) {
				_mm_storeu_si128(dstm,     _mm_lddqu_si128(srcm));
				_mm_storeu_si128(dstm + 1, _mm_lddqu_si128(srcm + 1));
				_mm_storeu_si128(dstm + 2, _mm_lddqu_si128(srcm + 2));
				_mm_storeu_si128(dstm + 3, _mm_lddqu_si128(srcm + 3));
				_mm_storeu_si128(dstm + 4, _mm_lddqu_si128(srcm + 4));
				_mm_storeu_si128(dstm + 5, _mm_lddqu_si128(srcm + 5));
				_mm_storeu_si128(dstm + 6, _mm_lddqu_si128(srcm + 6));
				_mm_storeu_si128(dstm + 7, _mm_lddqu_si128(srcm + 7));
				dstm += 8;
				srcm += 8;
			}
		}

		while((usize)srcm < end) {
			_mm_storeu_si128(dstm++, _mm_lddqu_si128(srcm++));
		}	

		vi128 tail = _mm_lddqu_si128((vi128*)(src + size - 16));
		_mm_storeu_si128((vi128*)(dst + size - 16), tail);

		tail = _mm_lddqu_si128((vi128*)(src + size - 32));
		_mm_storeu_si128((vi128*)(dst + size - 32), tail);
	} 

	return dst;
}

void wplWriteImage(string filename, i64 w, i64 h, void* data)
{
	stbi_write_png(filename, w, h, 4, data, w * 4);
}

u8* wplReadEntireFile(char* filename, isize* size_out, MemoryArena* arena)
{
	char* str = NULL;
	FILE* fp = fopen(filename, "rb");
	if(fp != NULL) {
		fseek(fp, 0L, SEEK_END);
		isize size = ftell(fp);
		rewind(fp);
		str = arenaPush(arena, size + 1);
		fread(str, sizeof(char), size, fp);
		str[size] = '\0';
		fclose(fp);
		if(size_out != NULL) {
			*size_out = size;
		}
	} else {
		fprintf(stderr, ">>> Could not open file %s\n", filename);
	}
	return str;
}

#ifdef WPL_FONT
void wplPopulateKerning(void* info, wplKerning* k, isize start, isize end)
{
	for(isize i = start; i < end; ++i) {
		for(isize j = start; j < end; ++j) {
			k->kern[i][j] = stbtt_GetCodepointKernAdvance(info, i, j);
		}
	}
}

i32 wplGetGlyphIndexFromCodepoint(void* info, i32 codepoint) 
{
	return stbtt_FindGlyphIndex(info, codepoint);
}

void wplPopulateGlyph(void* info, i32 index, wplGlyph* glyph, f32 scale)
{
	glyph->index = index;
	stbtt_GetGlyphHMetrics(info, index,
			&glyph->advanceWidth,
			&glyph->leftSideBearing);
	stbtt_GetGlyphBox(info, index, 
			&glyph->x1, &glyph->y1,
			&glyph->x2, &glyph->y2);
	glyph->w = glyph->x2 - glyph->x1;
	glyph->h = glyph->y2 - glyph->y1;
	glyph->scale = scale;
	glyph->sw = glyph->w * scale + 2;
	glyph->sh = glyph->h * scale + 2;
}

void* wplRenderGlyph(void* fontInfo, wplGlyph* glyph, MemoryArena* arena)
{
	isize w = glyph->sw;
	isize h = glyph->sh;
	u8* output = arenaPush(arena, w * h + 1);

#if 0
	stbtt_MakeGlyphBitmap(fontInfo,
		output,
		w, h, w,
		glyph->scale, glyph->scale,
		glyph->index);
#else	
	f32 subx, suby;
	stbtt_MakeGlyphBitmapSubpixelPrefilter(
		fontInfo,
		output, 
		w, h, w,
		glyph->scale,
		glyph->scale,
		0, 0,
		1, 1,
		&subx, &suby,
		glyph->index);
#endif
	u32* bitmap = arenaPush(arena, w * h * 4 + 32);

	for(isize i = 0; i < h; ++i) {
		for(isize j = 0; j < w; ++j) {
			u8 val = output[i * w + j];
			if(val > 0) {
				f32 f = (f32)val / 255.0f;
				f *= 0.5;
				f += 0.5;
				f *= f;
				//val = (u8)(f * 255);
			}
			u32 upper = val > 0 ? 0x00FFFFFF : 0;

			u32 pixel = (val << 24) | upper;
			bitmap[i * w + j] = pixel;
		}
	}

	glyph->bitmap = (void*)bitmap;
	return output;
}

void* wplCreateFontContext(void* font, MemoryArena* arena)
{
	stbtt_fontinfo* info = arenaPush(arena, sizeof(stbtt_fontinfo));
	int ret = stbtt_InitFont(info, font, 0);
	return info;
}

void* wplLoadFont(wplWindow* window, string filename, MemoryArena* arena)
{
	stringValidate(&filename);
	char buf[2048];
	snprintf(buf, 2048, "%s%s", window->basePath, wplTempString);
	u8* data = wplReadEntireFile(buf, NULL, arena);
	
	return data;
}

f32 wplGetFontScale(void* font, i64 pixels)
{
	return stbtt_ScaleForPixelHeight(font, pixels);
}
#endif

void wplQuit()
{
	SDL_Quit();
}
