#include <string.h>
#include "./code/display.h"

// #############################################################################################################""
// on peut sans doute optimiser les fonctions en partant du principe que: la racine étant de taille n
// en 1 "itération", la racine peut seulement augmenter sa taille de 1
// donc le cache seulement augmenter sa taille maximale de 1
// #############################################################################################################
// On peut trier le cache


int main() {
unsigned long racine;
unsigned int couche_racine;
unsigned int couche_precedente;
char affichage_iterations[20];
int i;
unsigned int x_min;
unsigned int y_min;

unsigned long long compteur_iterations = 0;

//On crée les feuilles, dans le cache
    cache = (Node**)malloc(sizeof(Node*));
    cache[0] = (Node*)malloc(2 * sizeof(Node));
//cellule unité morte
    cache[0][0].couche = 0;
    cache[0][0].indice_no = 0;
    cache[0][0].indice_ne = 0;
    cache[0][0].indice_so = 0;
    cache[0][0].indice_se = 0;
//cellule unité vivante
    cache[0][1].couche = 0;
    cache[0][1].indice_no = 1;
    cache[0][1].indice_ne = 1;
    cache[0][1].indice_so = 1;
    cache[0][1].indice_se = 1;
    dimensions_cache = (unsigned long long*)malloc(sizeof(unsigned long long));
    dimensions_cache[0] = 2;
    taille_cache = 0;
// 0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0 Affichage 0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O
if (SDL_Init(SDL_INIT_VIDEO) == -1) {//initialisation de SDL
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    exit(EXIT_FAILURE);
}
if(TTF_Init() == -1)
{
    SDL_Log("Unable to initialize SDL: %s", TTF_GetError());
    exit(EXIT_FAILURE);
}
//on ouvre la fenetre
SDL_Window* fenetre_affichage = SDL_CreateWindow("Idk", 1, 1, 1600, 800, SDL_WINDOW_SHOWN);
SDL_Renderer* renderer = SDL_CreateRenderer(fenetre_affichage,-1,SDL_RENDERER_SOFTWARE);
TTF_Font *police = TTF_OpenFont("./else/Roboto-Thin.ttf", 30);
SDL_Surface *text;
SDL_Texture* message;
SDL_Rect zone_affichage = {1400,0,0,30};
SDL_Color rouge;
rouge.a = (Uint8)255;
rouge.r = (Uint8)255;
rouge.g = (Uint8)255;
rouge.b = (Uint8)255;
// 0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O

//TabCoord tableau_coordonnees = LireCoordonneesInitiales( fopen("./else/coord.txt","r") );
TabCoord tableau_coordonnees = PlacerCellules( renderer, 10 );
AfficherTableau(tableau_coordonnees);

racine = ArbreFromTableau(tableau_coordonnees, &x_min, &y_min, &couche_racine);
LibererTableau(tableau_coordonnees);
RenderArray(renderer, TableauFromArbre(racine, couche_racine, x_min, y_min));
SDL_Delay(1000);
couche_precedente = couche_racine;

//itérations
for (i=0 ; i < 4 ; i++) {
    // la node qui nous intéresse doit etre entourée de 0, et celle sur laquelle on travaille est: la node entourée de 0, puis entourée de 0

    racine = Entourer( cache[couche_racine][racine] ) ;
    couche_racine += 1;
    if ( racine == -1 ) {
        printf("On arrete tout\n");
        break;
    }
    racine = Entourer( cache[couche_racine][racine] ) ;
    couche_racine += 1;
    if ( racine == -1 ) {
        printf("On arrete tout\n");
        break;
    }
    tableau_coordonnees = TableauFromArbre(racine, couche_racine, x_min, y_min);

    compteur_iterations += Puissance2(couche_racine - 2);
    
    racine = CalculerSuccesseur( couche_racine, racine );
    couche_racine -= 1;

    racine = Detourer( cache[couche_racine][racine], &couche_racine );

// 0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0 Affichage O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O

    if ( couche_precedente < couche_racine ) {
        // l'expansion positive est fixée à chaque itération, cela évite des calculs de différences entre itérations
        x_min -= Puissance2( couche_precedente - 1 );
        y_min -= Puissance2( couche_precedente - 1 );
    }
    else if ( couche_precedente < couche_racine ) {
        // l'expansion négative, par contre, peut varier
        x_min += Puissance2( couche_precedente - 1 ) - Puissance2( couche_racine - 1 );
        y_min += Puissance2( couche_precedente - 1 ) - Puissance2( couche_racine - 1 );
    }
    couche_precedente = couche_racine;
    tableau_coordonnees = TableauFromArbre(racine, couche_racine, x_min, y_min);


    RenderArray(renderer, tableau_coordonnees);
    sprintf(affichage_iterations, "%llu", compteur_iterations);
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
    zone_affichage.w = 18*strlen(affichage_iterations);
    SDL_RenderFillRect(renderer, &zone_affichage);
	text = TTF_RenderText_Solid(police, affichage_iterations, rouge);
	message = SDL_CreateTextureFromSurface(renderer, text);
	SDL_RenderCopy(renderer, message, NULL, &zone_affichage);
    SDL_RenderPresent(renderer);
    SDL_Delay(1000);

// 0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O
    LibererTableau(tableau_coordonnees);
}

// 0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O
// libération de la mémoire
    for (i=3 ; i < taille_cache ; i++) {
        free(cache[i]);
    }
    free(dimensions_cache);
    free(cache);

    TTF_CloseFont(police);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(fenetre_affichage);
    SDL_Quit();
// 0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O0O
}