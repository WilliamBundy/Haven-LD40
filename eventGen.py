def EventStart(a, b, c): pass
def EventSetTimeout(a, b): pass
def EventSelectCount(a, b): pass
def EventString(a): pass
def EventOption(a, b):
    if not ((b + "_") in globals().keys()):
        print("CreateEventAction({0})\n{{\n\treturn ;\n}}\n".format(b))
    globals()[b + "_"] = True
def EventDefaultOption(a):
    EventOption(None, a)

while True:
    try: 
        EventEnd = None
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
        break
    except NameError as err:
        msg = str(err).split("'")
        globals()[msg[1]] = msg[1]



