#include <stdio.h>
#include <math.h>

#include "wpl/wpl.h"
#include "vmath.c"
#include "random.c"
#include "names.h"
typedef struct Spritefont Spritefont;
struct Spritefont
{
	Rect2i* glyphs;
};

MemoryInfo gMemInfo;
MemoryArena* arena;
MemoryArena* tempArena;
f32 mouseX, mouseY;


#define _glyph_offset_x 0
#define _glyph_offset_y 0
Rect2i gohufontRects[] = {
#include "gohufont.glyphs"
};

struct {
	wplShader* shader;
	wplTexture* basicTex;
	wplTexture* bgTex;
	wplTexture* gohufontTex;
	Spritefont font;
} gameData;

#include "ui.c"
#include "gamestate.c"

void init(wplWindow* window)
{
	createEventTemplates(eventTemplates, &eventTemplateCount);
	gameData.shader = arenaPush(arena, sizeof(wplShader));
	gameData.basicTex = wplLoadTexture(window, "basic.png", arena);
	gameData.bgTex = wplLoadTexture(window, "bg.png", arena);
	gameData.gohufontTex = wplLoadTexture(window, "gohufont.png", arena);
	wplUploadTexture(gameData.basicTex);
	wplUploadTexture(gameData.bgTex);
	wplUploadTexture(gameData.gohufontTex);

	textGroup = arenaPush(arena, sizeof(wplRenderGroup));
	wplGroupInit(window, textGroup, 2048, gameData.shader,
			gameData.gohufontTex, arena);

	gameData.font.glyphs = gohufontRects;

	playInit(window);
	
	//TODO(will): implement world save/load
	// temporary worldgen/setup goes here
	play.world = arenaPush(play.arena, sizeof(World));
	
	play.world->r = &play.world->randomState;
	initRandom(play.world->r, 1123 * 0xBAF1);

	play.world->day = 1;
	play.world->resources.wood = 50;
	play.world->resources.food = 24;
	for(isize i = 0; i < 12; ++i) {
		addActor(generateActor());
		arenaPop(tempArena);
	}
}

void update(wplWindow* window, wplState* state)
{
	playUpdate(window, state);
}

int main(int argc, char** argv)
{
	gMemInfo = getMemoryInfo();
	arena = arenaBootstrap(gMemInfo, 0);
	tempArena = arenaBootstrap(gMemInfo, FlagArenaStack);

	wplWindowDef def = {0};
	def.title = "Haven";
	def.posCentered = 1; 
	def.monitorIndex = 1;
	//def.resizeable = 1;

	wplWindow window;
	if(!wplCreateWindow(&def, &window)) {
		fprintf(stderr, "Error: could not create window\n");
	}

	init(&window);
	
	wplState state = {0};
	wplInputState inputState = {0};
	state.input = &inputState;
	while(1) {
		if(!wplUpdate(&window, &state)) break;
		mouseX = state.mouseX;
		mouseY = state.mouseY;
		update(&window, &state);
		wplRender(&window);
	}
}
