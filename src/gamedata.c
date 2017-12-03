#define GenEventKini \
	M_event(WantToJoin, \
			"Some travelers want to join your haven.", "Will you let them?")\
	M_event(Hungry, \
			"Someone is extra hungry.", "Will you feed them?")\
	M_event(Argument, \
			"Some people are having an argument.", "Will you take sides?", \
			"Help %s", "Help %s", "Break them up")\
	M_event(Theft, \
			"Someone stole something", "Will you punish them?")\
	M_event(Rotting,\
			"Some food rotted!", "Will you move it somewhere?")
#define GenEventKinds \
	M_event(WantToJoin, \
			"Some travelers want to join your haven.", "Will you let them?")\
	M_event(Hungry, \
			"Someone is extra hungry.", "Will you feed them?")\
	M_event(Argument, \
			"Some people are having an argument.", "Will you take sides?", \
			"Help %s", "Help %s", "Break them up")\
	M_event(Theft, \
			"Someone stole something", "Will you punish them?")\
	M_event(Rotting,\
			"Some food rotted!", "Will you move it somewhere?")

#define M_event(name, ...) Event_##name,
enum EventKinds
{
	GenEventKinds

	EventKindCount
};

#undef M_event
#define M_event(name, text1, text2, ...) #name, text1, text2,
string eventStrings[] = {
	GenEventKinds
};

#define GenTraits \
	M_trait(Dextrous, Clumsy, 1, 1) \
	M_trait(Fast, Slow, 1, 1) \
	M_trait(Strong, Weak, 1, 1) \
	M_trait(Clever, Dull, 0.5, 1) \
	M_trait(Calm, Anxious, 0.5, 2) \
	M_trait(Serious, Lazy, 0.5, 1) \
	M_trait(Tough, Fragile, 1, 1) \
	M_trait(Kind, Mean, 0.5, 2) \
	M_trait(Brave, Cowardly, 1, 2)

#define M_trait(name1, name2, ...) Trait_##name1,
enum PositiveTraits
{
	Trait_Positive,
	GenTraits
	PositiveTraitCount
};
#undef M_trait

#define M_trait(name1, name2, ...) Trait_##name2,
enum NegativeTraits
{
	Trait_Negative,
	GenTraits
	NegativeTraitCount
};
#undef M_trait

#define M_trait(name1, name2, ...) #name1,
string posTraitNames[] = {
	"none",
	GenTraits
};
#undef M_trait

#define M_trait(name1, name2, ...) #name2,
string negTraitNames[] = {
	"none",
	GenTraits

};
#undef M_trait

#define M_trait(name1, name2, pm, nm) pm,
float posTraitAggroModifier[] = {
	1,
	GenTraits

};
#undef M_trait


#define M_trait(name1, name2, pm, nm) nm,
float negTraitAggroModifier[] = {
	1,
	GenTraits

};
#undef M_trait

#define GenActorJobs \
	M_job(None, "idle", "nothing") \
	M_job(FoodGather, "gathering food", "food") \
	M_job(WoodGather, "wood cutting", "wood") \
	M_job(Crafting, "crafting", "craftwork") \
	M_job(Building, "building", "build work") 

#define M_job(name, ...) ActorJob_##name,
enum ActorStates {
	GenActorJobs
	ActorJobCount
};

#undef M_job
#define M_job(name, desc, ...) desc,
string jobDescs[] = {
	GenActorJobs
};
#undef M_job
#define M_job(name, desc, t, ...) t,
string jobContribType[] = {
	GenActorJobs
};
#undef M_job

#define GenActorStates \
	M_astate(Idle, "relaxing") \
	M_astate(Working, "working") \
	M_astate(Moping, "moping around") \
	M_astate(Slacking, "slacking off") \
	M_astate(Hungry, "hungry") \
	M_astate(Suffering, "suffering") \
	M_astate(Helping, "helping") \
	M_astate(Fighting, "fighting")

#define M_astate(name, ...) ActorState_##name,
enum ActorJobs {
	GenActorStates
	ActorStateCount
};
#undef M_astate
#define M_astate(name, desc) desc,
string astateDescs[] = {
	GenActorStates
};
#undef M_astate
	

