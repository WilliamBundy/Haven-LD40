
/* TODO(will)
 * 	- Different event classes with differet colors: green events are 
 * 		good, yellow events are mixed, red events are always negative?
 * 		subsequent events could be blue, special events are purple/yellow?
 * 	- Event tabs need to be differentiated somehow so that it makes more sense
 * 		as they get removed from the list
 * 	- Events events events events! like, half the code is there for them, we 
 * 		need to add a bunch of UI code, like selecting someone to stop working
 * 		and then do the event, then get back to work, but events should be the 
 * 		next big thing for hours 18-24
 * 	- Building and crafting, use as modifiers to resource gathering
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

typedef int (*EventAction)(World* world, WorldEvent* event, int something, void* ud);

struct WorldEvent
{
	string text[16];
	string options[16];
	EventAction defaultAction;
	EventAction actions[16];
	int textCount, optionCount;
	Actor* affects[8];
	Actor* selected[8];
	int involveCountMax, involveCountMin, involveCount;
	f32 timeoutMin, timeoutMax, timeout;
	int peopleToSelectMin, peopleToSelectMax, peopleSelected;
	int minPopNeeded;
	int kind;
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
};

struct Hut
{
	Actor* actors[4];
	int condition;
	int timer;
};

#include "gamedata.c"
struct World 
{
	RandomState randomState;
	RandomState* r;

	Actor actors[256];
	Hut huts[64];

	int actorCount;
	int hutCount;

	struct {
		int population;
		int wood, food;
		int spears, axes, bows, shields;
	} resources;

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

void addDebugEvent()
{
	World* world = play.world;
	if(play.eventCount >= 256) return;
	WorldEvent* event = play.events + play.eventCount++;
	event->timeout = getRandRangeF64(world->r, 0.5, 2);

	event->kind = getRandomRange(world->r, 0, EventKindCount);
	event->text[0] = eventStrings[event->kind * 3];
	event->text[1] = eventStrings[event->kind * 3 + 1];
	event->text[2] = eventStrings[event->kind * 3 + 2];
}

void generateConflictEvent()
{
	World* world = play.world;
	if(play.eventCount >= 256) return;
	WorldEvent* event = play.events + play.eventCount++;
	//Select event randomly by... things
	//For conflict events
	{
		WorldEvent* candidates[256];
		isize candidateCount = 0;
		for(isize i = 0; i < eventTemplateCount; ++i) {
			WorldEvent* t = eventTemplates + i;
			if(t->minPopNeeded > world->actorCount) break;
			candidates[candidateCount++] = t;
		}

		int candidateGroups[ActorJobCount];
		isize groupCount = 0;
		for(isize i = 0; i < ActorJobCount; ++i) {
			if(world->jobCount[i] > 0)  {
				candidateGroups[groupCount++] = i;
			}
		}

		isize chosenGroup = getRandomRange(world->r, 0, groupCount);
		Actor* aggro = NULL;
		f32 highAggroChance = -1;
		for(isize i = 0; i < world->actorCount; ++i) {
			Actor* a = world->actors + i;
			if(a->job != chosenGroup) continue;

			f32 aggroChance = getRandom(world->r) * 0.25;
			if(a->mood < 3) aggroChance *= 3;
			if(a->food < 25) aggroChance *= 2;
			if(a->mood > 5) aggroChance *= 0.5;
			if(a->health > 5) aggroChance *= 0.5;
			if(a->health < 0) aggroChance *= 0;
			for(isize j = 0; j < 4; ++j) {
				aggroChance *= posTraitAggroModifier[a->positiveTraits[j]];
				aggroChance *= negTraitAggroModifier[a->negativeTraits[j]];
			}

			a->aggroChance = aggroChance;
			if(aggroChance > highAggroChance) {
				aggro = a;
				highAggroChance = aggroChance;
			}
		}

		if(aggro == NULL) printf("uuh, something's wrong");

		highAggroChance = -1;
		Actor* responds = NULL;
		for(isize i = 0; i < world->actorCount; ++i) {
			Actor* a = world->actors + i;
			if(a->job != chosenGroup) continue;
			if(a == aggro) continue;

			f32 aggroChance = a->aggroChance;
			for(isize j = 0; j < 4; ++j) {
				for(isize k = 0; k < 4; ++k) {
					if(a->positiveTraits[j] == aggro->negativeTraits[k]) {
						aggroChance *= 3;
					}

					if(a->positiveTraits[j] == aggro->positiveTraits[k]) {
						aggroChance *= 0.5; 
					}

					if(a->negativeTraits[j] == aggro->negativeTraits[k]) {
						aggroChance *= 0.5; 
					}
				}
			}


			if(abs(a->food - aggro->food) > 50) {
				aggroChance *= 1.5;
			}
			if(abs(a->health - aggro->health) > 5) {
				aggroChance *= 1.5;
			}
			if(abs(a->mood - aggro->mood) > 5) {
				aggroChance *= 1.5;
			}
			
			if(aggroChance > highAggroChance) {
				responds = a;
				highAggroChance = aggroChance;
			}

		}

		if(responds == NULL) {
			// do a solo event
		} else {
			// do a conflict event!!!!
		}
	}
	
	event->timeout = getRandRangeF64(world->r, 0.5, 2);

	event->kind = getRandomRange(world->r, 0, EventKindCount);
	event->text[0] = eventStrings[event->kind * 3];
	event->text[1] = eventStrings[event->kind * 3 + 1];
	event->text[2] = eventStrings[event->kind * 3 + 2];
}


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

void addActor(Actor* actor)
{
	if(play.world->actorCount >= 256) return;
	play.world->actors[play.world->actorCount++] = *actor;
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
		snprintf(buf, 256, "Contributed %d %s", actor->contribution,
				jobContribType[actor->contribType]);

		drawText(x + 4, pty+8, buf);
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

	if(!mouseState && !mouseIn) {
		s->color = 0x00000066;
	} else if(mouseIn) {
		s->color = 0x00000099;
		if(mouseState == 1) {
			s->color = 0x000000BB;
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

	f32 height = drawTextSW(s->x + 8, s->y + 8, e->text[1], 1, s->w - 16);
	height += drawTextSW(s->x + 8, s->y + height + 8, e->text[2], 0.5, s->w - 16);

	int eventResolved = 0;
	if(uiButton(s->x + 8, s->y + height + 8, "Yes")) {
		eventResolved = 1;
	}

	if(uiButton(s->x + 8 + 80 , s->y + height + 8, "No")) {
		eventResolved = -1;
	}

	s = wplGetSprite(textGroup);
	s->x = 16;
	s->y = 40 + EventPanelHeight(state->height) - 16;
	s->w = (EventPanelWidth(state->width) - 16) * (1.0 < e->timeout ? 1.0 : e->timeout);
	s->h = 8;
	s->flags = Sprite_NoTexture | Anchor_TopLeft;
	s->color = 0xFFFFFFFF;
	
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
		f32 timep = (f32)play.dayTimer / (f32)DayTimeInFrames + 0.5; 
		int c = 0xFF;
		int a = 0xFF;
		if(timep < 1) {
			c = (int)(timep * 255);
			if(c > 255) c = 255;
		}

		if(timep < 0.75) {
			a = (int)((timep+0.25) * 255);
			if(a > 255) a = 255;
		}

		bgs->color = a << 24 | a << 16 | c << 8 | 0xFF;
	} else {
		if(play.dayTimer < 0) {
			bgs->color = 0x666666FF;
		} else {
			bgs->color = 0;
		}
	}
	wplGroupDraw(window, state, play.group);

	play.group->scale = 2;
	play.group->texture = gameData.basicTex;

	World* world = play.world;

	if(world->actorCount <= 0) {
		drawTextS(64, 64, "Everybody died. You lose.", 2);
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
		}

		if(uiButton(80, 20, "Set everyone to idle")) {
			for(isize i = 0; i < world->actorCount; ++i) {
				Actor* a = world->actors + i;
				a->job = ActorJob_None;
			}
		}
	} else if(play.mode == Mode_DayEvents) {
		int lastDayTimer = play.dayTimer;

		if(uiButton(8, 20, "Waste some time")) {
			play.dayTimer -= DayTimeInFrames / 4;
		}
		play.dayTimer--;

		if(play.dayTimer <= 0) {
			play.mode = Mode_EveningReview;
			play.dayTimer = -1;
			play.activeEvent = -1;
			//TODO(will) at end of day, perform default actions for any pending events
			play.eventCount = 0;
			world->day++;
			//finalize day based on job
			f32 woodGather = 0;
			f32 foodGather = 0;
			f32 craftWork = 0;
			f32 buildWork = 0;
			f32 foodConsumed = 0;
			
			f32 foodMod = 1;
			f32 woodMod = 1;
			//TODO(will): implement something like "actor work modifier"
			//	basically: some actors are better than others at doing work
			//	and this is reflected by a multiplier in what they produce
			//TODO(will): implement wood gather falloff as time goes on.
			//TODO(will): building: farm, takes multi days to build, 
			//	but increases food gather
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
						printf("%s %f", a->name, amt);
						foodGather += amt;
						break;
					case ActorJob_WoodGather:
						amt = 0.9 + getRandom(world->r) * 0.25;
						amt *= workAmt * woodMod;
						woodGather += amt;
						break;
					case ActorJob_Crafting:
						amt = 1 + getRandom(world->r) * 0.1;
						amt *= workAmt;
						craftWork += amt;
						break;
					case ActorJob_Building:
						amt = 1 + getRandom(world->r) * 0.1;
						amt *= workAmt;
						craftWork += amt;
						break;
				}
				a->contribution = (int)(amt);
				a->contribType = a->job;
			}

			f32 foodAvailable = world->resources.food + foodGather;
			f32 foodEaten = foodAvailable < foodConsumed ? 
				foodAvailable : foodConsumed;
			foodAvailable -= foodEaten;
			
			world->resources.food = foodAvailable;
			world->resources.wood += (int)woodGather;

			for(isize i = 0; i < world->actorCount; ++i) {
				Actor* a = world->actors + i;
				a->food += 100 * (int)(foodEaten / world->actorCount);
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

		play.eventSpan += lastDayTimer - play.dayTimer;
		while(play.eventSpan > play.nextEventTime) {
			play.eventSpan -= play.nextEventTime;
			if(play.eventSpan <= 0) play.eventSpan = 0;
			play.nextEventTime = getRandomRange(world->r, 
					DayTimeInFrames / 48, DayTimeInFrames / 20);
			//trigger event
			addDebugEvent();
		}

		f32 timep = (f32)play.dayTimer / (f32)DayTimeInFrames; 
		f32 hours = (1-timep) * 24;
		if(play.activeEvent == -1) {
			f32 eventX = 8, eventY = 32;
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
				}
			}
		} else {
			if(uiButton(80, 20, "Close")) {
				play.activeEvent = -1;
			}

			int resolved = drawEventPanel(play.events + play.activeEvent, state);
			if(resolved) {
				play.events[play.activeEvent] = play.events[--play.eventCount];
				play.activeEvent = -1;
			}
		}

		for(isize i = 0; i < play.eventCount; ++i) {
			WorldEvent* e = play.events + i;
			e->timeout -= (24 / (f32)(DayTimeInFrames)); 
			//This works out because 
			//DayTimeInFrames = dayTimeInSeconds * framesPerSecond 
			// so, 24 hours in a day / (dayTimeInSeconds * framesPerSecond)
			if(e->timeout <= 0) {
				e->kind = -1;
				if(i == play.activeEvent) play.activeEvent = -1;
			}
		}

		for(isize i = play.eventCount; i >= 0; i--) {
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
			f32 minutes = hours - (int)hours;
			minutes *= 60;

			char buf[256];
			snprintf(buf, 256, "Time: %02d:%02d", (int)hours, (int)minutes);

			drawText(8, 8, buf);
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
