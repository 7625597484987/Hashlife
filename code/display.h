#ifndef DISPLAY_H
#define DISPLAY_H
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "functions.h"
#define TAILLE_CARRE 10

void AfficherNode(unsigned long nod, unsigned int couche );
Node InsererXY(Node nod, int x, int y, int niveau, int puissance_de_2);
unsigned long ArbreFromTableau( TabCoord tableau, unsigned int* x_minimum, unsigned int* y_minimum, unsigned int* couche );
TabCoord LireCoordonneesInitiales(FILE* fichier);
void TraiterNode(unsigned long node, unsigned int couche, TabCoord* tableau, unsigned int x_min, unsigned int y_min);
TabCoord TableauFromArbre(unsigned long adresse_node, unsigned int couche, unsigned int x_min, unsigned int y_min);
void RenderArray(SDL_Renderer* render, TabCoord tableau);
void RenderNode(SDL_Renderer* render, unsigned long nod, unsigned int couche, unsigned int x, unsigned int y, SDL_Rect* rectangle);
void RenderNodes(SDL_Renderer* render, unsigned long nod, unsigned int couche, unsigned int offset_x, unsigned int offset_y);
void AfficherTableau(TabCoord tableau);
TabCoord PlacerCellules(SDL_Renderer* render, unsigned int taille_carre);
#endif