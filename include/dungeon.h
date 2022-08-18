#ifndef DUNGEON_H
#define DUNGEON_H

struct skeleton {
    int loc;
    unsigned char flags;
};

struct room {
    unsigned char desc, flags;
};

enum {
    PFLAG_DUCK=1,
    PFLAG_REVOLVER=2,
};

enum {
    RFLAG_REVOLVER=1,
    RFLAG_GOLD=2,
    RFLAG_AMMO=4,
    RFLAG_KNOWN=0xf0,
};

enum {
    SFLAG_MOVED=1,
};

static const char *introMsg = ""
"Welcome to CoTAS - Castle of The Animated Skeletons!\n"
"\n"
"You have come to the castle to find your fortune...\n"
"- Shoot skeletons with a your trusty revolver,\n"
"   or duck to trip them up!\n"
"- If you aren't careful, they may EAT YOUR SOUL!\n"
"- Search for the exit to go deeper,\n"
"   or return to the entrance and quit while you're ahead!\n"
"- Type 'help' for a list of controls.\n\n"
"";

extern struct skeleton skeletons[];
extern int numSkeletons;

extern int map[];
extern struct room rooms[];
extern int mapsz, numRooms, currentLevel, maxLevel;
extern int playerLoc, playerFlags, playerAmmo, playerGold;

/* A [desc1] [desc2]. */

static const char *desc1[] = {
    "narrow",
    "shadowy",
    "foul smelling",
    "rugged",
    "small",
    "cold",
    "lonely",
    "dull",
    "dusty",
    "cramped",
    "dark",
    "dank",
    "damp",
    "quiet",
    "still",
    "crumbling",
};

static const char *desc2[] = {
    "ruin",
    "hovel",
    "chamber",
    "passage",
    "hall",
    "hallway",
    "pit",
    "crawlway",
    "living quarters",
    "coal cellar",
    "prayer room",
    "garrison",
    "store room",
    "prison cell",
    "cavern",
    "closet",
};

void genMap(int);
void printMap();
void describeRoom();
void describeSkeletons();

#endif
