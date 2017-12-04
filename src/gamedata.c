enum {
	Resource_Food,
	Resource_Wood,
	Resource_Tools,
	Resource_Weapons,
	Resource_Artifacts,
};

string resourceNames[] = {
	"Food", "Wood", "Tools", "Weapons", "Artifacts"
};




#define GenEventKinds \
	M_event(Gift, 0x228800AA) \
	M_event(Solo, 0x333333AA) \
	M_event(Group, 0x222222AA) \
	M_event(Accident, 0x994400AA) \
	M_event(Conflict, 0x990000AA) \
	M_event(Raid, 0x990000AA) \
	M_event(Outsider, 0x000088AA) \
	M_event(Special, 0x550088AA) \
	M_event(Tutorial, 0xFFCC00AA)
			
#define M_event(name, ...) Event_##name,
enum EventKinds
{
	GenEventKinds
	EventKindCount
};

#undef M_event
#define M_event(name, ...) #name,
string eventKindStrings[] = {
	GenEventKinds
};
#undef M_event
#define M_event(name, color, ...) color,
u32 eventKindColors[] = {
	GenEventKinds
};

#define GenTraits \
	M_trait(Dextrous, Clumsy, 1, 1, 1.5, 0.9, 1.5, 0.75) \
	M_trait(Fast, Slow, 1, 1, 1.1, 1, 1.1, 0.9) \
	M_trait(Strong, Weak, 1, 1, 1.1, 1, 2, 0.6) \
	M_trait(Clever, Dull, 0.5, 1, 1, 1, 1.1, 1) \
	M_trait(Calm, Anxious, 0.5, 2, 1, 1, 1.1, 0.95) \
	M_trait(Serious, Lazy, 0.5, 1, 1.5, 0.75, 1, 1) \
	M_trait(Tough, Fragile, 1, 1, 1, 1, 1, 1) \
	M_trait(Kind, Mean, 0.5, 2, 1, 1, 0.75, 1.5) \
	M_trait(Brave, Cowardly, 1, 2, 1, 1, 1.5, 0.9)

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

#define M_trait(name1, name2, pm, nm, ...) pm,
float posTraitAggroModifier[] = {
	1,
	GenTraits

};
#undef M_trait


#define M_trait(name1, name2, pm, nm, ...) nm,
float negTraitAggroModifier[] = {
	1,
	GenTraits

};
#undef M_trait
#define M_trait(name1, name2, pm, nm, ppm, nnm, ...) ppm,
float posTraitWorkMod[] = {
	1,
	GenTraits

};
#undef M_trait

#define M_trait(name1, name2, pm, nm, ppm, nnm, ...) nnm,
float negTraitWorkMod[] = {
	1,
	GenTraits

};
#undef M_trait

#define M_trait(name1, name2, pm, nm, ppm, nnm, pppm, nnnm,...) pppm,
float posTraitDmgMod[] = {
	1,
	GenTraits

};
#undef M_trait

#define M_trait(name1, name2, pm, nm, ppm, nnm, pppm, nnnm, ...) nnnm,
float negTraitDmgMod[] = {
	1,
	GenTraits

};
#undef M_trait

#define GenActorJobs \
	M_job(None, "resting", "nothing") \
	M_job(FoodGather, "gathering food", "food") \
	M_job(WoodGather, "wood cutting", "wood") \
	M_job(Crafting, "crafting", "craft work") \
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
	
string craftTargets[] = {
	"none", 
	"tool",
	"weapon"
};

string buildTargets[] = {
	"none", 
	"hut", 
	"farm",
	"smith"
};
