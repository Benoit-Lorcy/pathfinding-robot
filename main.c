#include "graphique.h"
//programme inspiré du site https://khayyam.developpez.com/articles/algo/astar/

int main(int argc, char **argv) {
    //check arguments
    if (argc != 2) {
        printf("erreur, nombre incorrect d'arguments\n");
        exit(1);
    }

    //on initialise la map
    Map appartement = readMap(argv[1]);

    //choix du type d'affichage par l'utilisateur
    int saisie = 0;
    printf("Comment voulez vous executer le programme\n1.Text\n2.Graphic\nChoix :\n");
    scanf("%d", &saisie);

    if (saisie == 1) {
        //fonction qui gère le programme text
        textProg(appartement);
    } else if (saisie == 2) {
        //initialisation
        hiddenSdl graphic = initSdl(appartement);
        //boucle d'évenment
        eventSdl(graphic, appartement);
        //fermeture de la sdl
        closeSdl(graphic);
    }

    //on free l'appartement
    int i;
    for (i = 0; i < appartement.h; i++) {
        free(appartement.textMap[i]);
    }
    free(appartement.textMap);

    return 0;
}