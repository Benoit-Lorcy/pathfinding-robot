#include "graphique.h"

// initSdl
// fonction qui initialise la sdl
// entrée : Map appartement, return : hiddenSdl
hiddenSdl initSdl(Map appartement) {
    hiddenSdl graphic;
    SDL_Window *win = 0;
    SDL_Renderer *ren = 0;

    /* Initialisation de la SDL. Si ça se passe mal, on quitte */
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "Erreur initialisation\n");
        exit(1);
    }

    /* Création de la fenêtre et du renderer */
    SDL_CreateWindowAndRenderer(appartement.w * COEFF_X, appartement.h * COEFF_Y, 0, &win, &ren);  // SDL_WINDOW_SHOWN|SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC,&win,&ren);
    SDL_SetWindowTitle(win, "pathFinding");

    if (!win || !ren) {
        fprintf(stderr, "Erreur à la création des fenêtres\n");
        SDL_Quit();
        exit(1);
    }
    /* Affichage du fond noir */
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderClear(ren);
    SDL_RenderPresent(ren);

    graphic.ren = ren;
    graphic.win = win;
    return graphic;
}

// eventSdl
// fonction qui gère l'execution du programme en mode graphique
// entrée : hiddenSdl graphic, Map appartement
void eventSdl(hiddenSdl graphic, Map appartement) {
    SDL_Event event;
    int quit = 0;
    int check = 0;
    dessinMap(graphic.ren, appartement);
    while (!quit) {
        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_MOUSEBUTTONDOWN:
                dessinMap(graphic.ren, appartement);
                appartement = pathFinding(appartement, (int)event.button.x / COEFF_X, (int)event.button.y / COEFF_Y, &check, 2, graphic);
                if (!(check == -1)) {
                    dessinMap(graphic.ren, appartement);
                    countSteps(appartement);
                    appartement = clearTableau(appartement);
                }
                check = 0;
                break;
        }
    }
}

// closeSdl
// fonction qui permet de fermer correctement la sdl
// entrée : hiddenSdl graphic
void closeSdl(hiddenSdl graphic) {
    SDL_DestroyRenderer(graphic.ren);
    SDL_DestroyWindow(graphic.win);
    SDL_Quit();
    return;
}

// dessinMap
// fonction qui déssine toute la map en graphique
// entrée : SDL_Renderer *ren, Map appartement
void dessinMap(SDL_Renderer *ren, Map appartement) {
    int colr, colg, colb;
    SDL_Rect r;

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

    int i, j;
    for (i = 0; i < appartement.h; i++) {
        for (j = 0; j < appartement.w; j++) {
            if (appartement.textMap[i][j] == 'x') {
                r.x = j * COEFF_X;
                r.y = i * COEFF_Y;
                r.w = COEFF_X;
                r.h = COEFF_Y;
                colr = 90;
                colg = 31;
                colb = 23;
                SDL_SetRenderDrawColor(ren, colr, colg, colb, 250);
                SDL_RenderFillRect(ren, &r);
            }
            if (appartement.textMap[i][j] == ' ') {
                r.x = j * COEFF_X;
                r.y = i * COEFF_Y;
                r.w = COEFF_X;
                r.h = COEFF_Y;
                colr = 255;  //251;
                colg = 255;  //209;
                colb = 255;
                SDL_SetRenderDrawColor(ren, colr, colg, colb, 255);
                SDL_RenderFillRect(ren, &r);

                if ((i % 2 == 0 && j % 2 == 0) || (i % 2 == 1 && j % 2 == 1)) {
                    colr = 245;  //colr = rand() * 255;  //251;
                    colg = 233;  //colg = rand() * 255;  //209;
                    colb = 229;  //colb = rand() * 255;

                    SDL_SetRenderDrawColor(ren, colr, colg, colb, 255);
                } else {
                    colr = 250;  //colr = rand() * 255;  //251;
                    colg = 250;  //colg = rand() * 255;  //209;
                    colb = 250;  //colb = rand() * 255;

                    SDL_SetRenderDrawColor(ren, colr, colg, colb, 255);
                }

                SDL_RenderFillRect(ren, &r);
            }
            if (appartement.textMap[i][j] == 'R') {
                SDL_Surface *image = SDL_LoadBMP("robot.bmp");
                SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, image);

                r.x = j * COEFF_X;
                r.y = i * COEFF_Y;
                r.w = COEFF_X;
                r.h = COEFF_Y;
                SDL_RenderCopy(ren, texture, NULL, &r);
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(image);
            }
            if (appartement.textMap[i][j] == '*') {
                r.x = j * COEFF_X;
                r.y = i * COEFF_Y;
                r.w = COEFF_X;
                r.h = COEFF_Y;
                colr = 251;
                colg = 209;
                colb = 0;
                SDL_SetRenderDrawColor(ren, colr, colg, colb, 100);
                SDL_RenderFillRect(ren, &r);
            }
        }
    }
    SDL_RenderPresent(ren);
}

// dessinMap
// fonction qui dessine un pixel
// entrée : SDL_Renderer *ren, Map appartement
void dessinPixel(SDL_Renderer *ren, int x, int y, char type, SDL_Texture *texture) {
    int colr, colg, colb;
    SDL_Rect r;

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

    if (type == 'c') {
        r.x = x * COEFF_X;
        r.y = y * COEFF_Y;
        r.w = COEFF_X;
        r.h = COEFF_Y;
        colr = rand() * 255;  //251;
        colg = rand() * 255;  //209;
        colb = rand() * 255;  //0;
        SDL_SetRenderDrawColor(ren, colr, colg, colb, 255);
        SDL_RenderFillRect(ren, &r);
    }

    if (type == 'r') {
        r.x = x * COEFF_X;
        r.y = y * COEFF_Y;
        r.w = COEFF_X;
        r.h = COEFF_Y;
        SDL_RenderCopy(ren, texture, NULL, &r);
    }
    SDL_RenderPresent(ren);
}

// dessinOpti
// fonction qui prend un ensemble de points et qui les dessines
// contrairement a la fonction dessinMap elle permet de ne desiné que les éléments nécessaires
// entrée : Point *pathPoint, int pathSize, hiddenSdl graphic
void dessinOpti(Point *pathPoint, int pathSize, hiddenSdl graphic) {
    SDL_Surface *image = SDL_LoadBMP("robot.bmp");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(graphic.ren, image);
    int i;
    for (i = 0; i < pathSize; i++) {
        dessinPixel(graphic.ren, pathPoint[pathSize - i - 1].x, pathPoint[pathSize - i - 1].y, 'r', texture);

        if (i > 0) {
            dessinPixel(graphic.ren, pathPoint[pathSize - i].x, pathPoint[pathSize - i].y, 'c', texture);
        }

        SDL_Delay(50);
    }
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(image);
}

//randomRange
//fonction qui renvoyer un nmobre aléatoire entre deux bornes
//entrée :int lower, int upper
int randomRange(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}