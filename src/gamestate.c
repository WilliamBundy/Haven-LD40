
/* TODO(will)
 * 	- Need to finish results panel of events
 * 	- Schedule events at time of day
 * 		- Specific events based on day number:
 * 			early days: always single traverelers want to join
 * 			maybe find people in the woods? 
 * 	- Event:
 * 		Traders
 * 		Combat/Raids
 * 	- Huts == max population, can't accept people without more huts
 * 	- Event requires workers in job, not just involved workers
 * 	- Wishful thinking :
	 * 	- Sound effects, "music"
	 * 	- Little graphical effects, like huts in the background
	 * 	- oh, and we need some more/better graphics
	 * 	- story/title prompts
	 * 	- tutorial stuff
	 * 	- main menu
 */
typedef struct World World;
typedef struct Actor Actor;
typedef struct Hut Hut;
typedef struct WorldEvent WorldEvent;

Actor* addActor(Actor* actor);
Actor* generateActor();
typedef int (*EventAction)(World* world, WorldEvent* event, int something, void* ud);

#include "gamedata.c"
struct WorldEvent
{
	string text[16];
	string options[16];
	struct {
		int hasReq;
		int resource;
		int amtMin, amtMax, amt;
	} optionReqs[16];

	string resultText;
	string negativeResultText;
	string resultLines[16];
	int resultValues[16];
	int resultCount, resolved;
	EventAction defaultAction;
	EventAction actions[16];
	int appliesToJobs[ActorJobCount];
	int textCount, optionCount, optionRequiresSelection;
	Actor* involves[8];
	Actor* selected[8];
	int involveCountMax, involveCountMin, involveCount;
	f32 timeoutMin, timeoutMax, timeout;
	int peopleToSelectMin, peopleToSelectMax, peopleSelected;
	int minPopNeeded;
	int kind;
	int jobSpecific;
};


struct Actor
{
	string name;
	f32 aggroChance;
	int state, job, mood;
	int health, food;
	int contribution, contribType;
	int daysConsecutiveWork;
	int sex;
	int positiveTraits[4];
	int negativeTraits[4];
	int selected;
};

struct Hut
{
	Actor* actors[4];
	int condition;
	int timer;
};

struct World 
{
	RandomState randomState;
	RandomState* r;

	Actor actors[256];

	int actorCount;
	int maxPopulation;
	
	int craftTarget;
	int buildTarget;

	int craftWorkNeeded;
	int buildWorkNeeded;

	struct {
		int population;
		int wood, food;
		int tools, weapons, artifacts;
	} resources;

	struct {
		int huts;
		int farms;
		int smiths;
	} buildings;

	int jobCount[ActorJobCount];

	int day;
};
#include "eventProcs.c"

#define DayTimeInFrames 7200
//#define DayTimeInFrames 360

enum PlayModes
{
	Mode_MorningAssign,
	Mode_DayEvents,
	Mode_EveningReview,
	Mode_NightEvents
};

struct PlayState {
	MemoryArena* arena;
	wplRenderGroup* group;

	World* world;
	Actor* removed[256];
	string deadNames[256];
	isize removedCount;
	isize deadCount;
	f32 actorScroll;

	WorldEvent events[256];
	int eventCount, activeEvent;
	int eventSpan, nextEventTime;
	int dayTimer;

	int mode;
} play;
#include "eventGen.c"


Actor* generateActor()
{
	World* world = play.world;
	Actor* actor = arenaPush(tempArena, sizeof(Actor));
	actor->mood = getRandomRange(world->r, 4,6);
	actor->health = getRandomRange(world->r, 8, 12);
	actor->food = getRandomRange(world->r, 38, 50);
	actor->sex = getRandom(world->r) < 0.50002 ? 1 : 0;
	if(actor->sex) {
		actor->name = maleNames[getRandomRange(world->r, 0, 80)];
	} else {
		actor->name = femaleNames[getRandomRange(world->r, 0, 80)];
	}
	
	int numTraits = getRandomRange(world->r, 2, 7);
	int badTraits = numTraits / 2;
	int goodTraits = numTraits - badTraits;

	for(isize i = 0; i < goodTraits; ++i) {
		//check for uniqueness
		while(1) {
			actor->positiveTraits[i] = getRandomRange(world->r, 1, PositiveTraitCount);
			int fin = 1;
			for(isize j = 0; j < 4; j++) {
				if(j == i) continue;
				if(actor->positiveTraits[j] == actor->positiveTraits[i])  {
					fin = 0;
					break;
				}
			}
			if(fin) break;
		}
	}

	for(isize i = 0; i < badTraits; ++i) {
		//check for uniqueness
		//the enums are set up so that the positive trait and negative trait
		//have the same number. We never want to have both the positive/negative
		//versions of a trait; that'd make no sense, so we check for no dupes
		//and for no alts
		while(1) {
			actor->negativeTraits[i] = getRandomRange(world->r, 1, NegativeTraitCount);
			int fin = 1;
			for(isize j = 0; j < 4; j++) {
				if(actor->positiveTraits[j] == actor->negativeTraits[i])  {
					fin = 0;
					break;
				}
				
				if(j == i) continue;

				if(actor->negativeTraits[j] == actor->negativeTraits[i]) {
					fin = 0;
					break;
				}
			}
			if(fin) break;
		}
	}

	return actor;
}

Actor* addActor(Actor* actor)
{
	if(play.world->actorCount >= 256) return NULL;
	play.world->actors[play.world->actorCount++] = *actor;
	return play.world->actors + play.world->actorCount - 1;
}


#define Face1 64, 80, (5*16), 0
#define Face2 64, 80, (10*16), 0

void addSprite(f32 x, f32 y, f32 w, f32 h, f32 tx, f32 ty)
{
	wplGroupAdd(play.group, Anchor_TopLeft, x, y, w, h, tx, ty, w, h);
}

void addSpriteS(f32 x, f32 y, f32 w, f32 h, f32 tx, f32 ty, f32 s)
{
	wplGroupAdd(play.group, Anchor_TopLeft, x, y, w * s, h * s, tx, ty, w, h);
}

#define ActorCardWidth 72
#define ActorCardHeight 128
void drawActor(Actor* actor, f32 x, f32 y)
{
	//box: 72 wide, 120+padding tall
	if(actor->name == NULL) return;

	wplSprite* s = wplGetSprite(play.group);
	s->x = x;
	s->y = y;
	s->w = ActorCardWidth;
	s->h = ActorCardHeight;
	s->flags = Sprite_NoTexture | Anchor_TopLeft;
	s->color = 0x00000099;

	if(play.mode == Mode_DayEvents) {
		if(play.activeEvent != -1) {
			WorldEvent* e = play.events + play.activeEvent;
			if(e->peopleToSelectMin > 0) {
				if(e->jobSpecific != -1) {
					int job = e->jobSpecific;
					for(isize i = 0; i < e->involveCount; ++i) {
						if(e->involves[i] == actor) {
							s->color = 0x44;
							break;
						}
					}

					if(s->color != 0x44) {
						if(job == actor->job) {
							s->color = 0x11880099;
						} else {
							s->color = 0x66000099;
						}
					}
				} else {
					s->color = 0x33FF0099;
				}
			}
		} else {
			s->color = 0x00000099;
		}
	} else {
		s->color = 0x00000099;
	}

	if(actor->selected) {
		s->color = 0x666666CC;
	} 

	if(actor->sex) {
		addSpriteS(x + 4, y, Face2, 0.5);
	} else {
		addSpriteS(x + 4, y, Face1, 0.5	);
	}

	if(actor->health < -10) {
		drawText(x + 4, y + 44, actor->name);
		drawText(x + 4, y + 60, "is dead");
		return;
	}

	char buf[256];
	snprintf(buf, 256, "Mood:%d", actor->mood);
	drawText(x + 40, y + 4, buf);
	snprintf(buf, 256, "Food:%d", actor->food);
	drawText(x + 40, y + 14, buf);
	snprintf(buf, 256, "HP: %d", actor->health);
	drawText(x + 40, y + 24, buf);
	snprintf(buf, 256, "Work: \n%d", actor->daysConsecutiveWork);
	drawText(x + 40, y + 34, buf);

	drawText(x + 4, y + 44, actor->name);

	drawText(x + 4, y + 60, jobDescs[actor->job]);
	drawText(x + 4, y + 68, astateDescs[actor->state]);
	f32 pty = y + 80;
	f32 nty = pty;
	for(isize i = 0; i < 4; ++i) {
		int pt = actor->positiveTraits[i];
		int nt = actor->negativeTraits[i];

		if(pt > 0) {
			drawText(x + 4, pty, posTraitNames[pt]);
			pty += 8;
		}

		if(nt > 0) {
			drawTextR(x + s->w - 4, nty, negTraitNames[nt]);
			nty += 8;
		}
	}

	if(actor->contribution > 0 && actor->contribType > 0) {
		snprintf(buf, 256, "Produced %d %s", actor->contribution,
				jobContribType[actor->contribType]);

		drawTextSW(x + 4, pty+8, buf, 0.5, 56);
	}
}

int drawEventTab(WorldEvent* e, f32 x, f32 y)
{
	wplSprite* s = wplGetSprite(textGroup);
	s->x = x;
	s->y = y;
	s->w = 24;
	s->h = 24;
	s->flags = Sprite_NoTexture | Anchor_TopLeft;

	int mouseIn = 0, mouseState = 0;
	f32 mx, my;
	getMouse(textGroup, &mx, &my);
	if(mx > x && my > y && mx < (x + s->w) && my < (y + s->h)) {
		mouseIn = 1;
		mouseState = wplMouseIsDown(1);
		if(!mouseState) {
			mouseState = wplMouseIsJustUp(1) * 2;
		}
	}

	s->color = eventKindColors[e->kind];
	s->color &= ~0xFF;
	if(!mouseState && !mouseIn) {
		s->color |= 0x66;
	} else if(mouseIn) {
		if(mouseState == 1) {
			s->color |= 0xBB;
		}  else {
			s->color |= 0x99;
		}
	}

	drawText(x + 4, y + 2, "!!!");

	s = wplGetSprite(textGroup);
	s->x = x + 4;
	s->y = 12 + y;
	s->w = 16 * (1.0 < e->timeout ? 1.0 : e->timeout);
	s->h = 8;
	s->flags = Sprite_NoTexture | Anchor_TopLeft;
	s->color = 0xFFFFFFFF;


	return mouseIn && mouseState == 2;
}

int worldGetResource(int res)
{
	World* world = play.world;
	switch(res) {
		case Resource_Food: return world->resources.food;
		case Resource_Wood: return world->resources.wood;
		case Resource_Tools: return world->resources.tools;
		case Resource_Weapons: return world->resources.weapons;
		case Resource_Artifacts: return world->resources.artifacts;
		default: return 0;
	}
	return 0;
}


#define EventPanelWidth(sw) (sw) / 4 - ActorCardWidth - 24 
#define EventPanelHeight(sh) (sh) / 2 - 48
int drawEventPanel(WorldEvent* e, wplState* state)
{
	wplSprite* s = wplGetSprite(textGroup);
	s->x = 8;
	s->y = 40;
	s->w = EventPanelWidth(state->width);
	s->h = EventPanelHeight(state->height);
	s->flags = Sprite_NoTexture | Anchor_TopLeft;
	s->color = 0x66;

	int textWidth = s->w - 40;

	int eventResolved = 0;
	if(e->resolved) {
		e->timeout = 1;

		if(uiButton(16, 48, "Close")) {
			eventResolved = 1;
		}

		f32 y = 22;
		if(e->resultText) {
			y += drawTextSW(s->x + 8, s->y + y, e->resultText, 1, textWidth);
			y += 4;
		}

		int valueIndex = 0;
		for(isize i = 0; i < e->resultCount; ++i) {
			string tp = e->resultLines[i];
			char buf[256];
			if(stringContains(e->resultLines[i], '%')) {
				snprintf(buf, 256, e->resultLines[i], e->resultValues[valueIndex++]);
				tp = buf;
			}
			y += drawTextSW(s->x + 8, s->y + y, tp, 0.5, textWidth) * 0.5;
			y += 4;
		}

	} else {
		f32 height = 0;
		int textPersonIndex = 0;
		for(isize i = 0; i < e->textCount; ++i) {
			string tp = e->text[i];
			char buf[256];
			if(stringContains(e->text[i], '%')) {
				if(e->involves[textPersonIndex]) {
					snprintf(buf, 256, e->text[i], e->involves[textPersonIndex++]->name);
					tp = buf;
				}
			}
			height += drawTextSW(s->x + 8, height +  s->y + 8, tp,
					i == 0 ? 1 : 0.5, textWidth) * (i == 0 ? 1 : 0.5);
		}
		int eventResolved = 0;
		f32 opty = s->y + height + 16;
		int personIndex = 0;
		for(isize i = 0; i < e->optionCount; ++i) {
			if(!e->options[i]) continue;
			string tp = e->options[i];
			char buf[256];
			if(stringContains(e->options[i], '%')) {
				snprintf(buf, 256, e->options[i], e->involves[personIndex++]->name);
				tp = buf;
			}
			if(e->optionRequiresSelection == i) {
				if(e->peopleSelected >= e->peopleToSelectMin) {
					if(uiButton(16, opty, tp)) {
						if(e->actions[i]) e->actions[i](play.world, e, 0, NULL);
						eventResolved = 1;
					}
				} else {
					char buf[256];
					isize c = snprintf(buf, 256, "You need to select at least %d people",
							e->peopleToSelectMin - e->peopleSelected);

					drawText(16, opty, buf);
					opty += 10;
					drawText(16 + 8, opty, e->options[i]);
				}
			} else if(e->optionReqs[i].hasReq) {
				char buf[256];
				snprintf(buf, 256, "Cost: %d %s", e->optionReqs[i].amt, 
						resourceNames[e->optionReqs[i].resource]);
				drawText(16, opty, buf);
				opty += 10;
				if(worldGetResource(e->optionReqs[i].resource) >= e->optionReqs[i].amt)	{
					//can buy
					if(uiButton(16, opty, tp)) {
						if(e->actions[i]) e->actions[i](play.world, e, 0, NULL);
						eventResolved = 1;
					}
				} else {
					// can't buy
					drawText(16, opty, "You cannot afford this.");
				}
			} else {
				if(uiButton(16, opty, tp)) {
					if(e->actions[i]) e->actions[i](play.world, e, 0, NULL);
					eventResolved = 1;
				}
			}
			opty += 16;
		}

		e->resolved = eventResolved;
		eventResolved = 0;
		s = wplGetSprite(textGroup);
		s->x = 16;
		s->y = 40 + EventPanelHeight(state->height) - 16;
		s->w = (EventPanelWidth(state->width) - 16) * (1.0 < e->timeout ? 1.0 : e->timeout);
		s->h = 8;
		s->flags = Sprite_NoTexture | Anchor_TopLeft;
		s->color = 0xFFFFFFFF;
	}

	return eventResolved;
}

void playInit(wplWindow* window)
{
	play.arena = arenaBootstrap(gMemInfo, 0);
	play.group = arenaPush(play.arena, sizeof(wplRenderGroup));
	wplGroupInit(window, play.group, 2048,
			gameData.shader, gameData.bgTex, play.arena);

	play.dayTimer = -1;
	play.activeEvent = -1;
	play.nextEventTime = DayTimeInFrames / 28;
	play.mode = Mode_MorningAssign;
}

void playUpdate(wplWindow* window, wplState* state)
{
	play.group->texture = gameData.bgTex;
	play.group->scale = 1;
	wplSprite* bgs = wplGroupAdd(play.group, 
			Anchor_TopLeft, 0, 0, state->width, state->height, 0, 0, 1280, 720);
	if(play.mode == Mode_DayEvents) {
		f32 timep = (f32)play.dayTimer / (f32)DayTimeInFrames; 
		int c = 0xFF;
		int a = 0xFF;
		if(timep < 0.5) {
			c = (int)((timep + 0.5) * 255);
			if(c > 255) c = 255;
		}

		if(timep < 0.25) {
			a = (int)((timep+0.5) * 255);
			if(a > 255) a = 255;
		}

		bgs->color = a << 24 | a << 16 | c << 8 | 0xFF;
	} else {
		if(play.dayTimer < 0) {
			bgs->color = 0x333333FF;
		} else {
			bgs->color = 0;
		}
	}
	wplGroupDraw(window, state, play.group);

	play.group->scale = 2;
	play.group->texture = gameData.basicTex;

	World* world = play.world;

	if(world->actorCount <= 0) {
		drawTextS(8, 8, "Everybody died. You lose.", 2);
		wplGroupDraw(window, state, textGroup);
		return;
	}

	/*
	if(wplKeyIsJustDown(60)) {
		world->resources.artifacts = 3;
	}
	*/


	if(world->resources.artifacts >= 3) {
		drawTextS(4, 8, "You gathered all the artifacts! You win!", 2);

		char buf[256];

		snprintf(buf, 256, "It took you %d days", world->day);
		drawTextS(4, 32, buf, 2);
		f32 by = 64;
		snprintf(buf, 256, "Food: %d", world->resources.food);
		drawText(8, by, buf);

		snprintf(buf, 256, "Huts: %d", world->buildings.huts);
		drawText(96, by, buf);
		by += 10;

		snprintf(buf, 256, "Wood: %d", world->resources.wood);
		drawText(8, by, buf);
		snprintf(buf, 256, "Farms: %d", world->buildings.farms);
		drawText(96, by, buf);
		by += 10;

		snprintf(buf, 256, "Population %d", world->actorCount);
		drawText(8, by, buf);
		snprintf(buf, 256, "Smiths: %d", world->buildings.smiths);
		drawText(96, by, buf);
		by += 10;
		snprintf(buf, 256, "Tools: %d", world->resources.tools);
		drawText(8, by, buf);
		by += 10;

		snprintf(buf, 256, "Weapons: %d", world->resources.weapons);
		drawText(8, by, buf);
		by += 10;

		snprintf(buf, 256, "Artifacts: %d", world->resources.artifacts);
		drawText(8, by, buf);
		by += 10;


		wplGroupDraw(window, state, textGroup);
		return;

	}

	if(play.mode == Mode_MorningAssign) {
		drawText(8, 8, "Good morning! Please assign workers:");
		if(uiButton(8, 20, "Confirm")) {
			play.dayTimer = DayTimeInFrames - DayTimeInFrames / 4;
			play.mode = Mode_DayEvents;

			memset(world->jobCount, 0, sizeof(int) * ActorJobCount);
			for(isize i = 0; i < world->actorCount; ++i) {
				Actor* a = world->actors + i;
				world->jobCount[a->job]++;
			}

			for(isize i = 0; i < world->actorCount; ++i) {
				Actor* a = world->actors + i;
				int lastState = a->state;

				if(a->food < -500) {
					a->state = ActorState_Suffering;
					a->health -= 5;
				} else if(a->food < -150) {
					a->state = ActorState_Hungry;
					a->health--;
				} else {
					if(a->job == ActorJob_None) {
						a->state = ActorState_Idle;
						a->daysConsecutiveWork = 0;
					} else if(a->mood < -1) {
						a->state = ActorState_Moping;
					} else {
						a->state = ActorState_Working;
					}

				}

				if(a->health < -10) {
					play.removed[play.removedCount++] = a;
				}

			}

		}

		if(uiButton(80, 20, "Set everyone to idle")) {
			for(isize i = 0; i < world->actorCount; ++i) {
				Actor* a = world->actors + i;
				a->job = ActorJob_None;
			}
		}


		if(world->craftTarget == 0) {
			drawText(8, 36, "No crafting target");
		} else {
			char buf[256];
			snprintf(buf, 256, "Crafting a %s: %d work left", 
					craftTargets[world->craftTarget], world->craftWorkNeeded);
			drawText(8, 36, buf);
		}

		if(world->buildTarget == 0) {
			drawText(8, 44, "No building target");
		} else {
			char buf[256];
			snprintf(buf, 256, "Building a %s: %d work left", 
					buildTargets[world->buildTarget], world->buildWorkNeeded);
			drawText(8, 44, buf);
		}

		f32 by = 64;
		
		char buf[256];
		snprintf(buf, 256, "Food: %d", world->resources.food);
		drawText(8, by, buf);

		snprintf(buf, 256, "Huts: %d", world->buildings.huts);
		drawText(96, by, buf);
		by += 10;

		snprintf(buf, 256, "Wood: %d", world->resources.wood);
		drawText(8, by, buf);
		snprintf(buf, 256, "Farms: %d", world->buildings.farms);
		drawText(96, by, buf);
		by += 10;

		snprintf(buf, 256, "Population %d", world->actorCount);
		drawText(8, by, buf);
		snprintf(buf, 256, "Smiths: %d", world->buildings.smiths);
		drawText(96, by, buf);
		by += 10;
		snprintf(buf, 256, "Tools: %d", world->resources.tools);
		drawText(8, by, buf);
		by += 10;

		snprintf(buf, 256, "Weapons: %d", world->resources.weapons);
		drawText(8, by, buf);
		by += 10;

		snprintf(buf, 256, "Artifacts: %d", world->resources.artifacts);
		drawText(8, by, buf);
		by += 10;

		by +=  8;

		drawText(8, by, "1 wood, 5 work. Improves build/craft speed by 10%");
		by += 10;
		if(uiButton(8, by, "Craft Tool")) {
			world->craftWorkNeeded = 5;
			world->craftTarget = 1;
		}
		by += 22;

		drawText(8, by, "1 wood, 8 work. Allows people to defend the haven");
		by += 10;
		if(uiButton(8, by, "Craft Weapon")) {
			world->craftWorkNeeded = 5;
			world->craftTarget = 2;
		}
		by += 22;

		drawText(8, by, "15 wood, 20 work. Houses six people");
		by += 10;
		if(uiButton(8, by, "Build Hut")) {
			world->buildWorkNeeded = 20;
			world->buildTarget = 1;
		}
		by += 22;

		drawText(8, by, "5 wood, 50 work. Increases food production by 25%");
		by += 10;
		if(uiButton(8, by, "Build Farm")) {
			world->buildWorkNeeded = 50;
			world->buildTarget = 2;
		}
		by += 22;

		drawText(8, by, "25 wood, 100 work. Lets you understand artifacts");
		by += 10;
		if(uiButton(8, by, "Build Smith")) {
			world->buildWorkNeeded = 100; 
			world->buildTarget = 3;
		}
		by += 22;

	
		if(world->day == 1) {
			drawTextS(EventPanelWidth(state->width) + 8, state->height / 4 - 18,
					"Click on a person\nto cycle through jobs", 2);
			drawTextS(EventPanelWidth(state->width) + 8, state->height / 4 + 64,
					"You want to gather food and craft tools to start", 1);
		}

	} else if(play.mode == Mode_DayEvents) {
		int lastDayTimer = play.dayTimer;

		if(uiButton(128-16 , 14, "+1h")) {
			play.dayTimer -= DayTimeInFrames / 24;
		}
		if(uiButton(96+64, 14, "+4h")) {
			play.dayTimer -= DayTimeInFrames / 6;
		}
		play.dayTimer--;

		if(play.dayTimer <= 0) {
			play.mode = Mode_EveningReview;
			play.dayTimer = -1;
			play.activeEvent = -1;

			for(isize i = 0; i < play.eventCount; ++i) {
				WorldEvent* e = play.events + i;
				if(e->defaultAction)
					e->defaultAction(world, e, 0, NULL);
			}
			play.eventCount = 0;
			world->day++;

			f32 woodGather = 0;
			f32 foodGather = 0;
			f32 craftWork = 0;
			f32 buildWork = 0;
			f32 foodConsumed = 0;

			f32 artifactMod = 2 * world->resources.artifacts * world->buildings.smiths;
			f32 foodMod = artifactMod + world->buildings.farms * 0.25f;
			f32 woodMod = artifactMod;
			f32 buildCraftMod = artifactMod + world->resources.tools * 0.1f;
			//TODO(will): implement wood gather falloff as time goes on.
			for(isize i = 0; i < world->actorCount; ++i) {
				Actor* a = world->actors + i;
				if(a->health < -10) continue;
				a->food -= getRandomRange(world->r, 95, 105);
				//TODO(will) modulate based on skills
				foodConsumed += 1;
				f32 workAmt = 1 + (a->health > 0 ? (a->health / 20.0) : 0);
				if(a->state == ActorState_Suffering) {
					workAmt = 0;
				} else if(a->state == ActorState_Hungry) {
					workAmt = 0.5;
				} else if(a->state == ActorState_Moping) {
					workAmt = 1.0 / a->daysConsecutiveWork;
				}

				for(isize j = 0; j < 4; ++j) {
					workAmt *= posTraitWorkMod[a->positiveTraits[j]];
					workAmt *= negTraitWorkMod[a->negativeTraits[j]];
				}

				a->daysConsecutiveWork++;

				if(a->daysConsecutiveWork > 4) {
					int moodLoss = a->daysConsecutiveWork - 4;
					a->mood -= moodLoss > 10 ? 10 : moodLoss;
				}

				f32 amt = 0;
				switch(a->job) {
					case ActorState_Idle:
						if(a->mood < 0) a->mood /= 2;
						a->mood += 2;
						if(a->mood > 10) a->mood = 10;
						break;
					case ActorJob_FoodGather:
						amt = 1.5 + getRandom(world->r) * 0.5;
						amt *= workAmt * foodMod;
						foodGather += amt;
						break;
					case ActorJob_WoodGather:
						amt = 0.9 + getRandom(world->r) * 0.25;
						amt *= workAmt * woodMod;
						woodGather += amt;
						break;
					case ActorJob_Crafting:
						amt = 1 + getRandom(world->r) * 0.1;
						amt *= workAmt * buildCraftMod;
						craftWork += amt;
						break;
					case ActorJob_Building:
						amt = 1 + getRandom(world->r) * 0.1;
						amt *= workAmt * buildCraftMod;
						buildWork += amt;
						break;
				}
				a->contribution = (int)(amt);
				a->contribType = a->job;
			}

			f32 foodAvailable = world->resources.food + foodGather;
			f32 foodEaten = foodAvailable < foodConsumed ? 
				foodAvailable : foodConsumed;
			foodAvailable -= foodEaten;

			//TODO(will) move crafting costs/names/etc to arrays 
			world->resources.food = foodAvailable;
			world->resources.wood += (int)woodGather;
			if(world->craftTarget) {
				world->craftWorkNeeded -= craftWork;
				if(world->craftWorkNeeded <= 0) {
					int craftCost = 1;
					if(world->resources.wood >= craftCost) {
						world->resources.wood -= craftCost;
						if(world->craftTarget == 1) {
							world->resources.tools++;
						} else if(world->craftTarget == 2) {
							world->resources.weapons++;
						}
						world->craftTarget = 0;
						world->craftWorkNeeded = 0;
					}
				}
			}

			if(world->buildTarget) {
				world->buildWorkNeeded -= buildWork;
				if(world->buildWorkNeeded <= 0) {
					int craftCost = 1;
					if(world->buildTarget == 1) {
						craftCost = 15; 
					} else if(world->buildTarget == 2) {
						craftCost = 5;
					} else if(world->buildTarget == 3) {
						craftCost = 25;
					}
					if(world->resources.wood >= craftCost) {
						world->resources.wood -= craftCost;
						if(world->buildTarget == 1) {
							world->buildings.huts++;
						} else if(world->buildTarget == 2) {
							world->buildings.farms++;
						} else if(world->buildTarget == 3) {
							world->buildings.smiths++;
						}

						world->buildTarget = 0;
						world->buildWorkNeeded = 0;
					}


					//TODO(will) increment correct resource for craft
				}
			}

			{ //food consumption
				Actor* hungry[256];
				isize hungryCount = 0;
				foodConsumed = 0;
				for(isize i = 0; i < world->actorCount; ++i) {
					Actor* a = world->actors + i;
					a->food += 100 * (int)(foodEaten / world->actorCount);
					if(a->food < 100) {
						hungry[hungryCount++] = a;
						foodConsumed += 0.5;
					}
				}

				foodAvailable = world->resources.food;
				foodEaten = foodAvailable < foodConsumed ? 
					foodAvailable : foodConsumed;
				foodAvailable -= foodEaten;
				world->resources.food = foodAvailable;

				for(isize i = 0; i < hungryCount; ++i) {
					Actor* a = hungry[i];
					a->food += 50 * (int)(foodEaten / hungryCount);
				}
				for(isize i = 0; i < world->actorCount; ++i) {
					Actor* a = world->actors + i;
					a->selected = 0;
					if(a->food > 100 && a->mood > 0) {
						a->health++;
						if(a->health > 10) {
							a->health = 10;
						}
					}
				}

				play.deadCount = 0;
				//lol worst remove ever
				for(isize i = 0; i < play.removedCount; ++i) {
					play.deadNames[i] = play.removed[i]->name;
					play.removed[i]->name = NULL;
					for(isize j = 0; j < world->actorCount; ++j) {
						Actor* a = world->actors + j;
						a->mood--;
					}
				}
				play.deadCount = play.removedCount;

				for(isize i = 0; i < world->actorCount; ++i) {
					Actor* a = world->actors + i;
					if(a->name == NULL) {
						if(world->actorCount == 1) {
							world->actorCount = 0;
						} else {
							world->actors[i] = world->actors[--world->actorCount];
						}

					}
				}

				for(isize i = 0; i < world->actorCount; ++i) {
					Actor* a = world->actors + i;
					if(a->name == NULL) {
						if(world->actorCount == 1) {
							world->actorCount = 0;
						} else if(world->actorCount == i - 1) {
							--world->actorCount;
						} else {
							world->actors[i] = world->actors[--world->actorCount];
						}
					}
				}

				play.removedCount = 0;
			}
		}

		play.eventSpan += lastDayTimer - play.dayTimer;

		if(world->day <= 5) {
			int tod = DayTimeInFrames - (9 * (DayTimeInFrames / 24));
			if(tod <= lastDayTimer && tod >= play.dayTimer) {
				getSpecificEvent(0);
			}
		}

		if(world->day > 10) {
			//buyArtifact every 3rd day, 13+ people
			int tod = DayTimeInFrames - (18 * (DayTimeInFrames / 24));
			if(world->actorCount > 12 && world->day % 3 == 0) {
				if(tod <= lastDayTimer && tod >= play.dayTimer) {
					getSpecificEvent(1);
				}
			} else if(world->actorCount > 18 && world->day % 2 == 0) {
				if(tod <= lastDayTimer && tod >= play.dayTimer) {
					getSpecificEvent(1);
				}
			} else if(world->actorCount > 24 && world->resources.artifacts > 0)  {
				if(tod <= lastDayTimer && tod >= play.dayTimer) {
					getSpecificEvent(2);
				}
			}
		}
		while(play.eventSpan > play.nextEventTime) {
			play.eventSpan -= play.nextEventTime;
			if(play.eventSpan <= 0) play.eventSpan = 0;
			play.nextEventTime = getRandomRange(world->r, 
					DayTimeInFrames / 96, DayTimeInFrames / 40);
			//trigger event
			int eventRate = world->actorCount / 3 + 1;
			if(world->day > 5) {
				eventRate += world->day / 5;
			}

			eventRate = getRandomRange(world->r, 
					eventRate / 2 + 1, eventRate);

			for(isize i = 0; i < eventRate; ++i) {
				const i32 eventChances[] = {
					Event_Gift,     Event_Gift,
					Event_Solo,     Event_Solo, Event_Solo,
					Event_Accident, Event_Accident,
					Event_Conflict, Event_Conflict, Event_Conflict, Event_Conflict,
					Event_Outsider, Event_Outsider, Event_Outsider, Event_Outsider,
					Event_Gift,     Event_Gift,
					Event_Solo,     Event_Solo, Event_Solo,
					Event_Accident, Event_Accident,
					Event_Conflict, Event_Conflict, Event_Conflict, Event_Conflict,
					Event_Outsider, Event_Outsider, Event_Outsider, Event_Outsider,
					Event_Gift,     Event_Gift,
					Event_Solo,     Event_Solo, Event_Solo,
					Event_Accident, Event_Accident,
					Event_Conflict, Event_Conflict, Event_Conflict, Event_Conflict,
					Event_Outsider, Event_Outsider, Event_Outsider, Event_Outsider,
				};
				i32 eventChanceCount = sizeof(eventChances) / 4;
				i32 chosen = getRandomRange(world->r, 0, eventChanceCount);
				switch(chosen) {
					case Event_Gift: generateGiftEvent(); break;
					case Event_Solo: generateSoloEvent(); break;
					case Event_Accident: generateAccidentEvent(); break;
					case Event_Conflict: generateConflictEvent(); break;
					case Event_Outsider: generateOutsiderEvent(); break;
				}
			}
		}

		f32 timep = (f32)play.dayTimer / (f32)DayTimeInFrames; 
		f32 hours = (1-timep) * 24;
		if(play.activeEvent == -1) {
			f32 eventX = 8, eventY = 64;
			for(isize i = 0; i < play.eventCount; ++i) {
				//char buf[256];
				//snprintf(buf, 256, "%s", );
				WorldEvent* event = play.events + i;
				int ret = drawEventTab(event, eventX, eventY);
				eventX += 24 + 4;
				if(eventX + 24 > (state->width / 4 - ActorCardWidth - 8)) {
					eventX = 8;
					eventY += 28;
				}

				if(ret) {
					play.activeEvent = i;
					for(isize i = 0; i < world->actorCount; ++i) {
						Actor* a = world->actors + i;
						a->selected = 0;
					}
				}
			}
		} else {
			int resolved = drawEventPanel(play.events + play.activeEvent, state);
			if(resolved) {
				play.events[play.activeEvent] = play.events[--play.eventCount];
				play.activeEvent = -1;
				for(isize i = 0; i < world->actorCount; ++i) {
					Actor* a = world->actors + i;
					a->selected = 0;
				}
			}
		}

		play.eventSpan += lastDayTimer - play.dayTimer;
		for(isize i = 0; i < play.eventCount; ++i) {
			WorldEvent* e = play.events + i;
			int diff = (lastDayTimer - play.dayTimer);
			if(diff > 1) diff /= 2;
			e->timeout -= diff * ((f32)24 / (f32)(DayTimeInFrames)); 
			//This works out because 
			//DayTimeInFrames = dayTimeInSeconds * framesPerSecond 
			// so, 24 hours in a day / (dayTimeInSeconds * framesPerSecond)
			if(e->timeout <= 0) {
				if(e->defaultAction)
					e->defaultAction(world, e, 0, NULL);
				e->kind = -1;
				if(i == play.activeEvent) {
					play.activeEvent = -1;
					for(isize i = 0; i < world->actorCount; ++i) {
						Actor* a = world->actors + i;
						a->selected = 0;
					}
				}
			}
		}

		for(isize i = 0; i < play.eventCount; ++i) {
			WorldEvent* e = play.events + i;
			if(e->kind == -1) {
				if(play.eventCount == 1) {
					play.eventCount = 0;
				} else if(i - 1 == play.eventCount) {
					--play.eventCount;
				} else {
					play.events[i] = play.events[--play.eventCount];
				}
			}
		}

		if(play.eventCount < 0) play.eventCount = 0;

		{
			wplSprite* s = wplGetSprite(play.group);
			s->x = 8;
			s->y = 8;
			s->w = EventPanelWidth(state->width);
			s->h = 48;
			s->flags = Sprite_NoTexture | Anchor_TopLeft;
			s->color = 0x99;
			f32 minutes = hours - (int)hours;
			minutes *= 60;

			char buf[256];
			snprintf(buf, 256, "Day %d -- Time: %02d:%02d", world->day, (int)hours, (int)minutes);
			drawText(16, 16, buf);

			if(play.activeEvent == -1) {
				f32 y = 26;
				snprintf(buf, 256, "Food: %d", world->resources.food);
				drawText(16, y, buf);
				snprintf(buf, 256, "Tools: %d", world->resources.tools);
				drawTextR(s->w, y, buf);
				y += 8;

				snprintf(buf, 256, "Wood: %d", world->resources.wood);
				drawText(16, y, buf);
				snprintf(buf, 256, "Weapons: %d", world->resources.weapons);
				drawTextR(s->w, y, buf);
				y += 8;

				snprintf(buf, 256, "Population %d", world->actorCount);
				drawText(16, y, buf);
				snprintf(buf, 256, "Artifacts: %d", world->resources.artifacts);
				drawTextR(s->w, y, buf);
				y += 8;
			} else {
				s->h = 24; 
			}
		}
	} else if(play.mode == Mode_EveningReview) {
		play.eventCount = 0;
		if(play.dayTimer < 0 && uiButton(4, 4, "Go to sleep")) {
			play.mode = Mode_MorningAssign;
			play.dayTimer = -1;
			for(isize i = 0; i < world->actorCount; ++i) {
				Actor* a = world->actors + i;
				int lastState = a->state;

				if(a->food < -500) {
					a->state = ActorState_Suffering;
					a->health -= 5;
				} else if(a->food < -150) {
					a->state = ActorState_Hungry;
					a->health--;
				} else {
					if(a->job == ActorJob_None) {
						a->state = ActorState_Idle;
						a->daysConsecutiveWork = 0;
					} else if(a->mood < -1) {
						a->state = ActorState_Moping;
					} else {
						a->state = ActorState_Working;
					}

				}

				if(a->health < -10) {
					play.removed[play.removedCount++] = a;
				}

			}


		}

		if(play.dayTimer < 0) {
			char buf[256];
			f32 y = 18;

			snprintf(buf, 256, "Food: %d", world->resources.food);
			drawText(4, y, buf);
			y += 14;

			if(world->resources.food < world->actorCount) {
				drawText(16, y, "WARNING: You do not have enough food for everyone!");
				y += 14;
				drawText(16, y, "Make sure you set people to gather food in the morning!");
				y += 14;
			}

			snprintf(buf, 256, "Wood: %d", world->resources.wood);
			drawText(4, y, buf);
			y += 14;

			snprintf(buf, 256, "Population %d", world->actorCount);
			drawText(4, y, buf);
			y += 18;

			for(isize i = 0; i < play.deadCount; ++i)  {
				string a = play.deadNames[i];
				snprintf(buf, 256, "%s died", a);
				drawText(4, y, buf);
				y += 10;
			}

			int realCount = 0;
			for(isize i = 0; i < world->actorCount; ++i) {
				if(world->actors[i].name) {
					realCount++;
				}
			}

			world->actorCount = realCount;

		}
	}


	play.actorScroll -= wplGetMouseWheel() * 32;
	if(play.actorScroll < 0) play.actorScroll = 0;
	if(play.actorScroll > (world->actorCount / 4) * 128) 
		play.actorScroll = (world->actorCount / 4) * 128;

	f32 ax = state->width / 4 - ActorCardWidth - 8, ay = 8 - play.actorScroll;
	drawText(ax, ay, "Haven (for LD40) - by William Bundy - williambundy.xyz - @William_Bundy - github.com/WilliamBundy");
	ay += 16;
	for(isize i = 0; i < world->actorCount; ++i) {
		Actor* a = world->actors + i;
		//drawActor(a, state->width / 4 + i * 80, 8);
		if(play.mode == Mode_MorningAssign) {
			if(wplMouseIsJustDown(1)) {
				f32 mx, my;
				getMouse(play.group, &mx, &my);
				if(mx > ax && my > ay && 
						mx < (ax + ActorCardWidth) && 
						my < (ay + ActorCardHeight)) {
					a->job = (a->job + 1) % ActorJobCount;
				}
			}
		} else if(play.activeEvent != -1) {
			if(wplMouseIsJustDown(1)) {
				f32 mx, my;
				getMouse(play.group, &mx, &my);
				if(mx > ax && my > ay && 
						mx < (ax + ActorCardWidth) && 
						my < (ay + ActorCardHeight)) {
					WorldEvent* event = play.events + play.activeEvent;
					int canSelect = 1;
					for(isize i = 0; i < event->involveCount; ++i) {
						if(event->involves[i] == a) {
							canSelect = 0;
							break;
						}
					}
					if(canSelect) {
						if(!a->selected) {
							if(event->peopleSelected < event->peopleToSelectMax) {
								a->selected = 1;
								event->peopleSelected++;
							}
						} else {
							a->selected = 0;
							event->peopleSelected--;
						}

					}
				}

			}

		}
		drawActor(a, ax, ay);
		ax += 80;
		if(ax + 80 > state->width / 2) {
			ax = state->width / 4 - ActorCardWidth - 8;
			ay += 136;
		}
	}

	wplGroupDraw(window, state, play.group);
	textGroup->scale = play.group->scale;
	wplGroupDraw(window, state, textGroup);
}
