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
def EventKind(a): pass
def EventOptionReq(a, b, c): pass

while True:
    try: 
        EventStart("Traders will buy food", 1, 1)
        EventSetTimeout(3, 6)
        EventKind(Outsider)
        EventDefaultOption(eventDoNothing)
        EventString("A few traders have arrived! They are willing to sell tools for food")
        EventOption("Sell!", eventFoodForTools)
        EventOptionReq(Resource_Food, 10, 30)
        EventOption("Decline their offer", eventDoNothing)
        EventEnd;

        EventStart("Traders will buy wood", 1, 1)
        EventSetTimeout(3, 6)
        EventKind(Outsider)
        EventDefaultOption(eventDoNothing)
        EventString("A few traders have arrived! They are willing to sell tools for wood")
        EventOption("Sell!", eventWoodForTools)
        EventOptionReq(Resource_Wood, 5, 20)
        EventOption("Decline their offer", eventDoNothing)
        EventEnd;

        EventStart("Traders will buy food", 1, 1)
        EventSetTimeout(3, 6)
        EventKind(Outsider)
        EventDefaultOption(eventDoNothing)
        EventString("A few traders have arrived! They are willing to sell wood for food")
        EventOption("Sell!", eventFoodForWood)
        EventOptionReq(Resource_Food, 5, 20)
        EventOption("Decline their offer", eventDoNothing)
        EventEnd;

        EventStart("Traders will buy wood", 1, 1)
        EventSetTimeout(3, 6)
        EventKind(Outsider)
        EventDefaultOption(eventDoNothing)
        EventString("A few traders have arrived! They are willing to sell food for wood")
        EventOption("Sell!", eventWoodForFood)
        EventOptionReq(Resource_Wood, 2, 10)
        EventOption("Decline their offer", eventDoNothing)
        EventEnd;

        EventStart("Traders will buy tools", 1, 1)
        EventSetTimeout(3, 6)
        EventKind(Outsider)
        EventDefaultOption(eventDoNothing)
        EventString("A few traders have arrived! They are willing to sell resources for tools")
        EventOption("Sell!", eventToolsForStuff)
        EventOptionReq(Resource_Tools, 1, 5)
        EventOption("Decline their offer", eventDoNothing)
        EventEnd;

        EventStart("Traders will buy weapons", 1, 1)
        EventSetTimeout(3, 6)
        EventKind(Outsider)
        EventDefaultOption(eventDoNothing)
        EventString("A few traders have arrived! They are willing to sell resources for weapons")
        EventOption("Sell!", eventWeaponsForStuff)
        EventOptionReq(Resource_Weapons, 1, 5)
        EventOption("Decline their offer", eventDoNothing)
        EventEnd;
        break
    except NameError as err:
        msg = str(err).split("'")
        globals()[msg[1]] = msg[1]



