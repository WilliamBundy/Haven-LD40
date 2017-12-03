
#define CreateEventAction(name) int name(World* world, WorldEvent* event, int something, void* ud)

CreateEventAction(eventTurnAwayTravellers)
{
	return 0;
}

CreateEventAction(eventWelcomeTravellers)
{
	return 0;
}

CreateEventAction(eventIgnoreHurtGatherers)
{
	return 0;
}

CreateEventAction(eventSendSomeoneToHelpGatherers)
{
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
	return 0;
}

CreateEventAction(eventExtraHungryNoEat)
{
	return 0;
}

CreateEventAction(eventWakeUpNapper)
{
	return 0;
}

CreateEventAction(eventLetThemSleep)
{
	return 0;
}

CreateEventAction(eventFreeFood)
{
	return 0;
}

CreateEventAction(eventIgnoreFood)
{
	return 0;
}

CreateEventAction(eventFreeWood)
{
	return 0;
}

CreateEventAction(eventIgnoreWood)
{
	return 0;
}

CreateEventAction(eventArgumentDefault)
{
	return 0;
}

CreateEventAction(eventSideWithA)
{
	return 0;
}

CreateEventAction(eventSideWithB)
{
	return 0;
}

CreateEventAction(eventSeparateTheTwo)
{
	return 0;
}

CreateEventAction(eventFightDefault)
{
	return 0;
}

CreateEventAction(eventBreakUpFight)
{
	return 0;
}

CreateEventAction(eventLetFightFinish)
{
	return 0;
}


#define EventStart(name, involvedCountMin, involvedCountMax) { WorldEvent* e = getNextEvent(events, eventCount); \
	e->kind = i++; \
	e->involveCountMin = involvedCountMin; \
	e->involveCountMax = involvedCountMax; \
	e->minPopNeeded = minPopNeeded;
#define EventKind(k) event->kind = Event_##k;
#define EventSetTimeout(mn, mx) e->timeoutMin = mn; e->timeoutMax = mx;
#define EventSelectCount(mn, mx) e->peopleToSelectMin = mn; e->peopleToSelectMax = mx;
#define EventString(str) e->text[e->textCount++] = str;
#define EventOption(str, result) e->options[e->optionCount] = str; e->actions[e->optionCount++] = result;
#define EventDefaultOption(result) 
#define EventEnd }

WorldEvent* getNextEvent(WorldEvent* events, isize* eventCount)
{
	WorldEvent* e = events + *eventCount++;
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
		EventDefaultOption(eventTurnAwayTravellers)
		EventString("Some wandering travellers have come across your haven, "
				"and would like to join your group.")
		EventOption("Welcome them", eventWelcomeTravellers)
		EventOption("Turn them away", eventTurnAwayTravellers)
	EventEnd;

	EventStart("Some people were hurt while gathering food", 1, 3)
		EventSetTimeout(1, 2)
		EventDefaultOption(eventIgnoreHurtGatherers)
		EventSelectCount(1, 1)
		EventString("A few people were attacked by wild animals while foraging for food. "\
				"Will you send someone to help them?")
		EventOption("Send Someone", eventSendSomeoneToHelpGatherers);
		EventOption("Hope they make it", eventIgnoreHurtGatherers);
	EventEnd;

	EventStart("Stranger threatens your group", 1, 1)
		EventOption("Send someone to defuse the situation", eventSendToDefuse)
		EventOption("Ignore him", eventIgnoreStranger)
	EventEnd;

	EventStart("Stranger threatens your gatherers", 1, 1)
		EventOption("Choose a gatherer to talk", eventSendToDefuseGatherer)
		EventOption("Ignore him", eventIgnoreStrangerGatherer)
	EventEnd;

	EventStart("Stranger threatens your woodcutters", 1, 1)
		EventOption("Choose a woodcutter to talk", eventSendToDefuseWoodcutter)
		EventOption("Ignore him", eventIgnoreStrangerWoodcutter)
	EventEnd;

	EventStart("Someone was extra hungry", 1, 1)
		EventOption("Let them eat more", eventExtraHungryEat);
		EventOption("Deny them the food they want", eventExtraHungryNoEat);
	EventEnd;

	EventStart("Someone decided to take a nap", 1, 1)
		EventOption("Wake them up! No slacking allowed!", eventWakeUpNapper);
		EventOption("Let them sleep; they could do with the rest", eventLetThemSleep);
	EventEnd;

	EventStart("You found some food", 0, 0)
		EventOption("Nice!", eventFreeFood);
		EventOption("Ignore it", eventIgnoreFood);
	EventEnd;

	EventStart("You found some wood", 1, 1)
		EventOption("Nice!", eventFreeWood);
		EventOption("Ignore it", eventIgnoreWood);
	EventEnd;

	minPopNeeded = 4;
	EventStart("Some poeple got into an argument", 2, 2)
		EventSetTimeout(0.25, 1)
		EventDefaultOption(eventArgumentDefault)
		EventString("Two people started arguing over a difference of opinion")
		EventOption("Take sides with %s", eventSideWithA)
		EventOption("Take sides with %s", eventSideWithB)
		EventOption("Separate the two", eventSeparateTheTwo);
	EventEnd;

	EventStart("Some people started fighting", 2, 2)
		EventSetTimeout(0.25, 1)
		EventDefaultOption(eventFightDefault)
		EventString("An argument devolved into a fight!")
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

