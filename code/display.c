#include "display.h"

SDL_Rect carre = {0, 0, TAILLE_CARRE, TAILLE_CARRE};
int N_carres_X = 1600/TAILLE_CARRE;
int N_carres_Y = 800/TAILLE_CARRE;

void AfficherNode(unsigned long nod, unsigned int couche ) {
    if ( couche == 0 ) {
        printf("%lu ", nod);
    }
    else {
        AfficherNode(cache[ couche ][ nod ].indice_no, couche - 1);
        AfficherNode(cache[ couche ][ nod ].indice_ne, couche - 1);
        AfficherNode(cache[ couche ][ nod ].indice_so, couche - 1);
        AfficherNode(cache[ couche ][ nod ].indice_se, couche - 1);
    }
}

// \brief la node renvoyée n'est pas cachée
// \brief son indice futur vaut -1
Node InsererXY(Node nod, int x, int y, int niveau, int puissance_de_2) {
    //printf("\t\t\t couches: racine %d\n",nod.couche);
    //printf("\t\t\t racine_no %d, racine_ne %d, racine_so %d, racine_se %d\n",nod.no->couche,  nod.ne->couche,  nod.so->couche,  nod.se->couche );
    if ( niveau == 1 ) {
        if ( x >= 1 ) {
            if ( y >= 1 ) {
                nod.indice_se = 1;
            }
            else {
                nod.indice_ne = 1;
            }
        }
        else {
            if ( y >= 1 ) {
                nod.indice_so  = 1;
            }
            else {
                nod.indice_no = 1;
            }
        }
    }
    else {
        if ( x >= puissance_de_2/2 ) {
            if ( y >= puissance_de_2/2 ) {
                nod.indice_se = VerifierCache( InsererXY( cache[niveau - 1][nod.indice_se] , x - puissance_de_2/2 , y - puissance_de_2/2 , niveau - 1 , puissance_de_2/2) );
            }
            else {
                nod.indice_ne = VerifierCache( InsererXY( cache[niveau - 1][nod.indice_ne] , x - puissance_de_2/2 , y, niveau - 1 , puissance_de_2/2) );
            }
        }
        else {
            if ( y >= puissance_de_2/2 ) {
                nod.indice_so = VerifierCache( InsererXY( cache[niveau - 1][nod.indice_so] , x , y - puissance_de_2/2 , niveau - 1 , puissance_de_2/2) );
            }
            else {
                nod.indice_no = VerifierCache( InsererXY( cache[niveau - 1][nod.indice_no] , x , y , niveau - 1 , puissance_de_2/2) );
            }
        }
    }
    nod.couche = niveau;
    nod.indice_futur = 18446744073709551615ul;// -1 en unsigned
    return(nod);
}

// \brief Initialise les nodes à partir du tableau contenant les coordonnées des cellules initiales
// La node renvoyée est déjà cachée
unsigned long ArbreFromTableau( TabCoord tableau, unsigned int* x_minimum, unsigned int* y_minimum, unsigned int* couche ) {
    int i;
    *x_minimum = tableau.tab[0][0];
    int x_maximum = tableau.tab[0][0];
    *y_minimum = tableau.tab[0][1];
    int y_maximum = tableau.tab[0][1];
    for (i=1 ; i < tableau.longueur ; i++ ) {
        if ( tableau.tab[i][0] < *x_minimum ) {
            *x_minimum = tableau.tab[i][0];
        }
        else if ( tableau.tab[i][0] > x_maximum ) {
            x_maximum = tableau.tab[i][0];
        }
        if ( tableau.tab[i][1] < *y_minimum ) {
            *y_minimum = tableau.tab[i][1];
        }
        else if ( tableau.tab[i][1] > y_maximum ) {
            y_maximum = tableau.tab[i][1];
        }
    }
    int dimension = max( x_maximum - *x_minimum , y_maximum - *y_minimum);
    *couche = 0;
    int puissance_2 = 1;
    while ( puissance_2 <= dimension ) {
        *couche += 1;
        puissance_2 = 2 * puissance_2;
    }

    CreerZeros( *couche );

    unsigned long retour = 0;

    for (i=0 ; i < tableau.longueur ; i++ ) {
        retour = VerifierCache( InsererXY( cache[*couche][retour], tableau.tab[i][0] - *x_minimum, tableau.tab[i][1] - *y_minimum, *couche, puissance_2) );
    }
    return retour;
}

TabCoord LireCoordonneesInitiales(FILE* fichier) {
    TabCoord retour;
    fscanf(fichier, "#%u\n", &retour.longueur );
    retour.tab = (unsigned int**)malloc( (retour.longueur) * sizeof(int*) );
    for (int i = 0 ; i < retour.longueur ; i++ ) {
        retour.tab[i] = (unsigned int*)malloc(2*sizeof(int));
        fscanf(fichier, "%u,%u\n", retour.tab[i], retour.tab[i] + 1 );
    }
    return retour;
}

void TraiterNode(unsigned long node, unsigned int couche, TabCoord* tableau, unsigned int x_min, unsigned int y_min) {
    if ( couche == 1 ) {
        if ( cache[couche][node].indice_no == 1 ) {
            tableau->longueur += 1;
            tableau->tab = realloc(tableau->tab, (tableau->longueur) * sizeof(unsigned int*) );
            tableau->tab[tableau->longueur - 1] = malloc(2*sizeof(unsigned int)) ;
            tableau->tab[tableau->longueur - 1][0] = x_min;
            tableau->tab[tableau->longueur - 1][1] = y_min;
        }
        if ( cache[couche][node].indice_ne == 1 ) {
            tableau->longueur += 1;
            tableau->tab = realloc(tableau->tab, (tableau->longueur) * sizeof(unsigned int*) );
            tableau->tab[tableau->longueur - 1] = malloc(2*sizeof(unsigned int)) ;
            tableau->tab[tableau->longueur - 1][0] = x_min + 1;
            tableau->tab[tableau->longueur - 1][1] = y_min;
        }
        if ( cache[couche][node].indice_so == 1 ) {
            tableau->longueur += 1;
            tableau->tab = realloc(tableau->tab, (tableau->longueur) * sizeof(unsigned int*) );
            tableau->tab[tableau->longueur - 1] = malloc(2*sizeof(unsigned int)) ;
            tableau->tab[tableau->longueur - 1][0] = x_min;
            tableau->tab[tableau->longueur - 1][1] = y_min + 1;
        }
        if ( cache[couche][node].indice_se == 1 ) {
            tableau->longueur += 1;
            tableau->tab = realloc(tableau->tab, (tableau->longueur) * sizeof(unsigned int*) );
            tableau->tab[tableau->longueur - 1] = malloc(2*sizeof(unsigned int)) ;
            tableau->tab[tableau->longueur - 1][0] = x_min + 1;
            tableau->tab[tableau->longueur - 1][1] = y_min + 1;
        }
    }
    else {
        unsigned int p_2 = Puissance2(couche - 1);
        if ( cache[couche][node].indice_no != 0 ) {
            TraiterNode( cache[couche][node].indice_no, couche - 1, tableau, x_min, y_min );
        }
        if ( cache[couche][node].indice_ne != 0 ) {
            TraiterNode( cache[couche][node].indice_ne, couche - 1, tableau, x_min + p_2, y_min );
        }
        if ( cache[couche][node].indice_so != 0 ) {
            TraiterNode( cache[couche][node].indice_so, couche - 1, tableau, x_min, y_min + p_2 );
        }
        if ( cache[couche][node].indice_se != 0 ) {
            TraiterNode( cache[couche][node].indice_se, couche - 1, tableau, x_min + p_2, y_min + p_2);
        }
    }
}

TabCoord TableauFromArbre(unsigned long adresse_node, unsigned int couche, unsigned int x_min, unsigned int y_min) {
    TabCoord tableau;
    tableau.longueur = 0;
    tableau.tab = NULL;

    TraiterNode(adresse_node, couche, &tableau, x_min, y_min);

    return tableau;
}

void RenderArray(SDL_Renderer* render, TabCoord tableau) {
    SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(render);

    SDL_SetRenderDrawColor(render, 255, 255, 255, SDL_ALPHA_OPAQUE);
    for (int i=0 ; i < tableau.longueur; i++) {
// Si le carré peut être affiché à l'écran
        if ( tableau.tab[i][0] >=0 && tableau.tab[i][0] < N_carres_X && tableau.tab[i][1] >=0 && tableau.tab[i][1] < N_carres_Y ) {

            carre.x = TAILLE_CARRE*tableau.tab[i][0];
            carre.y = TAILLE_CARRE*tableau.tab[i][1];
            SDL_RenderFillRect(render, &carre);
        }
    }
    SDL_RenderPresent(render);
}

void RenderNode(SDL_Renderer* render, unsigned long nod, unsigned int couche, unsigned int x, unsigned int y, SDL_Rect* rectangle) {
    if (couche == 0) {
        rectangle->x = 10*x;
        rectangle->y = 10*y;
        if ( nod == 0 ) {
            SDL_SetRenderDrawColor(render, 100, 100, 100, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(render, rectangle);
        }
        else {
            SDL_SetRenderDrawColor(render, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(render, rectangle);
        }
        SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(render, rectangle);
    }
    else {
        RenderNode( render, cache[couche][nod].indice_no, couche-1 , x , y , rectangle);
        RenderNode( render, cache[couche][nod].indice_ne, couche-1 , x + Puissance2(couche - 1) , y , rectangle);
        RenderNode( render, cache[couche][nod].indice_so, couche-1 , x, y + Puissance2(couche - 1), rectangle );
        RenderNode( render, cache[couche][nod].indice_se, couche-1 , x + Puissance2(couche - 1) , y + Puissance2(couche - 1), rectangle);
    }
}

void RenderNodes(SDL_Renderer* render, unsigned long nod, unsigned int couche, unsigned int offset_x, unsigned int offset_y) {
    SDL_SetRenderDrawColor(render, 100, 100, 100, SDL_ALPHA_OPAQUE);
    SDL_Rect rec = {0, 0, 10, 10};
    RenderNode(render, nod, couche, offset_x, offset_y, &rec);
    SDL_RenderPresent(render);
}

void AfficherTableau(TabCoord tableau) {
    for (int i = 0 ; i < tableau.longueur ; i++) {
        printf(" %d, %d \n", tableau.tab[i][0], tableau.tab[i][1]);
    }
}

TabCoord PlacerCellules(SDL_Renderer* render, unsigned int taille_carre) {
    TabCoord tableau;
    tableau.longueur = -1;
    SDL_Event event;
    SDL_PollEvent(&event);
    int mouseX = event.motion.x;
    int mouseY = event.motion.y;
    SDL_bool done = SDL_FALSE;
    SDL_Rect car;
    car.w = taille_carre;
    car.h = taille_carre;
    int x;
    int y;
    unsigned int lg = 0;
    unsigned char** plato =(unsigned char**) malloc(1600/taille_carre * sizeof(unsigned char*));
    for (x=0 ; x < 1600/taille_carre ; x++) {
        plato[x] =(unsigned char*) malloc(800/taille_carre * sizeof(unsigned char));
        for (y=0 ; y < 800/taille_carre ; y++) {
            plato[x][y] = 0;
        }
    }
    while (!done) {//tant que notre booleen est faux
        SDL_PollEvent(&event); //si un événement a été détecté
            switch (event.type) {//on va faire des trucs en fonction de l'événement détecté
                case SDL_QUIT: //Si la fenetre est fermée
                    done = SDL_TRUE;//on met notre booléen=True, pour arreter la boucle
                    return tableau;
                    break;
                case SDL_KEYDOWN://si une touche est pressée 
                    if (event.key.keysym.sym == SDLK_RETURN) {//si la touche pressée est la touche Entrée
                        done=SDL_TRUE;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    x = mouseX/taille_carre;
                    y = mouseY/taille_carre;
                    if ( event.button.button == SDL_BUTTON_LEFT) {
                        if ( plato[x][y] == 0 ) {
                            SDL_SetRenderDrawColor(render,255,255,255,SDL_ALPHA_OPAQUE);
                                    
                            car.x = taille_carre*(x);
                            car.y = taille_carre*(y);
                            SDL_RenderFillRect(render, &car);
                            plato[x][y] = 1;
                            lg += 1;
                        }
                    }
                    else if (event.button.button == SDL_BUTTON_RIGHT) {
                        if ( plato[x][y] == 1 ) {
                            SDL_SetRenderDrawColor(render,0,0,0,SDL_ALPHA_OPAQUE);
                            car.x = taille_carre*(x);
                            car.y = taille_carre*(y);
                            SDL_RenderFillRect(render, &car);

                            plato[x][y] = 0;
                            lg -= 1;
                        }
                    }
                    SDL_RenderPresent(render);
                    break;
                case SDL_MOUSEMOTION:
                    mouseX = event.motion.x;
                    mouseY = event.motion.y;

                    x = mouseX/taille_carre;
                    y = mouseY/taille_carre;
                    if ( event.button.button == SDL_BUTTON_LEFT) {
                        if ( plato[x][y] == 0 ) {
                            SDL_SetRenderDrawColor(render,255,255,255,SDL_ALPHA_OPAQUE);
                                
                            car.x = taille_carre*(x);
                            car.y = taille_carre*(y);
                            SDL_RenderFillRect(render, &car);
                            plato[x][y] = 1;
                            lg += 1;
                        }
                    }
                    else if (event.button.button == SDL_BUTTON_RIGHT) {
                        if ( plato[x][y] == 1 ) {
                            SDL_SetRenderDrawColor(render,0,0,0,SDL_ALPHA_OPAQUE);
                            car.x = taille_carre*(x);
                            car.y = taille_carre*(y);
                            SDL_RenderFillRect(render, &car);

                            plato[x][y] = 0;
                            lg -= 1;
                        }
                    }
                    SDL_RenderPresent(render);

                    break;
            }
    }
    tableau.longueur = lg;
    tableau.tab = (unsigned int**)malloc(lg*sizeof(unsigned int*));
    lg = 0;
    for (x=0 ; x < 1600/taille_carre ; x++) {
        for (y=0 ; y < 800/taille_carre ; y++) {
            if ( plato[x][y] == 1 ) {
                tableau.tab[lg] = (unsigned int*) malloc( 2 * sizeof(unsigned int) );
                tableau.tab[lg][0] = x;
                tableau.tab[lg][1] = y;
                lg += 1;
            }
        }
        free(plato[x]);
    }
    free(plato);
    return tableau;
}