#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "function.h"
#define COEFF_X 32
#define COEFF_Y 32

void dessinPixel(SDL_Renderer* ren, int x, int y, char type, SDL_Texture* texture);
void dessinMap(SDL_Renderer* ren, Map appartement);
hiddenSdl initSdl(Map appartement);
void eventSdl(hiddenSdl graphic, Map appartement);
void closeSdl(hiddenSdl graphic);
void dessinOpti(Point* pathPoint, int pathSize, hiddenSdl graphic);
int randomRange(int lower, int upper);
