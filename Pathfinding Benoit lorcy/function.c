#include "graphique.h"

//readMap
//on lit une map depuis un fichier text et on revoir un strutctmap
//entrée : chaine de caractère, retour Map
Map readMap(char* filename) {
    char text[LGMAX];
    char* substring = calloc(sizeof(char), LGMAX);
    Map appartement;
    FILE* fp;

    // on ouvre le fichier
    if ((fp = fopen(filename, "r")) == NULL) {
        printf("Erreur:...\n");
        exit(1);
    }

    // on regarde la première ligne pour trouver la largeur et la hauteur
    fgets(text, LGMAX, fp);

    // on trouve la hauteur et la largeur de la pièce
    int i = 0;
    while (text[i] != ':') {
        substring[i] = text[i];
        i++;
    }
    appartement.w = atoi(substring);
    i++;
    strcpy(substring, "");
    int j = 0;
    while (text[i] != '\n') {
        substring[j] = text[i];
        i++;
        j++;
    }
    appartement.h = atoi(substring);

    //lecture des lignes du texte
    char ligne[appartement.w];
    appartement.textMap = malloc(sizeof(char*) * appartement.h);
    for (i = 0; i < appartement.h; i++) {
        appartement.textMap[i] = malloc(sizeof(char) * (appartement.w + 2));
        fgets(ligne, appartement.w + 2, fp);
        for (j = 0; j < appartement.w; j++) {
            appartement.textMap[i][j] = ligne[j];
        }
    }

    //on ferme le fichier et on free
    fclose(fp);
    free(substring);
    return appartement;
}

//textProg
//execution du programme en textuel
//entrée : Map
void textProg(Map appartement) {
    int check = 0;
    Point goal;
    hiddenSdl temp;
    printTableau(appartement);
    while (check != 1) {
        Saisie(appartement, &goal);
        appartement = pathFinding(appartement, goal.x, goal.y, &check, 1, temp);
        if (check == 0) {
            printTableau(appartement);
            countSteps(appartement);
            appartement = clearTableau(appartement);
        }
    }
}

//printTableau
//affiche le tableau dans la console
//entrée : Map
void printTableau(Map appartement) {
    printf("%d:%d\n", appartement.w, appartement.h);
    int i, j;
    for (i = 0; i < appartement.h; i++) {
        for (j = 0; j < appartement.w; j++) {
            printf("%c", appartement.textMap[i][j]);
        }
        printf("\n");
    }
}

//pathFinding
//fonction principale qui cherche le chemin le plus court et qui affiche le déplacement du robot selon le mode d'affichage choisit
//entrée : appartement, x, y, textOrGraph, hiddenSdl, entrée/sortie : check, retour : Map
Map pathFinding(Map appartement, int x, int y, int* check, int textOrGraph, hiddenSdl graphic) {
    //3 listes: liste bloquée, liste fermée et liste ouverte
    Point* blockedList = NULL;
    int blockedListSize = 0;
    Noeud* closedList = NULL;
    int closedListSize = 0;
    Noeud* openList = NULL;
    int openListSize = 0;

    //on déclare le point de départ, d'arrivée et le noeud acutel
    Point start;
    Noeud current;
    Point goal;

    //initilialisation de la liste bloqué
    blockedList = init(blockedList, &blockedListSize, appartement, &start);
    int flag, i;
    goal.x = x;
    goal.y = y;

    //on vérifie le x et le y si check = 1 on arrêtre le prgramme et si check = -1 on redemandemande les coordonées
    for (i = 0; i < blockedListSize; i++) {
        if (x == blockedList[i].x && y == blockedList[i].y) {
            *check = -1;
            return appartement;
        }
    }
    if (x > appartement.w || x < 0 || y > appartement.h || y < 0) {
        *check = 1;
        return appartement;
    }

    //initialisation du point courrant
    current.x = start.x;
    current.y = start.y;
    current.g = 0;
    current.h = distance(start.x, start.y, goal.x, goal.y);
    current.f = current.h;
    closedList = addClosedList(closedList, &closedListSize, current);
    openList = addAdjacent(openList, &openListSize, blockedList, &blockedListSize, closedList, &closedListSize, current, goal);

    //boucle qui trouve le meilleur chemin
    //condition : tant que le point courant n'est pas le point de départ:
    while (!((current.x == goal.x) && (current.y == goal.y))) {
        //on checher le meilleur noeud dans la liste ouverte, il devient le nouveau point courrant
        current = bestNoeud(openList, &openListSize);
        //on le met dans la liste fermée
        closedList = addClosedList(closedList, &closedListSize, current);
        //on le retire de la liste ouverte
        openList = delOpenList(openList, &openListSize, current);
        //on l'ajoute ses voisins a la liste ouverte
        openList = addAdjacent(openList, &openListSize, blockedList, &blockedListSize, closedList, &closedListSize, current, goal);
    }

    //reconstitution du chemin
    Point* pathPoint = NULL;
    int pathSize = 0;
    pathPoint = path(closedList, &closedListSize, goal, start, &pathSize);

    //on affiche le robot qui se déplace case par case selon le mode d'affichage
    if (textOrGraph == 1) {
        printSteps(pathPoint, pathSize, appartement);
    } else if (textOrGraph == 2) {
        dessinOpti(pathPoint, pathSize, graphic);
    }

    appartement = mapPath(appartement, pathPoint, goal, &pathSize);

    //on free toutes les listes
    free(blockedList);
    free(closedList);
    free(openList);
    free(pathPoint);

    return appartement;
}

//init
//intialistation de la closed map(obstacles) et du point de départ
//entrée : Point* blockedList, Map appartement, Entrée/ Sortie : int* blockedListSize, Point* start, retour : Point* blockedList
Point* init(Point* blockedList, int* blockedListSize, Map appartement, Point* start) {
    int i, j;
    for (i = 0; i < appartement.h; i++) {
        for (j = 0; j < appartement.w; j++) {
            if (appartement.textMap[i][j] == 'x') {
                (*blockedListSize)++;
                blockedList = realloc(blockedList, (*blockedListSize) * sizeof(Point));
                blockedList[*blockedListSize - 1].x = j;
                blockedList[*blockedListSize - 1].y = i;
            }

            if (appartement.textMap[i][j] == 'R') {
                start->x = j;
                start->y = i;
            }
        }
    }
    return blockedList;
}

//Saisie
//fonction pour la saisie du point d'arrivée en textuel
//entrée : Map appartement, entrée sortie : Point* goal
void Saisie(Map appartement, Point* goal) {
    int i;
    Point test;  // point de test

    printf("entrez la destination (en dehors de la map pour arrêter le programme)\nx :");
    scanf("%d", &(test.x));
    printf("y :");
    scanf("%d", &(test.y));
    (test.x)--;
    (test.y)--;

    goal->x = test.x;
    goal->y = test.y;
    return;
}

// distance
// distance entre deux points
// entrée : int x1, int y1, int x2, int y2, return : int
int distance(int x1, int y1, int x2, int y2) {
    return (abs(x1 - x2) + abs(y1 - y2));
}

// presentInListOfNoeud
// on vérifier si un élement est présent dans une liste de Noeud
// entrée : int x, int y, Noeud* listeNoeud, int* tabSize, sortie : int
int presentInListOfNoeud(int x, int y, Noeud* listeNoeud, int* tabSize) {
    int i;
    int flag = 0;
    for (i = 0; i < *tabSize; i++) {
        if (x == listeNoeud[i].x && y == listeNoeud[i].y) {
            flag = 1;
        }
    }
    return flag;
}

// presentInListOfPoint
// on vérifier si un élement est présent dans une liste de Point
// entrée : int x, int y, Noeud* listeNoeud, int* tabSize, sortie : int
int presentInListOfPoint(int x, int y, Point* listePoint, int* tabSize) {
    int i;
    int flag = 0;
    for (i = 0; i < *tabSize; i++) {
        if (x == listePoint[i].x && y == listePoint[i].y) {
            flag = 1;
        }
    }
    return flag;
}

// addAdjacent
// on essaye d'ajouter les noeuds adjacents a la liste ouverte
// entrée : Noeud* closedList, int* closedListSize, Noeud n1, Point goal, Point* blockedList, int* blockedListSize
// entrée /sortie : Noeud* openList, int* openListSize,
// return Noeud*
Noeud* addAdjacent(Noeud* openList, int* openListSize, Point* blockedList, int* blockedListSize,
                   Noeud* closedList, int* closedListSize, Noeud n1, Point goal) {
    int i;
    int index = 0;

    // liste des noueds nord sud est west
    Noeud adjacent[4];
    adjacent[0] = n1;
    adjacent[1] = n1;
    adjacent[2] = n1;
    adjacent[3] = n1;
    adjacent[0].x = n1.x - 1;
    adjacent[1].x = n1.x + 1;
    adjacent[2].y = n1.y - 1;
    adjacent[3].y = n1.y + 1;

    //boucle passant pas les 4 points
    for (i = 0; i < 4; i++) {
        //si le noeud est présent dans la liste fermée on passe
        if (presentInListOfPoint(adjacent[i].x, adjacent[i].y, blockedList, blockedListSize)) continue;

        //si le noeud n'est pas présent dans la liste fermée :
        if (!presentInListOfNoeud(adjacent[i].x, adjacent[i].y, closedList, closedListSize)) {
            // G cost différence entre la node de départ  et la node actuelle
            // G cost peu aussi etre le cout du parent + la distance jusqu'au parent
            // H cost différence entre le node actuelle et la node finale
            // F cost somme entre G et F
            adjacent[i].g = n1.g + 1;
            adjacent[i].h = distance(adjacent[i].x, adjacent[i].y, goal.x, goal.y);
            adjacent[i].f = adjacent[i].g + adjacent[i].h;
            adjacent[i].parent.x = n1.x;
            adjacent[i].parent.y = n1.y;

            //si le noeud est
            if (presentInListOfNoeud(adjacent[i].x, adjacent[i].y, openList, openListSize)) {
                int j;
                int flag = 0;
                for (j = 0; j < *openListSize; j++) {
                    if (adjacent[i].x == openList[j].x && adjacent[i].y == openList[j].y) {
                        break;
                    }
                }

                if (adjacent[i].f < openList[j].f) {
                    openList[j] = adjacent[i];
                    //printf("meilleur point trouvé");
                }

            } else {
                (*openListSize)++;
                openList = realloc(openList, sizeof(Noeud) * (*openListSize + 1));
                openList[*openListSize - 1] = adjacent[i];
            }
        }
    }
    return openList;
}

// bestNoeud
// fonction qui détermine le meilleur noeud dans les noeuds ouverts
// entrée: openlist, entrée/sortie: openListSize, return Noeud*
Noeud bestNoeud(Noeud* openList, int* openListSize) {
    int bestF, i;
    bestF = openList[0].f;
    int flag = 0;
    for (i = 0; i < (*openListSize); i++) {
        if (bestF > openList[i].f) {
            bestF = openList[i].f;
            flag = i;
        }
    }
    return openList[flag];
}

// addClosedList
// fonction pour ajouter a la liste fermée (retire en même temps de la liste ouverte)
// entrée : Noeud* closedList, Noeud n1, entrée/sortie : closedListSize , return Noeud*
Noeud* addClosedList(Noeud* closedList, int* closedListSize, Noeud n1) {
    (*closedListSize)++;
    closedList = realloc(closedList, (*closedListSize) * sizeof(Noeud));
    closedList[*closedListSize - 1] = n1;
    return closedList;
}

//delOpenList
//retire un élément de la liste ouverte
//entrée: Noeud* openList, Noeud n1, entrée/Sortie : openListSize, return, Noeud *
Noeud* delOpenList(Noeud* openList, int* openListSize, Noeud n1) {
    int i;
    int flag = 0;
    for (i = 0; i < *openListSize; i++) {
        if (n1.x == openList[i].x && n1.y == openList[i].y) {
            flag = 1;
            break;
        }
    }

    if (flag) {
        openList[i] = openList[*openListSize - 1];
        *openListSize = *openListSize - 1;
    } else {
        //printf("impossible de supprimer l'élément, il n'est pas dans la liste ouverte");
    }

    return openList;
}

// path
// retouve le chemin en cherchant le parent de chaque point
// entrée : Noeud* closedList, int* closedListSize, Point goal, Point start, entrée/sortie : pathSize, return : Point *
Point* path(Noeud* closedList, int* closedListSize, Point goal, Point start, int* pathSize) {
    Point* pathPoints = NULL;
    int i = 0;
    int j;

    pathPoints = malloc(sizeof(Point));
    pathPoints[0] = goal;

    while (!(pathPoints[i].x == start.x && pathPoints[i].y == start.y)) {
        for (j = 0; j < *closedListSize; j++) {
            if (pathPoints[i].x == closedList[j].x && pathPoints[i].y == closedList[j].y) {
                break;
            }
        }
        i++;
        pathPoints = realloc(pathPoints, (i + 1) * sizeof(Point));
        pathPoints[i].x = closedList[j].parent.x;
        pathPoints[i].y = closedList[j].parent.y;
    }

    *pathSize = i + 1;
    return pathPoints;
}

// mapPath
// affiche le chemin final du robot
// entrée : Map appartement, Point* pathPoint, Point goal, int* pathSize, return Map
Map mapPath(Map appartement, Point* pathPoint, Point goal, int* pathSize) {
    int i, j;
    for (i = 0; i < *pathSize; i++) {
        appartement.textMap[pathPoint[i].y][pathPoint[i].x] = '*';
    }

    appartement.textMap[goal.y][goal.x] = 'R';

    return appartement;
}

// countSteps
// affiche dans la console le nombre de cases pour arriver au but
// entrée : Map appartement
void countSteps(Map appartement) {
    int i, j;
    int count = 0;
    for (i = 0; i < appartement.h; i++) {
        for (j = 0; j < appartement.w; j++) {
            if (appartement.textMap[i][j] == '*') {
                count++;
            }
        }
    }
    printf("nombre d'étapes : %d\n", count);
    return;
}

// clearTableau
// netoie la map en retirant le chemin laissé par le robot
// entrée : Map apartement, return : Map
Map clearTableau(Map appartement) {
    int i, j;
    int count = 0;
    for (i = 0; i < appartement.h; i++) {
        for (j = 0; j < appartement.w; j++) {
            if (appartement.textMap[i][j] == '*') {
                appartement.textMap[i][j] = ' ';
            }
        }
    }
    return appartement;
}

// printSteps
// affiche le chemin point par point du robot en mode text
// entrée :Point* pathPoint, int pathSize, Map appartement
void printSteps(Point* pathPoint, int pathSize, Map appartement) {
    int i;
    for (i = 0; i < pathSize; i++) {
        appartement.textMap[pathPoint[pathSize - i - 1].y][pathPoint[pathSize - i - 1].x] = 'R';

        if (i > 0) {
            appartement.textMap[pathPoint[pathSize - i].y][pathPoint[pathSize - i].x] = '*';
        }
        printTableau(appartement);
        usleep(300000);
    }
}

//une fonction qui permet de
/*
void printProcess(Map appartement, Noeud* openList, int* openListSize, Noeud* closedList, int* closedListSize, Noeud current) {
    int i;
    for (i = 0; i < *closedListSize; i++) {
        appartement.textMap[closedList[i].y][closedList[i].x] = 'o';
    }

    for (i = 0; i < *openListSize; i++) {
        appartement.textMap[openList[i].y][openList[i].x] = '~';
    }

    appartement.textMap[current.y][current.x] = 'd';

    printTableau(appartement);
    return;
}
*/