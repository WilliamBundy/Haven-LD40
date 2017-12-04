#include <stdio.h>
#include <math.h>
#include <time.h>
#include <Shlwapi.h>

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

void saveGame(wplWindow* window, World* world)
{
	char buf[1024];
	snprintf(buf, 1024, "%shaven.save", window->basePath);
	FILE* f = fopen(buf, "wb");
	if(f) {
		fwrite(world, sizeof(World), 1, f);
	} else {
		printf("Failed to save game!");
	}
}
 
int loadGame(wplWindow* window, World* world)
{
	char buf[1024];
	snprintf(buf, 1024, "%shaven.save", window->basePath);
	if(!PathFileExists(buf)) return 0;
	FILE* f = fopen(buf, "rb");
	if(f) {
		fread(world, sizeof(World), 1, f);
		return 1;
	}
	return 0;
}

int gameLoaded = 0;

void init(wplWindow* window)
{
	createEventTemplates(eventTemplates, &eventTemplateCount);
	gameData.shader = arenaPush(arena, sizeof(wplShader));
	gameData.basicTex = wplLoadTexture(window, "faces.png", arena);
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
	int loaded = loadGame(window, play.world);
	if(play.world->actorCount == 0) loaded = 0;

	gameLoaded = loaded;

	if(loaded) {
		play.world->r = &play.world->randomState;
		for(isize i = 0; i < play.world->actorCount; ++i) {
			Actor* actor = play.world->actors + i;
			if(actor->sex) {
				actor->name = maleNames[actor->nameIndex];
			} else {
				actor->name = femaleNames[actor->nameIndex];
			}
		}
	} else {
		play.world->r = &play.world->randomState;
		initRandom(play.world->r, 1123 * time(0));
		play.world->day = 1;
		play.world->buildings.huts = 1;
		play.world->resources.wood = 20;
		play.world->resources.food = 24;
		for(isize i = 0; i < 3; ++i) {
			addActor(generateActor());
			arenaPop(tempArena);
		}
	}
}

void update(wplWindow* window, wplState* state)
{
	if(gameLoaded) {
		if(uiButtonL(8, state->height / 8 - 16, "Load save game")) {
			gameLoaded = 0;
		}
		if(uiButtonL(8, state->height / 8 + 16, "Start a new game")) {
			memset(play.world, 0, sizeof(World));
			play.world->r = &play.world->randomState;
			initRandom(play.world->r, 1123 * time(0));
			play.world->day = 1;
			play.world->buildings.huts = 1;
			play.world->resources.wood = 20;
			play.world->resources.food = 24;
			for(isize i = 0; i < 3; ++i) {
				addActor(generateActor());
				arenaPop(tempArena);
			}
			gameLoaded = 0;
		}
		textGroup->scale = 4;
		wplGroupDraw(window, state, textGroup);
	} else {
		playUpdate(window, state);
	}
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
	state.input->keyboard = state.input->scancodes;
	while(1) {
		if(!wplUpdate(&window, &state)) {
			if(play.mode == Mode_MorningAssign) {
				saveGame(&window, play.world);
			}
			break;
		}
		mouseX = state.mouseX;
		mouseY = state.mouseY;
		update(&window, &state);
		if(state.exitEvent) {
			break;
		}
		wplRender(&window);
	}
}
