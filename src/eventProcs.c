
#define CreateEventAction(name) int name(World* world, WorldEvent* event, int something, void* ud)

CreateEventAction(eventDoNothing)
{
	return 0;
}

CreateEventAction(eventWelcomeTravellers)
{
	for(isize i = 0; i < event->involveCount; ++i) {
		Actor* a = addActor(generateActor());
		arenaPop(tempArena);

		event->involves[i] = a;
	}
	return 0;
}

CreateEventAction(eventIgnoreHurtGatherers)
{
	if(event->involves[0]) {
		i32 mod = 1;
		for(isize i = 0; i < 4; ++i) {
			if(event->involves[0]->positiveTraits[i] == Trait_Tough) {
				mod = 2;
			}
		}
		event->involves[0]->health -= getRandomRange(world->r, 2, 4) / mod;
		event->involves[0]->job = ActorJob_None;
	}
	return 0;
}

CreateEventAction(eventSendSomeoneToHelpGatherers)
{
	if(event->involves[0]) {
		event->involves[0]->job = ActorJob_None;
		if(event->selected[0]) {
			event->selected[0]->job = ActorJob_None;
		}
	}
	return 0;
}

CreateEventAction(eventSendToDefuse)
{
	return 0;
}

CreateEventAction(eventIgnoreStranger)
{
	return 0;
}

CreateEventAction(eventSendToDefuseGatherer)
{
	return 0;
}

CreateEventAction(eventIgnoreStrangerGatherer)
{
	return 0;
}

CreateEventAction(eventSendToDefuseWoodcutter)
{
	return 0;
}

CreateEventAction(eventIgnoreStrangerWoodcutter)
{
	return 0;
}

CreateEventAction(eventExtraHungryEat)
{
	if(event->involves[0]) {
		if(world->resources.food > 1) {
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
		event->involves[0]->food -= 50;
		event->involves[0]->mood--;
		event->resultValues[0] = 50;
	}
	return 0;
}

CreateEventAction(eventWakeUpNapper)
{
	if(event->involves[0]) {
		event->involves[0]->mood--;
		event->involves[0]->mood--;
	}
	return 0;
}

CreateEventAction(eventLetThemSleep)
{
	if(event->involves[0]) {
		event->involves[0]->mood += 2;
		event->involves[0]->job = ActorJob_None;
		event->resultValues[0] = 2;
	}
	return 0;
}

CreateEventAction(eventFreeFood)
{
	world->resources.food += getRandomRange(world->r, world->actorCount/2, world->actorCount * 1.5);
	return 0;
}

CreateEventAction(eventFreeWood)
{
	int res = getRandomRange(world->r, 1 * world->day, 3 * world->day);
	world->resources.food += res;
	event->resultValues[0] = res;
	return 0;
}


CreateEventAction(eventArgumentDefault)
{
	if(event->involves[0] && event->involves[1]) {
		event->involves[0]->mood -= 2;
		event->involves[1]->mood -= 2;
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
	}
	return 0;
}

CreateEventAction(eventSideWithB)
{
	if(event->involves[0] && event->involves[1]) {
		event->involves[0]->mood -= 3;
		event->resultValues[0] = 3;
	}
	return 0;
}

CreateEventAction(eventSeparateTheTwo)
{
	if(event->involves[0] && event->involves[1]) {
		event->involves[0]->mood--;
		event->involves[1]->mood--;
		event->resultValues[0] = 1;
		event->resultValues[1] = 1;
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


#define EventStart(name, involvedCountMin, involvedCountMax) { WorldEvent* e = getNextEvent(events, eventCount); \
	e->text[e->textCount++] = name;\
	e->kind = i++; \
	e->involveCountMin = involvedCountMin; \
	e->involveCountMax = involvedCountMax; \
	e->minPopNeeded = minPopNeeded;\
	e->optionRequiresSelection = -1;
#define EventKind(k) e->kind = Event_##k;
#define EventSetTimeout(mn, mx) e->timeoutMin = mn; e->timeoutMax = mx;
#define EventSelectCount(mn, mx) e->peopleToSelectMin = mn; e->peopleToSelectMax = mx;
#define EventString(str) e->text[e->textCount++] = str;
#define EventOption(str, result) e->options[e->optionCount] = str; e->actions[e->optionCount++] = result;
#define EventOptionReqSelected e->optionRequiresSelection = e->optionCount - 1;
#define EventDefaultOption(result) e->defaultAction = result;
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
		EventSetTimeout(3, 6)
		EventKind(Outsider)
		EventDefaultOption(eventDoNothing)
		EventString("Some wandering travellers have come across your haven, "
				"and would like to join your group.")
		EventOption("Welcome them", eventWelcomeTravellers)
		EventOption("Turn them away", eventDoNothing)
	EventEnd;

	EventStart("Some people were hurt while gathering food", 1, 3)
		EventSetTimeout(1, 2)
		EventKind(Accident)
		EventDefaultOption(eventIgnoreHurtGatherers)
		EventSelectCount(1, 1)
		EventString("A few people were attacked by wild animals while foraging for food. "\
				"Will you send someone to help them?")
		EventOption("Send Someone", eventSendSomeoneToHelpGatherers);
		EventOptionReqSelected;
		EventOption("Hope they make it", eventIgnoreHurtGatherers);
	EventEnd;

	EventStart("Stranger threatens your group", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Outsider)
		EventSelectCount(1, 1)
		EventOption("Send someone to defuse the situation", eventSendToDefuse)
		EventOptionReqSelected;
		EventOption("Ignore him", eventIgnoreStranger)
	EventEnd;

	EventStart("Stranger threatens your gatherers", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Outsider)
		EventSelectCount(1, 1)
		EventOption("Choose a gatherer to talk", eventSendToDefuseGatherer)
		EventOptionReqSelected;
		EventOption("Ignore him", eventIgnoreStrangerGatherer)
	EventEnd;

	EventStart("Stranger threatens your woodcutters", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Outsider)
		EventSelectCount(1, 1)
		EventOption("Choose a woodcutter to talk", eventSendToDefuseWoodcutter)
		EventOptionReqSelected;
		EventOption("Ignore him", eventIgnoreStrangerWoodcutter)
	EventEnd;

	EventStart("Someone was extra hungry", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Solo)
		EventOption("Let them eat more", eventExtraHungryEat);
		EventOption("Deny them the food they want", eventExtraHungryNoEat);
	EventEnd;

	EventStart("Someone decided to take a nap", 1, 1)
		EventSetTimeout(1, 2)
		EventKind(Solo)
		EventOption("Wake them up! No slacking allowed!", eventWakeUpNapper);
		EventOption("Let them sleep; they could do with the rest", eventLetThemSleep);
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

	minPopNeeded = 4;
	EventStart("Someone insulted someone!", 2, 2)
		EventKind(Conflict)
		EventOption("Nothing you can do about it now", eventInsulted)
	EventEnd;

	EventStart("Some poeple got into an argument", 2, 2)
		EventKind(Conflict)
		EventSetTimeout(1, 2)
		EventDefaultOption(eventArgumentFinish)
		EventString("Two people started arguing over a difference of opinion")
		EventOption("Take sides with %s", eventSideWithA)
		EventOption("Take sides with %s", eventSideWithB)
		EventOption("Separate the two", eventSeparateArgument);
	EventEnd;

	EventStart("Some people started fighting", 2, 2)
		EventKind(Conflict)
		EventSetTimeout(1, 2)
		EventDefaultOption(eventLetFightFinish)
		EventString("%s started a fight with")
		EventString("%s!")
		EventOption("Break up the fight", eventBreakUpFight);
		EventOption("Let them fight it out", eventLetFightFinish);
	EventEnd;

}

//TODO(will) remember you have to set the people involved by hand
void populateEventFromTemplate(World* world, WorldEvent* event, WorldEvent* template)
{
	memcpy(event, template, sizeof(WorldEvent));
	event->timeout = getRandRangeF64(world->r, event->timeoutMin, event->timeoutMax);
	event->involveCount = getRandomRange(world->r,
				event->involveCountMin, event->timeoutMax);
}

