#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dungeon.h"
#include "dirs.h"

int map[24*24];
struct room rooms[24*24];
struct skeleton skeletons[200];
int mapsz, numRooms, currentLevel, numSkeletons, maxLevel;
int playerLoc, playerFlags, playerAmmo, playerGold;

void genMap(int lvl)
{
    int i, j, k, d, c, x, y, p;
    int sz, skelodds, goldodds, ammoodds;

    switch (lvl) {
    case 0:
    case 1:
        sz = 4;
        skelodds = 8;
        goldodds = 7;
        ammoodds = 10;
        break;
    case 2:
    case 3:
    case 4:
        sz = 5;
        skelodds = 5;
        goldodds = 4;
        ammoodds = 8;
        break;
    default:
        sz = 6;
        skelodds = 3;
        goldodds = 2;
        ammoodds = 6;
        break;
    }

    if (lvl <= maxLevel) {
        skelodds /= 2;
        goldodds *= 2;
        ammoodds *= 2;
    } else {
        maxLevel = lvl;
    }

    mapsz = sz;
    for (i = 0; i < sz*sz; i++) {
        map[i] = 0;
    }

    srand(time(0));
    i = rand()%(sz*sz);
    map[i] = 1;
    playerLoc = i;
    numSkeletons = 0;

    i = 1;
    for (;;) {
        j = rand()%(sz*sz);

        for (k = 0; k < sz*sz; k++) {
            c = 0;
            j = (j+1)%(sz*sz);
            if (map[j]) {
                continue;
            }

            for (d = 0; d < 4; d++) {
                x = j%sz+dirs[d*2];
                y = j/sz+dirs[d*2+1];
                if (x < 0 || y < 0 || x >= sz || y >= sz) {
                    continue;
                }
                if (map[y*sz+x]) {
                    c++;
                }
            }

            if (c < 2 && c > 0) {
                break;
            }
        }

        if (map[j] || c == 0 || c >= 2) {
            break;
        }

        if (i != 1 && rand()%skelodds == 1) {
            skeletons[numSkeletons++] = (struct skeleton){j, 0};
        }

        map[j] = ++i;
        p = j;
    }

    numRooms = i;

    if (currentLevel > lvl) {
        playerLoc = p;
    }
    currentLevel = lvl;

    for (i = 0; i < numRooms; i++) {
        rooms[i].desc = rand()%256;
        rooms[i].flags = 0;

        if (i == 0 && !(playerFlags & PFLAG_REVOLVER)) {
            if (rand()%5 || currentLevel == maxLevel) {
                rooms[i].flags |= RFLAG_REVOLVER;
            }
        }
        if (rand()%goldodds == 1) {
            rooms[i].flags |= RFLAG_GOLD;
        }
        if (rand()%ammoodds == 1) {
            rooms[i].flags |= RFLAG_AMMO;
        }

        if (i >= 256) {
            continue;
        }

        for (;;) {
            for (j = 0; j < i; j++) {
                if (rooms[j].desc == rooms[i].desc) {
                    j = -1;
                    break;
                }
            }
            if (j != -1) {
                break;
            }
            rooms[i].desc++;
        }
    }

    rooms[map[playerLoc]-1].flags |= RFLAG_KNOWN;
}

void printMap() {
    int i;

    for (i = 0; i < mapsz*mapsz; i++) {
        if (map[i] == 0) {
            printf("   ");
        } else if (i == playerLoc) {
            printf("[#]");
        } else if (!(rooms[map[i]-1].flags & RFLAG_KNOWN)) {
            printf("   ");
        } else if (map[i] == 1) {
            printf("[<]");
        } else if (map[i] == numRooms) {
            printf("[>]");
        } else {
            printf("[ ]");
        }

        if ((i+1)%mapsz == 0) {
            printf("\n");
        }
    }
}

void describeRoom()
{
    int i, j, n, x, y;

    printf("Room %d-", currentLevel+1);
    if (map[playerLoc] <= 26) {
        printf("%c\n", map[playerLoc]+'A'-1);
    } else {
        printf("%c%c\n",
            map[playerLoc]+'A'-1, map[playerLoc]+'A'-1);
    }

    printf("  A %s %s.\n\n",
        desc1[rooms[map[playerLoc]-1].desc >> 4],
        desc2[rooms[map[playerLoc]-1].desc & 0x0f]);

    if (map[playerLoc] == 1) {
        printf("There are stairs leading up here.\n\n");
    } else if (map[playerLoc] == numRooms) {
        printf("There are stairs leading down here.\n\n");
    }

    if (rooms[map[playerLoc]-1].flags & 0x0f) {
        if (rooms[map[playerLoc]-1].flags & RFLAG_GOLD) {
            printf("There is some gold here.\n");
        }
        if (rooms[map[playerLoc]-1].flags & RFLAG_AMMO) {
            printf("There is some ammo here.\n");
        }
        if (rooms[map[playerLoc]-1].flags & RFLAG_REVOLVER) {
            printf("There is a revolver here.\n");
        }
        printf("\n");
    }

    printf("[Exits: ");
    n = 0;
    for (i = 0; i < 4; i++) {
        x = playerLoc%mapsz+dirs[i*2];
        y = playerLoc/mapsz+dirs[i*2+1];
        if (x < 0 || y < 0 || x >= mapsz || y >= mapsz) {
            continue;
        }
        if (map[y*mapsz+x]) {
            n++;
        }
    }

    if (map[playerLoc] == 1 || map[playerLoc] == numRooms) {
        n++;
    }

    j = 0;
    for (i = 0; i < 4; i++) {
        x = playerLoc%mapsz+dirs[i*2];
        y = playerLoc/mapsz+dirs[i*2+1];
        if (x < 0 || y < 0 || x >= mapsz || y >= mapsz) {
            continue;
        }
        if (map[y*mapsz+x]) {
            if (++j == n) {
                printf("%s", dirstrs[i]);
            } else {
                printf("%s, ", dirstrs[i]);
            }
        }
    }

    if (map[playerLoc] == 1) {
        printf("up");
    } else if (map[playerLoc] == numRooms) {
        printf("down");
    }

    printf("]\n");

    describeSkeletons();
}

void describeSkeletons() {
    int i, j, k, x, y;

    j = 0;
    for (i = 0; i < numSkeletons; i++) {
        if (skeletons[i].loc == playerLoc) {
            if (j++ == 0) {
                printf("\n");
            }
            if (skeletons[i].flags & SFLAG_MOVED) {
                printf("A skeleton enters the room!\n");
            } else {
                printf("A skeleton chatters its teeth!\n");
            }
        }
    }

    k = 0;
    for (i = 0; i < 4; i++) {
        x = playerLoc%mapsz+dirs[i*2];
        y = playerLoc/mapsz+dirs[i*2+1];
        if (x < 0 || y < 0 || x >= mapsz || y >= mapsz) {
            continue;
        }
        for (j = 0; j < numSkeletons; j++) {
            if (y*mapsz+x == skeletons[j].loc) {
                if (k++ == 0) {
                    printf("\n");
                }
                printf("You hear chattering to the %s...\n", dirstrs[i]);
            }
        }
    }
}
