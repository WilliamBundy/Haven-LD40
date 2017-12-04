
#define CreateEventAction(name) int name(World* world, WorldEvent* event, int something, void* ud)

void eventResult(WorldEvent* e, string res)
{
	e->resultLines[e->resultCount++] = res;
}

CreateEventAction(eventDoNothing)
{
	event->resultText = event->negativeResultText;
	return 0;
}

CreateEventAction(eventRaidFight)
{
	event->resultText = "You fight for your lives!";
	eventResult(event, "You chase off the raiders");
	eventResult(event, "but some people got hurt in the battle");
	isize damage = getRandomRange(world->r, 4, world->actorCount / 3);
	for(isize i = 0; i < world->actorCount / 3; ++i) {
		world->actors[getRandomRange(world->r, 0, world->actorCount)].health--;
	}

	return 0;
}

CreateEventAction(eventRaidFightBad)
{
	event->resultText = "You fight for your lives!";
	eventResult(event, "You don't have enough weapons for everyone though");
	eventResult(event, "A lot of people got hurt though");
	isize damage = getRandomRange(world->r, world->actorCount / 3, world->actorCount);
	for(isize i = 0; i < world->actorCount / 3; ++i) {
		world->actors[getRandomRange(world->r, 0, world->actorCount)].health--;
	}

	return 0;
}


CreateEventAction(eventFoodRotSmall)
{
	event->resultText = "You throw out the rotten stuff just in time";
	eventResult(event, "You only had to throw out %d food");

	int r = getRandomRange(world->r, 3, 6);
	event->resultValues[0] = r;
	world->resources.food -= r;
	if(world->resources.food < 0) world->resources.food = 0;
	return 0;
}

CreateEventAction(eventFoodRotBig)
{
	event->resultText = "So much had gotten rotten, it was awful";
	eventResult(event, "You only had to throw out %d food");
	int r = getRandomRange(world->r, 5, 12);
	event->resultValues[0] = r;
	world->resources.food -= r;
	if(world->resources.food < 0) world->resources.food = 0;
	return 0;
}

CreateEventAction(eventBadTimber)
{

	event->resultText = "It's all gnarled and twisted!";
	eventResult(event, "Turns out you had %d useless wood");
	int r = getRandomRange(world->r, 1, 3);
	event->resultValues[0] = r;
	world->resources.wood -= r;
	if(world->resources.wood < 0) world->resources.wood = 0;
	return 0;
}

CreateEventAction(eventRaidRun)
{
	event->resultText = "Hiding in the trees, the bandits ransack the haven";
	event->resultValues[0] = world->resources.food / 2;
	event->resultValues[1] = world->resources.wood / 2;
	event->resultValues[2] = world->resources.tools / 3;

	world->resources.food -= event->resultValues[0];
	world->resources.wood -= event->resultValues[1];
	world->resources.tools -= event->resultValues[2];

	eventResult(event, "Everyone is safe, but");
	eventResult(event, "%d food was stolen");
	eventResult(event, "%d wood was stolen");
	eventResult(event, "%d tools were stolen");
	eventResult(event, "and a bunch of people got hurt in the confusion");

	for(isize i = 0; i < world->actorCount / 4; ++i) {
		world->actors[getRandomRange(world->r, 0, world->actorCount)].health--;
	}

	return 0;
}

CreateEventAction(eventWelcomeTravellers)
{
	event->resultText = "Thanking you, they join the haven";
	if(world->day < 5) event->involveCount = 1;
	for(isize i = 0; i < event->involveCount; ++i) {
		if(world->actorCount >= world->buildings.huts * 6) {
			event->resultText = "They can't find a home.";
			eventResult(event, "Disappointed, they leave.");
			eventResult(event, "You need to build more huts before people can join");
			break;
		}
		Actor* a = addActor(generateActor());
		arenaPop(tempArena);

		event->involves[i] = a;
	}
	return 0;
}

CreateEventAction(eventSellFoodForTools)
{
	event->resultText = "The trader smiles, happy with his purchase";
	eventResult(event, "He sold you %d tools");
	world->resources.food -= event->optionReqs[0].amt;
	int out = event->optionReqs[0].amt / 10 + 1;
	int r = getRandomRange(world->r, 
			out * 0.75, out * 1.5) + 1;
	world->resources.tools += r;

	event->resultValues[0] = r;

	return 0;
}

CreateEventAction(eventSellWoodForTools)
{
	event->resultText = "The trader smiles, happy with his purchase";
	eventResult(event, "He sold you %d tools");
	world->resources.wood -= event->optionReqs[0].amt;
	int out = event->optionReqs[0].amt / 8 + 1;
	int r = getRandomRange(world->r, 
			out * 0.75, out * 1.5) + 1;
	world->resources.tools += r;
	event->resultValues[0] = r;
	return 0;
}

CreateEventAction(eventSellFoodForWeapons)
{
	event->resultText = "The trader smiles, happy with his purchase";
	eventResult(event, "He sold you %d weapons");
	world->resources.food -= event->optionReqs[0].amt;
	int out = event->optionReqs[0].amt / 10 + 1;
	int r = getRandomRange(world->r, 
			out * 0.75, out * 1.5) + 1;
	world->resources.weapons += r;
	event->resultValues[0] = r;
	return 0;
}

CreateEventAction(eventSellWoodForWeapons)
{
	event->resultText = "The trader smiles, happy with his purchase";
	eventResult(event, "He sold you %d weapons");
	world->resources.wood -= event->optionReqs[0].amt;
	int out = event->optionReqs[0].amt / 8 + 1;
	int r = getRandomRange(world->r, 
			out * 0.75, out * 1.5) + 1;
	world->resources.weapons += r;
	event->resultValues[0] = r;
	return 0;
}

CreateEventAction(eventSellFoodForWood)
{
	event->resultText = "The trader smiles, happy with his purchase";
	eventResult(event, "He sold you %d wood");
	world->resources.food -= event->optionReqs[0].amt;
	int out = event->optionReqs[0].amt / 2 + 1;
	int r = getRandomRange(world->r, 
			out * 0.75, out * 1.5) + 1;
	world->resources.wood += r;
	event->resultValues[0] = r;
	return 0;
}

CreateEventAction(eventSellWoodForFood)
{
	event->resultText = "The trader smiles, happy with his purchase";
	eventResult(event, "He sold you %d food");
	world->resources.wood -= event->optionReqs[0].amt;
	int out = event->optionReqs[0].amt * 3 / 2 + 1;
	int r = getRandomRange(world->r, 
			out * 0.75, out * 1.5) + 1;
	world->resources.food += r;
	event->resultValues[0] = r;
	return 0;
}

CreateEventAction(eventSellToolsForStuff)
{
	event->resultText = "The trader smiles, happy with his purchase";
	eventResult(event, "He sold you %d food");
	eventResult(event, "and %d wood");
	world->resources.tools -= event->optionReqs[0].amt;
	int out = event->optionReqs[0].amt * 10;
	int out2 = event->optionReqs[0].amt * 8;
	int r = getRandomRange(world->r, 
			out * 0.75, out * 1.5) + 1;
	world->resources.food += r;
	event->resultValues[0] = r;
	r = getRandomRange(world->r, 
			out2 * 0.75, out * 1.5) + 1;
	world->resources.wood += r;
	event->resultValues[1] = r;

	return 0;
}

CreateEventAction(eventSellWeaponsForStuff)
{
	event->resultText = "The trader smiles, happy with his purchase";
	eventResult(event, "He sold you %d food");
	eventResult(event, "and %d wood");
	world->resources.weapons -= event->optionReqs[0].amt;
	int out = event->optionReqs[0].amt * 20;
	int out2 = event->optionReqs[0].amt * 16;
	int r = getRandomRange(world->r, 
			out * 0.75, out * 1.5) + 1;
	world->resources.food += r;
	event->resultValues[0] = r;
	r = getRandomRange(world->r, 
			out2 * 0.75, out * 1.5) + 1;
	world->resources.wood += r;
	event->resultValues[1] = r;
	return 0;
}

CreateEventAction(eventIgnoreHurtGatherers)
{
	event->resultText = "%s got hurt and needs to rest";
	if(event->involves[0]) {
		i32 mod = 1;
		for(isize i = 0; i < 4; ++i) {
			if(event->involves[0]->positiveTraits[i] == Trait_Tough) {
				mod = 2;
			}
		}
		isize r = getRandomRange(world->r, 2, 4) / mod;
		event->involves[0]->health -= r;
		event->involves[0]->job = ActorJob_None;

		eventResult(event, "Their injury cost %d hp");
		event->resultValues[0] = r;
	}
	return 0;
}

CreateEventAction(eventSendSomeoneToHelpGatherers)
{
	event->resultText = "They get to %s in time";
	if(event->involves[0]) {
		event->involves[0]->dailyWorkMod = 0.75;
		eventResult(event, "They aren't hurt, but the incident took some time");
	}
	return 0;
}

CreateEventAction(eventCraftDamage)
{
	isize r = getRandomRange(world->r, 1, 3);
	world->craftWorkNeeded += r;
	event->resultValues[0] = r;
	eventResult(event, "It set you back by %d work");
	return 0;
}

CreateEventAction(eventBuildDamage)
{
	isize r = getRandomRange(world->r, 2, 5);
	world->buildWorkNeeded += r;
	event->resultValues[0] = r;
	eventResult(event, "It set you back by %d work");
	return 0;
}


CreateEventAction(eventExtraHungryEat)
{
	if(event->involves[0]) {
		if(world->resources.food > 1) {
			event->resultText =  "%s ate some food, and felt better";
			event->involves[0]->food += 100;
			world->resources.food--;
			event->resultValues[0] = 100;
		}
	}
	return 0;
}

CreateEventAction(eventExtraHungryNoEat)
{
	if(event->involves[0]) {
		event->resultText ="%s is unhappy being hungry";
		eventResult(event, "*stomach grumbling sounds*");
		event->involves[0]->food -= 50;
		event->involves[0]->mood--;
		event->resultValues[0] = 50;
	}
	return 0;
}

CreateEventAction(eventWakeUpNapper)
{
	event->resultText ="%s is unhappy about being woken up";
	if(event->involves[0]) {
		event->involves[0]->mood--;
		event->involves[0]->mood--;
	}
	return 0;
}

CreateEventAction(eventLetThemSleep)
{
	event->resultText ="%s happily sleeps";
	if(event->involves[0]) {
		event->involves[0]->mood += 2;
		event->involves[0]->job = ActorJob_None;
		event->resultValues[0] = 2;
	}
	return 0;
}

CreateEventAction(eventFreeFood)
{
	if(world->day > 10) {
		event->resultText = "Oh...";
		eventResult(event, "It's rotten...");
	}
	isize r = getRandomRange(world->r, world->actorCount/2, world->actorCount * 1.5);
	if(r == 0) r = 1;
	world->resources.food += r;
	event->resultText = "Eagerly, you gather the food";
	eventResult(event, "You found %d food");
	event->resultValues[0] = r;
	return 0;
}

CreateEventAction(eventFreeWood)
{
	int res = getRandomRange(world->r, 1 * world->day, 3 * world->day);
	if(res > 10) res = 10;
	if(res == 0) res = 1;
	world->resources.food += res;
	event->resultValues[0] = res;

	event->resultText = "Eagerly, you gather the wood";
	eventResult(event, "You found %d wood");
	return 0;
}


CreateEventAction(eventArgumentDefault)
{
	if(event->involves[0] && event->involves[1]) {
		event->involves[0]->mood -= 2;
		event->involves[1]->mood -= 2;

		event->resultText = "They fight it out, both of them are upset";
		eventResult(event, "Their moods got worse by %d points");
		event->resultValues[0] = 2;
		event->resultValues[1] = 2;
	}
	return 0;
}

CreateEventAction(eventSideWithA)
{
	if(event->involves[0] && event->involves[1]) {
		event->involves[1]->mood -= 3;;
		event->resultValues[0] = 3;
		event->resultText = "By supporting one, they feel better";
		eventResult(event, "but the other one is more upset than before");
		eventResult(event, "Their mood got worse by %d points");
	}
	return 0;
}

CreateEventAction(eventSideWithB)
{
	if(event->involves[0] && event->involves[1]) {
		event->involves[0]->mood -= 3;
		event->resultValues[0] = 3;
		event->resultText = "By supporting one, they feel better";
		eventResult(event, "but the other one is more upset than before");
		eventResult(event, "Their mood got worse by %d points");
	}
	return 0;
}

CreateEventAction(eventSeparateArgument)
{
	if(event->involves[0] && event->involves[1]) {
		event->involves[0]->mood--;
		event->involves[1]->mood--;
		event->resultValues[0] = 1;
		event->resultValues[1] = 1;
		event->resultText = "You separate them";
		eventResult(event, "they have some time to cool off, but they");
		eventResult(event, "still lost %d mood each");
	}
	return 0;
}

CreateEventAction(eventBreakUpFight)
{
	if(event->involves[0] && event->involves[1]) {
		event->involves[0]->health--;
		event->involves[1]->health--;
		event->resultValues[0] = 1;
		event->resultValues[1] = 1;
		event->resultText = "You manage to break up the fight";
		eventResult(event, "it seems you prevented any real damage");
		eventResult(event, "they still lost %d health each");
	}
	return 0;
}

CreateEventAction(eventLetFightFinish)
{
	if(event->involves[0] && event->involves[1]) {
		f32 p1dmg = 1, p2dmg = 1;
		for(isize i = 0; i < 4; ++i) {
			p1dmg *= posTraitDmgMod[event->involves[0]->positiveTraits[i]];
			p1dmg *= negTraitDmgMod[event->involves[0]->negativeTraits[i]];
			p2dmg *= posTraitDmgMod[event->involves[1]->positiveTraits[i]];
			p2dmg *= negTraitDmgMod[event->involves[1]->negativeTraits[i]];
		}

		int d1 = getRandomRange(
				world->r, 1, 4
				) * p2dmg + 1;
		int d2 = getRandomRange(
				world->r, 1, 4
				) * p1dmg + 1;
		event->involves[0]->health -= d1;
		event->involves[1]->health -= d2;
		event->resultValues[0] = d1;
		event->resultValues[1] = d2;
		event->resultText = "You watch them fight it out";
		eventResult(event, "one gets hurt for %d hp");
		eventResult(event, "the other for %d");

	}
	return 0;
}

CreateEventAction(eventInsulted)
{
	if(event->involves[0] && event->involves[1]) {
		event->involves[1]->mood--;
		event->resultValues[0] = 1;
	}
	return 0;
}

CreateEventAction(eventFeltHappy)
{
	if(event->involves[0]) {
		event->resultText = "%s felt good today";
		eventResult(event, "They just wanted to tell you.");
		event->involves[0]->mood++;
	}
	return 0;
}

CreateEventAction(eventFeltRelaxed)
{
	if(event->involves[0]) {
		event->resultText = "%s felt good today";
		eventResult(event, "They just wanted to tell you.");
		event->involves[0]->daysConsecutiveWork = 0;
	}
	return 0;
}

CreateEventAction(eventRecoveredQuickly)
{
	if(event->involves[0]) {
		event->resultText = "%s felt energized";
		eventResult(event, "They just wanted to tell you.");
		event->involves[0]->health++;
	}
	return 0;
}


CreateEventAction(eventBuyArtifact)
{
	event->resultText = "Smiling, she hands over the artifact";
	world->resources.artifacts++;
	return 0;
}

CreateEventAction(eventInvestigateCave)
{
	event->resultText = "Inside, you find an artifact!";
	world->resources.artifacts++;
	return 0;
}

CreateEventAction(eventFightMachine)
{
	event->resultText = "The behemoth crumbles to the touch, revealing a glistening core";
	world->resources.artifacts++;
	return 0;
}

#define EventStart(name, involvedCountMin, involvedCountMax) { WorldEvent* e = getNextEvent(events, eventCount); \
	e->text[e->textCount++] = name;\
	e->kind = i++; \
	e->involveCountMin = involvedCountMin; \
	e->involveCountMax = involvedCountMax; \
	e->minPopNeeded = minPopNeeded;\
	e->optionRequiresSelection = -1;\
	e->jobSpecific = -1; \
	e->defaultAction = eventDoNothing;
#define EventKind(k) e->kind = Event_##k;
#define EventSetTimeout(mn, mx) e->timeoutMin = mn; e->timeoutMax = mx;
#define EventSelectCount(mn, mx) e->peopleToSelectMin = mn; e->peopleToSelectMax = mx;
#define EventString(str) e->text[e->textCount++] = str;
#define EventOption(str, result) e->options[e->optionCount] = str; e->actions[e->optionCount++] = result;
#define EventOptionReqSelected e->optionRequiresSelection = e->optionCount - 1;
#define EventOptionReq(res, mn, mx) \
	e->optionReqs[e->optionCount-1].hasReq = 1;\
	e->optionReqs[e->optionCount-1].resource = res;\
	e->optionReqs[e->optionCount-1].amtMin = mn;\
	e->optionReqs[e->optionCount-1].amtMax = mx;
#define EventDefaultOption(result) e->defaultAction = result;
#define EventNegResult(r) e->negativeResultText = r;
#define EventResult(r) eventResult(e, r);
#define EventJobSpecific(job) e->jobSpecific = job;
#define EventEnd }

WorldEvent* getNextEvent(WorldEvent* events, isize* eventCount)
{
	isize ec = *eventCount;
	WorldEvent* e = events + ec;
	*eventCount = ec + 1;
	memset(e, 0, sizeof(WorldEvent));
	return e;
}

WorldEvent eventTemplates[256];
isize eventTemplateCount = 0;
void createEventTemplates(WorldEvent* events, isize* eventCount)
{
	int i = 0;
	int minPopNeeded = 0;
	//Multiple versions: wealthy travellers, poor travellers, big group of etc
	EventStart("Travellers want to join", 1, 3)
		EventSetTimeout(1, 3)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventNegResult("They nod and continue walking down the old road.")
		EventString("Some wandering travellers have come across your haven, "
				"and would like to join your group.")
		EventOption("Welcome them", eventWelcomeTravellers)
		EventOption("Turn them away", eventDoNothing)
	EventEnd;

	EventStart("A mysterious trader visits", 0, 0)
		EventSetTimeout(24, 24)
		EventKind(Special)
		EventDefaultOption(eventDoNothing)
		EventNegResult("The old woman says she will return in time.")
		EventString("An ancient woman says she has something of great value")
		EventString("and that she is willing to part with it, for a price")
		EventOption("You must buy the artifact.", eventBuyArtifact)
		EventOptionReq(Resource_Tools, 25, 25)
		EventOption("You cannot afford it right now", eventDoNothing)
	EventEnd;

	EventStart("You discover a mysterious cave", 0, 0)
		EventSetTimeout(24, 24)
		EventKind(Special)
		EventDefaultOption(eventDoNothing)
		EventNegResult("Whatever the secrets of the cave hold, they are not yours today.")
		EventString("Your gatherers discover a mysterious cave; it calls to them with promises of riches")
		EventOption("Send them to investigate", eventInvestigateCave)
		EventJobSpecific(ActorJob_FoodGather)
		EventOptionReqSelected
		EventSelectCount(10, 10)
		EventOption("There are not enough of us, it is too dangerous", eventDoNothing)
	EventEnd;

	EventStart("A giant machine approaches", 0, 0)
		EventSetTimeout(24, 24)
		EventKind(Special)
		EventDefaultOption(eventDoNothing)
		EventNegResult("The machine walks on")
		EventString("You have seen nothing like it before")
		EventString("It spurts fire and shoots arrows at those who come near")
		EventString("Could this be the final artifact?")
		EventOption("Send people to investigate?", eventFightMachine)
		EventOptionReq(Resource_Weapons, 10, 10)
		EventOptionReqSelected
		EventSelectCount(10, 10)
		EventOption("We do not have enough weaponry to fight the machine. It shall have to wait another day.", eventDoNothing)
	EventEnd;

	EventStart("Raid incoming!", 1, 1)
		EventSetTimeout(24, 24)
		EventKind(Raid)
		EventDefaultOption(eventRaidRun)
		EventString("Bandits are coming for your haven! Will you defend or flee?")
		EventOption("Defend", eventRaidFight)
		EventOptionReq(Resource_Weapons, 5, 20)
		EventOption("Defend with what we have", eventRaidFightBad)
		EventOption("Flee", eventRaidRun);
	EventEnd;


	EventStart("Someone felt happy today", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Solo)
		EventOption("Uh, okay?", eventFeltHappy);
	EventEnd;

	EventStart("Someone felt relaxed today", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Solo)
		EventOption("Uh, okay?", eventFeltRelaxed);
	EventEnd;

	EventStart("Someone recovered quickly", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Solo)
		EventOption("Uh, okay?", eventRecoveredQuickly);
	EventEnd;

	EventStart("Someone was extra hungry", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Solo)
		EventOption("Let %s eat more", eventExtraHungryEat);
		EventOption("Deny them the food they want", eventExtraHungryNoEat);
	EventEnd;

	EventStart("Someone decided to take a nap", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Solo)
		EventOption("Wake %s up! No slacking allowed!", eventWakeUpNapper);
		EventOption("Let them sleep; they could do with the rest", eventLetThemSleep);
	EventEnd;

	EventStart("You found... something", 0, 0)
		EventSetTimeout(1, 2)
		EventKind(Gift)
		EventString("...but you don't want it.")
		EventOption("Ignore it", eventDoNothing);
		EventDefaultOption(eventDoNothing);
	EventEnd;

	EventStart("You found some food", 0, 0)
		EventSetTimeout(1, 2)
		EventKind(Gift)
		EventOption("Nice!", eventFreeFood);
		EventOption("Ignore it", eventDoNothing);
		EventDefaultOption(eventDoNothing);
	EventEnd;

	EventStart("You found some wood", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Gift)
		EventOption("Nice!", eventFreeWood);
		EventOption("Ignore it", eventDoNothing);
		EventDefaultOption(eventDoNothing);
	EventEnd;

	EventStart("You found some wood", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Gift)
		EventOption("Nice!", eventFreeWood);
		EventOption("Ignore it", eventDoNothing);
		EventDefaultOption(eventDoNothing);
	EventEnd;

	EventStart("A stranger passes through", 1, 1)
		EventSetTimeout(3, 4)
		EventKind(Outsider)
		EventString("An old man visits your haven")
		EventString("He says he'll tell you a story, if you listen")
		EventNegResult("His story is passed down from generations")
		EventResult("He says this is not the original home of mankind")
		EventResult("and that we're all descended from a small group of settlers")
		EventOption("Listen to what he has to say", eventDoNothing)
	EventEnd;

	EventStart("A stranger passes through", 1, 1)
		EventSetTimeout(3, 4)
		EventKind(Outsider)
		EventString("An old man visits your haven")
		EventString("He says he'll tell you a story, if you listen")
		EventNegResult("His story is about adventure and hardship")
		EventResult("He mentions that healthy people get more done")
		EventResult("He beats his chest, talking about his exploits in the past")
		EventOption("Listen to what he has to say", eventDoNothing)
	EventEnd;

	EventStart("A stranger passes through", 1, 1)
		EventSetTimeout(3, 4)
		EventKind(Outsider)
		EventString("An old man visits your haven")
		EventString("He says he'll tell you a story, if you listen")
		EventNegResult("His story is about youth")
		EventResult("He rambles, but mentions that people become unhappy if they have to work all the time")
		EventResult("He tells you that, if you let them rest, idly, they'll be more productive")
		EventResult("You thank him, and he trots off")
		EventOption("Listen to what he has to say", eventDoNothing)
	EventEnd;


	minPopNeeded = 4;
	EventStart("A stranger passes through", 1, 1)
		EventSetTimeout(3, 4)
		EventKind(Outsider)
		EventString("An old man visits your haven")
		EventString("He says he'll tell you a story, if you listen")
		EventNegResult("His story is advice")
		EventResult("He talks of a boy who built a tool")
		EventResult("to build a better tool, and a better tool, and so on")
		EventResult("Eventually the boy could build anything, really fast")
		EventResult("The old man smiles and leaves")
		EventOption("Listen to what he has to say", eventDoNothing)
	EventEnd;

	EventStart("A stranger passes through", 1, 1)
		EventSetTimeout(3, 4)
		EventKind(Outsider)
		EventString("An old man visits your haven")
		EventString("He says he'll tell you a story, if you listen")
		EventNegResult("His story is about the origin of the traders")
		EventResult("Or rather, the lack of them.")
		EventResult("He seems to think from his travels")
		EventResult("that the traders seem to trade at an almost-fixed rate")
		EventResult("The old man smiles and leaves")
		EventOption("Listen to what he has to say", eventDoNothing)
	EventEnd;

	EventStart("Traders visit, selling wood", 1, 1)
		EventSetTimeout(3, 6)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventNegResult("Disappointed, they shake their head and walk away")
		EventString("A few traders have arrived! They are willing to sell wood for food")
		EventOption("Sell!", eventSellFoodForWood)
		EventOptionReq(Resource_Food, 5, 20)
		EventOption("Decline their offer", eventDoNothing)
	EventEnd;

	EventStart("Traders visit, selling food ", 1, 1)
		EventSetTimeout(3, 6)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventNegResult("Disappointed, they shake their head and walk away")
		EventString("A few traders have arrived! They are willing to sell food for wood")
		EventOption("Sell!", eventSellWoodForFood)
		EventOptionReq(Resource_Wood, 2, 10)
		EventOption("Decline their offer", eventDoNothing)
	EventEnd;

	EventStart("Traders visit, selling wood", 1, 1)
		EventSetTimeout(3, 6)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventNegResult("Disappointed, they shake their head and walk away")
		EventString("A few traders have arrived! They are willing to sell wood for food")
		EventOption("Sell!", eventSellFoodForWood)
		EventOptionReq(Resource_Food, 5, 20)
		EventOption("Decline their offer", eventDoNothing)
	EventEnd;

	EventStart("Traders visit, selling food ", 1, 1)
		EventSetTimeout(3, 6)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventNegResult("Disappointed, they shake their head and walk away")
		EventString("A few traders have arrived! They are willing to sell food for wood")
		EventOption("Sell!", eventSellWoodForFood)
		EventOptionReq(Resource_Wood, 2, 10)
		EventOption("Decline their offer", eventDoNothing)
	EventEnd;

	EventStart("Traders visit, selling wood", 1, 1)
		EventSetTimeout(3, 6)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventNegResult("Disappointed, they shake their head and walk away")
		EventString("A few traders have arrived! They are willing to sell wood for food")
		EventOption("Sell!", eventSellFoodForWood)
		EventOptionReq(Resource_Food, 5, 20)
		EventOption("Decline their offer", eventDoNothing)
	EventEnd;

	EventStart("Traders visit, selling food ", 1, 1)
		EventSetTimeout(3, 6)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventNegResult("Disappointed, they shake their head and walk away")
		EventString("A few traders have arrived! They are willing to sell food for wood")
		EventOption("Sell!", eventSellWoodForFood)
		EventOptionReq(Resource_Wood, 2, 10)
		EventOption("Decline their offer", eventDoNothing)
	EventEnd;

	EventStart("Someone fell over!", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Accident)
		EventDefaultOption(eventDoNothing)
		EventNegResult("But they're okay!")
		EventOption("Go to help them up", eventDoNothing);
	EventEnd;

	EventStart("Someone burped really loud!", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Accident)
		EventDefaultOption(eventDoNothing)
		EventNegResult("%s thinks it was funny")
		EventOption("That was startling", eventDoNothing);
	EventEnd;

	EventStart("Some food started to rot", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Accident)
		EventDefaultOption(eventFoodRotBig)
		EventNegResult("Gross")
		EventOption("Quick! Throw it out!", eventFoodRotSmall);
		EventOption("Gross, I'm not touching that", eventFoodRotBig);
	EventEnd;

	EventStart("Upon inspection...", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Accident)
		EventDefaultOption(eventBadTimber)
		EventString("This wasn't a very good tree; we can't use the wood")
		EventNegResult("Could have been worse...")
		EventOption("Well, let's get rid of it then", eventBadTimber);
	EventEnd;

	EventStart("Somone made a mistake while crafting", 1, 3)
		EventSetTimeout(1, 2)
		EventKind(Accident)
		EventDefaultOption(eventCraftDamage)
		EventJobSpecific(ActorJob_Building)
		EventString("It'll set the project back a little bit")
		EventOption("Oh well", eventCraftDamage);
	EventEnd;

	EventStart("Somone made a mistake while building", 1, 3)
		EventSetTimeout(1, 2)
		EventKind(Accident)
		EventDefaultOption(eventBuildDamage)
		EventJobSpecific(ActorJob_Building)
		EventString("It'll set the project back a little bit")
		EventOption("Oh well", eventBuildDamage);
	EventEnd;

	EventStart("Some people were hurt while cutting wood", 1, 3)
		EventSetTimeout(1, 2)
		EventKind(Accident)
		EventDefaultOption(eventIgnoreHurtGatherers)
		EventJobSpecific(ActorJob_WoodGather)
		EventSelectCount(1, 1)
		EventString("%s hurt themselves while cutting wood")
		EventString("Can you send someone to help?")
		EventOption("Send Someone", eventSendSomeoneToHelpGatherers);
		EventOptionReqSelected;
		EventOption("Hope they make it", eventIgnoreHurtGatherers);
	EventEnd;

	EventStart("Some people were hurt while gathering food", 1, 3)
		EventSetTimeout(1, 2)
		EventKind(Accident)
		EventDefaultOption(eventIgnoreHurtGatherers)
		EventJobSpecific(ActorJob_FoodGather)
		EventSelectCount(1, 1)
		EventString("%s hurt themselves while foraging")
		EventString("Can you send someone to help?")
		EventOption("Send Someone", eventSendSomeoneToHelpGatherers);
		EventOptionReqSelected;
		EventOption("Hope they make it", eventIgnoreHurtGatherers);
	EventEnd;

	EventStart("It seems like...", 2, 2)
		EventKind(Conflict)
		EventString("some people don't get along too well")
		EventNegResult("They'll have to learn to deal with it")
		EventOption("Such is life", eventInsulted)
	EventEnd;

	EventStart("It seems like...", 2, 2)
		EventKind(Conflict)
		EventString("some people don't get along too well")
		EventNegResult("They'll have to learn to deal with it")
		EventOption("Such is life", eventInsulted)
	EventEnd;


	EventStart("Someone insulted someone!", 2, 2)
		EventKind(Conflict)
		EventString("%s was insulted!")
		EventNegResult("They'll get over it in time")
		EventOption("Nothing you can do about it now", eventInsulted)
	EventEnd;

	EventStart("Some poeple got into an argument", 2, 2)
		EventKind(Conflict)
		EventSetTimeout(1, 2)
		EventDefaultOption(eventArgumentDefault)
		EventString("Two people started arguing over a difference of opinion")
		EventOption("Take sides with %s", eventSideWithA)
		EventOption("Take sides with %s", eventSideWithB)
		EventOption("Separate the two", eventSeparateArgument);
	EventEnd;

	minPopNeeded = 7;
	EventStart("Some people started fighting", 2, 2)
		EventKind(Conflict)
		EventSetTimeout(1, 2)
		EventDefaultOption(eventLetFightFinish)
		EventString("%s started a fight with")
		EventString("%s!")
		EventOption("Break up the fight", eventBreakUpFight);
		EventOption("Let them fight it out", eventLetFightFinish);
	EventEnd;
	
	//Trader block
	minPopNeeded = 10;
	EventStart("A stranger passes through", 1, 1)
		EventSetTimeout(3, 4)
		EventKind(Outsider)
		EventString("An old man visits your haven")
		EventString("He says he'll tell you a story, if you listen")
		EventNegResult("His story is a warning")
		EventResult("He says that bandits are afoot, and that you ")
		EventResult("must have weapons to defend yourself")
		EventResult("Otherwise, he warns, much may be lost in the disarray")
		EventOption("Listen to what he has to say", eventDoNothing)
	EventEnd;

	EventStart("A stranger passes through", 1, 1)
		EventSetTimeout(3, 4)
		EventKind(Outsider)
		EventString("An old man visits your haven")
		EventString("He says he'll tell you a story, if you listen")
		EventNegResult("His story is passed down from generations")
		EventResult("He says that somewhere out there is the planet Earth")
		EventResult("Where humans all come from originally")
		EventResult("but something awful happened, so they had to leave")
		EventOption("Listen to what he has to say", eventDoNothing)
	EventEnd;

	EventStart("A stranger passes through", 1, 1)
		EventSetTimeout(3, 4)
		EventKind(Outsider)
		EventString("An old man visits your haven")
		EventString("He starts coughing, violently")
		EventNegResult("*cough* FATE *cough*")
		EventResult("Between his coughs, he keeps saying 'fate!' 'faaate!'")
		EventResult("He says he saw it, swirling in the sky")
		EventResult("A secret message")
		EventResult("Eventually, his coughs die down, and he falls asleep on the spot")
		EventResult("By morning, he is nowhere to be found")
		EventOption("Try to help him", eventDoNothing)
	EventEnd;

	EventStart("A stranger passes through", 1, 1)
		EventSetTimeout(3, 4)
		EventKind(Outsider)
		EventString("An old man visits your haven")
		EventString("He says he'll tell you a story, if you listen")
		EventNegResult("He talks about business")
		EventResult("He said he was a doctor before he was a traveler")
		EventResult("It turns out people who are suffering can't work")
		EventResult("And tend to die pretty quickly")
		EventResult("But happy people with full bellies get stronger every day!")
		EventResult("He thanks you for your time and walks away")
		EventOption("Listen to what he has to say", eventDoNothing)
	EventEnd;

	EventStart("A stranger passes through", 1, 1)
		EventSetTimeout(3, 4)
		EventKind(Outsider)
		EventString("An old man visits your haven")
		EventString("He says he'll tell you a story, if you listen")
		EventNegResult("His story is about his father")
		EventResult("He tells of mysterious machines that walk the earth, in search of ancient rarities")
		EventResult("You don't know how true it is, but you thank him, and he goes on his way")
		EventOption("Listen to what he has to say", eventDoNothing)
	EventEnd;

	EventStart("A stranger passes through", 1, 1)
		EventSetTimeout(3, 4)
		EventKind(Outsider)
		EventString("An old man visits your haven")
		EventString("He says he'll tell you a story, if you listen")
		EventNegResult("His story is about adventure")
		EventResult("He was an explorer in his youth.")
		EventResult("He tells of a cave where an ancient treasure resides.")
		EventResult("If you search long enough, with enough people, you'll find it, he says.")
		EventOption("Listen to what he has to say", eventDoNothing)
	EventEnd;

	EventStart("Traders are selling weapons", 1, 1)
		EventSetTimeout(3, 4)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventNegResult("Unperturbed, they thank you for your time and move on")
		EventString("A few traders have arrived! They are willing to sell weapons for food")
		EventOption("Sell!", eventSellFoodForWeapons)
		EventOptionReq(Resource_Food, 20, 40)
		EventOption("Decline their offer", eventDoNothing)
	EventEnd;

	EventStart("Traders are selling weapons", 1, 1)
		EventSetTimeout(3, 6)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventNegResult("")
		EventNegResult("Unperturbed, they thank you for your time and move on")
		EventString("A few traders have arrived! They are willing to sell weapons for wood")
		EventOption("Sell!", eventSellWoodForWeapons)
		EventOptionReq(Resource_Wood, 10, 30)
		EventOption("Decline their offer", eventDoNothing)
	EventEnd;

	EventStart("Traders have tools for sale", 1, 1)
		EventSetTimeout(3, 6)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventNegResult("Unperturbed, they thank you for your time and move on")
		EventString("A few traders have arrived! They are willing to sell tools for food")
		EventOption("Sell!", eventSellFoodForTools)
		EventOptionReq(Resource_Food, 10, 30)
		EventOption("Decline their offer", eventDoNothing)
	EventEnd;

	EventStart("Traders have tools for sale", 1, 1)
		EventSetTimeout(3, 6)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventNegResult("")
		EventNegResult("Unperturbed, they thank you for your time and move on")
		EventString("A few traders have arrived! They are willing to sell tools for wood")
		EventOption("Sell!", eventSellWoodForTools)
		EventOptionReq(Resource_Wood, 5, 20)
		EventOption("Decline their offer", eventDoNothing)
	EventEnd;

	EventStart("Traders have tools for sale", 1, 1)
		EventSetTimeout(3, 6)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventNegResult("Unperturbed, they thank you for your time and move on")
		EventString("A few traders have arrived! They are willing to sell tools for food")
		EventOption("Sell!", eventSellFoodForTools)
		EventOptionReq(Resource_Food, 10, 30)
		EventOption("Decline their offer", eventDoNothing)
	EventEnd;

	EventStart("Traders have tools for sale", 1, 1)
		EventSetTimeout(3, 6)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventNegResult("")
		EventNegResult("Unperturbed, they thank you for your time and move on")
		EventString("A few traders have arrived! They are willing to sell tools for wood")
		EventOption("Sell!", eventSellWoodForTools)
		EventOptionReq(Resource_Wood, 5, 20)
		EventOption("Decline their offer", eventDoNothing)
	EventEnd;


	minPopNeeded = 12;
	//Selling tools/weapons
	EventStart("Traders will buy tools", 1, 1)
		EventSetTimeout(3, 6)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventNegResult("Unperturbed, they thank you for your time and move on")
		EventString("A few traders have arrived! They are willing to sell resources for tools")
		EventOption("Sell!", eventSellToolsForStuff)
		EventOptionReq(Resource_Tools, 1, 5)
		EventOption("Decline their offer", eventDoNothing)
	EventEnd;

	EventStart("Traders will buy weapons", 1, 1)
		EventSetTimeout(3, 6)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventNegResult("Unperturbed, they thank you for your time and move on")
		EventString("A few traders have arrived! They are willing to sell resources for weapons")
		EventOption("Sell!", eventSellWeaponsForStuff)
		EventOptionReq(Resource_Weapons, 1, 5)
		EventOption("Decline their offer", eventDoNothing)
	EventEnd;

}

//TODO(will) remember you have to set the people involved by hand
void populateEventFromTemplate(World* world, WorldEvent* event, WorldEvent* template)
{
	memcpy(event, template, sizeof(WorldEvent));
	event->timeout = getRandRangeF64(world->r, event->timeoutMin, event->timeoutMax) * 2;
	if(world->day > 6) {
		f32 t = (world->day - 6) / 18;
		if(t > 0.75) t = 0.75;
		event->timeout *= 1 - t;
	}
	event->timeout += 0.5;

	event->involveCount = getRandomRange(world->r,
				event->involveCountMin, event->timeoutMax);
	for(isize i = 0; i < event->optionCount; ++i) {
		if(event->optionReqs[i].hasReq) {
			event->optionReqs[i].amt = getRandomRange(
					world->r, event->optionReqs[i].amtMin,
					event->optionReqs[i].amtMax) * (1 + world->day / 5);
		}
	}
}

