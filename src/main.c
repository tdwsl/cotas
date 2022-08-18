#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "strutil.h"
#include "dungeon.h"
#include "dirs.h"

void printScore()
{
    char buf[10];
    printf("Your final score is %d.\n", playerGold*10+maxLevel*5);
    printf("Press enter...\n");
    fgets(buf, 10, stdin);
}

void endTurn()
{
    int i, j, d, x, y;

    for (i = 0; i < numSkeletons; i++) {
        if (skeletons[i].flags & SFLAG_MOVED) {
            skeletons[i].flags ^= SFLAG_MOVED;
            continue;
        }

        if (skeletons[i].loc == playerLoc) {
            if (playerFlags & PFLAG_REVOLVER) {
                playerAmmo = 0;
                playerFlags ^= PFLAG_REVOLVER;
                printf("A skeleton touches your revolver,"
                    " causing it to crumble to dust!\n\n");
            } else {
                printf("A skeleton eats your soul!\n");
                printScore();
                exit(0);
            }
            return;
        }

        for (d = 0; d < 4; d++) {
            x = skeletons[i].loc%mapsz+dirs[d*2];
            y = skeletons[i].loc/mapsz+dirs[d*2+1];
            if (x == playerLoc%mapsz && y == playerLoc/mapsz) {
                break;
            }
        }

        if (d == 4) {
            d = rand()%4;

            for (j = 0; j < 4; j++) {
                d = (d+1)%4;
                x = skeletons[i].loc%mapsz+dirs[d*2];
                y = skeletons[i].loc/mapsz+dirs[d*2+1];

                if (x < 0 || y < 0 || x >= mapsz || y >= mapsz) {
                    continue;
                }

                if (map[y*mapsz+x]) {
                    break;
                }
            }
        }

        if (skeletons[i].loc == playerLoc) {
            printf("A skeleton exits to the %s...\n", dirstrs[d]);
        }
        skeletons[i].loc = y*mapsz+x;
        skeletons[i].flags &= SFLAG_MOVED;

        if (skeletons[i].loc == playerLoc) {
            if (playerFlags & PFLAG_DUCK) {
                printf("A skeleton trips over you, and smashes to pieces!\n");
                skeletons[i--] = skeletons[--numSkeletons];
                playerFlags ^= PFLAG_DUCK;
                playerGold++;
                continue;
            }

            printf("A skeleton enters from the %s!\n", dirstrs[(d+2)%4]);
        }
    }
}

void tryMove(int d)
{
    int x, y, i;
    const char *err = "You can't go that way.\n";

    x = playerLoc%mapsz+dirs[d*2];
    y = playerLoc/mapsz+dirs[d*2+1];

    if (x < 0 || y < 0 || x >= mapsz || y >= mapsz) {
        printf(err);
        return;
    }

    i = y*mapsz+x;
    if (!map[i]) {
        printf(err);
        return;
    }

    printf("You move %s.\n\n", dirstrs[d]);
    playerLoc = i;
    rooms[map[playerLoc]-1].flags |= RFLAG_KNOWN;

    endTurn();
    describeRoom();
}

void printHelp()
{
    printf(""
"List of Commands:\n"
"  north, east,\n"
"  south, west   - move around the current level\n"
"\n"
"  up/down       - travel up or down a flight of stairs\n"
"\n"
"  duck          - trip up one incoming skeleton\n"
"  fire <dir>    - quickly shoot a skeleton with your trusty revolver\n"
"  take <item>   - pick something up\n"
"\n"
"  map           - print a layout of the current level\n"
"  look          - reprint the current description\n"
"  help          - print this message\n"
"  quit          - exit the program\n"
"Every command can be shortened - for example, 'n' or 'no' instead of\n"
"'north'.\n"
"This works with parameters, too, so 't r' is a valid substitute for\n"
"'take revolver'.\n"
"");
}

void take(char *itm)
{
    const char *err = "You don't see that here.\n";

    if (strstart("revolver", itm)) {
        if (!(rooms[map[playerLoc]-1].flags & RFLAG_REVOLVER)) {
            printf(err);
        } else if (playerFlags & PFLAG_REVOLVER) {
            printf("You already have a revolver.\n");
        } else {
            rooms[map[playerLoc]-1].flags ^= RFLAG_REVOLVER;
            playerFlags |= PFLAG_REVOLVER;
            printf("You take the revolver.\n");
            endTurn();
        }
    } else if (strstart("gold", itm)) {
        if (!(rooms[map[playerLoc]-1].flags & RFLAG_GOLD)) {
            printf(err);
        } else {
            rooms[map[playerLoc]-1].flags ^= RFLAG_GOLD;
            playerGold++;
            printf("You get the gold.\n");
            endTurn();
        }
    } else if (strstart("ammo", itm)) {
        if (!(rooms[map[playerLoc]-1].flags & RFLAG_AMMO)) {
            printf(err);
        } else if (!(playerFlags & PFLAG_REVOLVER)) {
            printf("You don't have the revolver.\n");
        } else if (playerAmmo >= 6) {
            printf("The revolver can only hold 6 bullets.\n");
        } else {
            rooms[map[playerLoc]-1].flags ^= RFLAG_AMMO;
            playerAmmo++;
            printf("You load the revolver ammo.\n");
            printf("You now have %d in the chamber.\n", playerAmmo);
            endTurn();
        }
    } else {
        printf("You can only pick up revolvers, gold or ammo.\n");
    }
}

void shoot(char *dir)
{
    int i, x, y, loc;
    const char *fail =
        "The bullet ricochets off the wall and kills you!\n";

    if (!(playerFlags & PFLAG_REVOLVER)) {
        printf("Not without a gun.\n");
        return;
    }

    if (playerAmmo == 0) {
        printf("No bullets.\n");
        return;
    }

    for (i = 0; i < 4 && !strstart(dirstrs[i], dir); i++);
    if (i >= 4) {
        printf("You can only shoot north, east, south or west.\n");
        return;
    }

    printf("Bang!\n");
    playerAmmo--;

    x = playerLoc%mapsz+dirs[i*2];
    y = playerLoc/mapsz+dirs[i*2+1];

    if (x < 0 || y < 0 || x >= mapsz || y >= mapsz) {
        printf(fail);
        printScore();
        exit(0);
    }

    loc = y*mapsz+x;
    if (map[loc] == 0) {
        printf(fail);
        printScore();
        exit(0);
    }

    for (i = 0; i < numSkeletons; i++) {
        if (skeletons[i].loc == loc) {
            printf("The bullet shatters the spine of a skeleton!\n");
            skeletons[i] = skeletons[--numSkeletons];
            return;
        }
    }

    printf("You don't hit anything.\n");
}

int main()
{
    char buf[200];
    char *com, *a1, *a2;

    printf(introMsg);

    maxLevel = -1;
    currentLevel = 0;
    playerAmmo = 0;
    playerGold = 0;
    playerFlags = 0;
    genMap(currentLevel);
    rooms[map[playerLoc]-1].flags |= RFLAG_REVOLVER;
    describeRoom();

    for (;;) {
        printf("\n>");
        bzero(buf, 200);
        fgets(buf, 199, stdin);

        com = buf;
        trim(com);
        lowercase(com);
        a1 = split(com);
        a2 = split(a1);

        if (!com[0]) {
            continue;
        }

        if (strstart("help", com)) {
            printHelp();
        } else if (strstart("look", com)) {
            describeRoom();
        } else if (strstart("north", com)) {
            tryMove(0);
        } else if (strstart("east", com)) {
            tryMove(1);
        } else if (strstart("south", com)) {
            tryMove(2);
        } else if (strstart("west", com)) {
            tryMove(3);
        } else if (strstart("up", com)) {
            if (map[playerLoc] != 1) {
                printf("There are no stairs leading up here.\n");
            } else if (currentLevel == 0) {
                printf("You leave the dungeon...\n");
                playerGold *= 2;
                maxLevel *= 3;
                break;
            } else {
                printf("You travel up a maze of stairs...\n\n");
                genMap(currentLevel-1);
                describeRoom();
            }
        } else if (strstart("down", com)) {
            if (map[playerLoc] != numRooms) {
                printf("There are no stairs leading down here.\n");
            } else {
                printf("You travel down a maze of stairs...\n\n");
                genMap(currentLevel+1);
                describeRoom();
            }
        } else if (strstart("duck", com)) {
            printf("You momentarily duck...\n");
            playerFlags |= PFLAG_DUCK;
            endTurn();
            playerFlags ^= PFLAG_DUCK;
            printf("You stand back up.\n");
            describeSkeletons();
        } else if (strstart("take", com)) {
            take(a1);
        } else if (strstart("fire", com)) {
            shoot(a1);
        } else if (strstart("map", com)) {
            printMap();
        } else if (strstart("quit", com)) {
            printf("You find a place to rest...\n");
            break;
        } else {
            printf("You can't do that.\n");
        }
    }
    printScore();
    return 0;
}
