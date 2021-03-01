#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LGMAX 150

struct Map {
    int w;
    int h;
    char** textMap;
};
typedef struct Map Map;

struct Point {
    int x;
    int y;
};
typedef struct Point Point;

// G cost différence entre la node de départ  et la node actuelle
// H cost différence entre le node actuelle et la node finale
// F cost somme entre G et F

struct Noeud {
    int x;
    int y;
    int g;
    int h;
    int f;
    Point parent;
};
typedef struct Noeud Noeud;

struct hiddenSdl {
    SDL_Window* win;
    SDL_Renderer* ren;
};
typedef struct hiddenSdl hiddenSdl;

void textProg(Map appartement);
Map readMap(char* filename);
void printTableau(Map appartement);
Map pathFinding(Map appartement, int x, int y, int* check, int textOrGraph, hiddenSdl graphic);
Point* init(Point* blockedList, int* blockedListSize, Map appartement, Point* start);
void Saisie(Map appartement, Point* goal);
int distance(int x1, int y1, int x2, int y2);
int presentInListOfNoeud(int x, int y, Noeud* listeNoeud, int* tabSize);
int presentInListOfPoint(int x, int y, Point* listeNoeud, int* tabSize);
Noeud* addAdjacent(Noeud* openList, int* openListSize, Point* blockedList, int* blockedListSize,
                   Noeud* closedList, int* closedListSize, Noeud n1, Point goal);
Noeud bestNoeud(Noeud* openList, int* openListSize);
Noeud* addClosedList(Noeud* closedList, int* closedListSize, Noeud n1);
Noeud* delOpenList(Noeud* openList, int* openListSize, Noeud n1);
Point* path(Noeud* closedList, int* closedListSize, Point goal, Point start, int* pathSize);
Map mapPath(Map appartement, Point* pathPoint, Point goal, int* pathSize);
void printProcess(Map appartement, Noeud* openList, int* openListSize, Noeud* closedList, int* closedListSize, Noeud current);
Map clearTableau(Map appartement);
void countSteps(Map appartement);
void printSteps(Point* pathPoint, int pathSize, Map appartement);
