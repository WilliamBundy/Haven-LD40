void addDebugEvent()
{
	World* world = play.world;
	if(play.eventCount >= 256) return;
	WorldEvent* event = play.events + play.eventCount++;
	memset(event, 0, sizeof(WorldEvent));
	event->timeout = getRandRangeF64(world->r, 1, 4);

	event->kind = getRandomRange(world->r, 0, EventKindCount);
	event->text[0] = eventKindStrings[event->kind];
	event->text[1] = eventKindStrings[event->kind];
}

int eventAddPeople(WorldEvent* e)
{
	isize added = 0; 
	World* world = play.world;
	if(e->jobSpecific) {
		//build candidate list, choose from that
		Actor* candidates[256];
		isize candidateCount = 0;
		for(isize i = 0; i < world->actorCount; ++i) {
			Actor* a = world->actors + i;
			if(a->job != e->jobSpecific) continue;

			candidates[candidateCount++] = a;
		}

		if(candidateCount < e->involveCount) {
			added = -1;
			e->involveCount = candidateCount;
			//TODO(will) maybe we should decide if the job should implode
			//// if there aren't enough people for it
		}

		for(isize i = 0; i < e->involveCount; ++i) {
			isize index = getRandomRange(world->r, 0, candidateCount);
			if(i > 0) {
				//uniquenses check
				for(isize j = 0; j < i; ++j)  {
					while(e->involves[j] == candidates[index]) {
						index = getRandomRange(world->r, 0, candidateCount);
					}
				}
			}
			added++;
			e->involves[i] = candidates[index];
		}
	} else {
		for(isize i = 0; i < e->involveCount; ++i) {
			isize index = getRandomRange(world->r, 0, world->actorCount);
			if(i > 0) {
				//uniquenses check
				for(isize j = 0; j < i; ++j)  {
					while(e->involves[j] == world->actors + index) {
						index = getRandomRange(world->r, 0, world->actorCount);
					}
				}
			}

			added++;
			e->involves[i] = world->actors + index;
		}
	}

	printf("%d %d\n", added, e->involveCount);
	return added < e->involveCount;
}

WorldEvent* getEvent(int kind)
{
	World* world = play.world;
	if(play.eventCount >= 256) return NULL;
	WorldEvent* event = play.events + play.eventCount;
	memset(event, 0, sizeof(WorldEvent));
	event->timeout = 1;
	WorldEvent* candidates[256];
	isize candidateCount = 0;
	for(isize i = 0; i < eventTemplateCount; ++i) {
		WorldEvent* t = eventTemplates + i;
		if(t->minPopNeeded > world->actorCount) break;
		if(t->kind != kind) continue;
		candidates[candidateCount++] = t;
	}

	if(candidateCount == 0) return NULL;
	WorldEvent* chosen = candidates[getRandomRange(
			world->r, 0, candidateCount)];
	populateEventFromTemplate(world, event, chosen);
	play.eventCount++;
	
	WorldEvent* e = event;
	return event;
}

WorldEvent* getSpecificEvent(int index)
{
	World* world = play.world;
	if(play.eventCount >= 256) return NULL;
	WorldEvent* event = play.events + play.eventCount;
	memset(event, 0, sizeof(WorldEvent));
	WorldEvent* chosen = eventTemplates + index;
	populateEventFromTemplate(world, event, chosen);
	event->timeout = 100;
	play.eventCount++;
	return event;
}

void generateGiftEvent()
{
	WorldEvent* e = getEvent(Event_Gift);
	if(!e) return;
}

void generateSoloEvent()
{
	WorldEvent* e = getEvent(Event_Solo);
	if(!e) return;
	int ret = eventAddPeople(e);
	if(ret) {
		play.eventCount--;
	}
}


void generateGroupEvent()
{
	WorldEvent* e = getEvent(Event_Group);
	if(!e) return;
	int ret = eventAddPeople(e);
	if(ret) {
		play.eventCount--;
	}
}
void generateAccidentEvent()
{
	WorldEvent* e = getEvent(Event_Accident);
	if(!e) return;
	int ret = eventAddPeople(e);
	if(ret) {
		play.eventCount--;
	}
}

void generateOutsiderEvent()
{
	WorldEvent* e = getEvent(Event_Outsider);
	if(!e) return;
}

void generateSpecialEvent()
{
	WorldEvent* e = getEvent(Event_Special);
	if(!e) return;
}

void generateConflictEvent()
{
	WorldEvent* e = getEvent(Event_Conflict);
	if(!e) return;
	World* world = play.world;
	int candidateGroups[ActorJobCount];
	isize groupCount = 0;
	for(isize i = 0; i < ActorJobCount; ++i) {
		if(world->jobCount[i] > 0)  {
			candidateGroups[groupCount++] = i;
		}
	}

	isize chosenGroup = getRandomRange(world->r, 0, groupCount);
	//printf("Chosen group : %d\n", chosenGroup);
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
	
	if(!aggro) {
		play.eventCount--;
		return;
	}

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
		play.eventCount--;
		return;
	} else {
		e->involves[0] = aggro;
		e->involves[1] = responds;
	}

}


