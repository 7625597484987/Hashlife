#include "functions.h"

unsigned long long* dimensions_cache = NULL;
unsigned long long taille_cache = 0;
//dimensions_cache[0] =  la taille maximale contenue dans le cache, c'est à dire la longueur de dimension_cache
//dimensions_cache[i] = le nombre de node différentes, de couche i, contenues dans le cache, ie: taille(cache[i])
Node** cache = NULL;
//les nodes de couche 0| les feuilles ne sont pas contenues dans le cache
// cache[i] : nodes de couche i contenues dans le cache, triés par ordre croissant de hash
// les zeros doivent faire partie du cache, par conséquent, cache[i][0] correspond toujours au zéro de couche i

int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}
// renvoit 2^exposant
unsigned long long Puissance2(int exposant) {
    unsigned long long retour = 1;
    for (int i=0 ; i < exposant ; i++) {
        retour = retour*2;
    }
    return retour;
}
void LibererTableau(TabCoord tableau) {
    for (int j=0 ; j < tableau.longueur ; j++) {
        free(tableau.tab[j]);
    }
    free(tableau.tab);  
}

// Crée toutes les nodes vides de couche inférieure ou valant niveau
void CreerZeros(int niveau) {
    cache = (Node**)realloc( cache,  (niveau + 1) * sizeof(Node*) );
    dimensions_cache = (unsigned long long*)realloc( dimensions_cache , (niveau + 1) * sizeof(unsigned long long) );

    for (int i = taille_cache + 1 ; i <= niveau ; i++) {
        cache[i] = (Node*)malloc(sizeof(Node));
        dimensions_cache[i] = 1;

        cache[i][0].couche = i;
        cache[i][0].indice_no = 0 ;
        cache[i][0].indice_ne = 0 ;
        cache[i][0].indice_so = 0 ;
        cache[i][0].indice_se = 0 ;
// 
        cache[i][0].indice_futur = 0 ;
    }
    taille_cache = niveau;
}

// Vérifie si la node est présente dans le cache : si elle l'est, on retourne son adresse, si elle ne l'est pas, on l'ajoute et l'on retourne son adresse
// On compare les hash
// le hash devrait il être calculé dans cette fonction directement?
unsigned long VerifierCache(Node truc) {
    if ( truc.couche > taille_cache ) {//si la node est plus grande que toutes celles contenues dans le cache
    //on augmente la taille du cache dans la fonction CreerZeros
//on ajoute une node vide, suivie de la node "truc" dans le cache, ce qui adapte les dimensions
        CreerZeros(truc.couche);
        cache[truc.couche] = (Node*)realloc(cache[truc.couche], 2*sizeof(Node));
        cache[truc.couche][1] = truc; 

        dimensions_cache[truc.couche] = 2;
//on retourne l'adresse, dans le cache, de la node
        return( 1 );
    }
    // Dans cette implémentation, le cache n'est pas trié
    unsigned int indice = 0;
    for (indice = 0 ; indice < dimensions_cache[truc.couche] ; indice ++) {
        if ( cache[truc.couche][indice].indice_no == truc.indice_no && 
        cache[truc.couche][indice].indice_ne == truc.indice_ne && 
        cache[truc.couche][indice].indice_so == truc.indice_so && 
        cache[truc.couche][indice].indice_se == truc.indice_se ) {// si la node se trouve déjà dans notre cache
        //on renvoit son adresse dans le cache
            return ( indice );
        }
    }
    cache[truc.couche] = (Node*)realloc(cache[truc.couche] , (dimensions_cache[truc.couche] + 1) * sizeof(Node));
    cache[ truc.couche ][ dimensions_cache[truc.couche] ] = truc;
    // on augmente la taille de la ligne de cache
    dimensions_cache[truc.couche] += 1;
    // on retourne l'adresse de la node que l'on vient de rajouter au cache
    return ( dimensions_cache[truc.couche] - 1 );
}

// Crée la node résultant de la fusion de a,b,c,d, avec a au Nord-Ouest, b au Nord-Est, etc..., dont la couche resultante est couche, et renvoit son adresse dans le cache
// La valeur retournée est déjà cachée
unsigned long Joindre(int couche, unsigned long a, unsigned long b, unsigned long c, unsigned long d) {
    Node retour;
    retour.couche =  couche ;
//nécessaire si on ne trouve pas la node dans le cache
    retour.indice_futur = 18446744073709551615ul;// -1 en unsigned
    retour.indice_no = a;
    retour.indice_ne = b;
    retour.indice_so = c;
    retour.indice_se = d;
    
    return VerifierCache(retour);
}

/* \brief cette fonction retourne notre node que l'on a faite passer au niveau supérieur
 * ie: on entoure notre carré (de côté 2**k) avec des cellules vides jusqu'à créer un carré de côté 2**(k+1)
 * la node renvoyée est déjà cachée */
unsigned long Entourer(Node root) {
    if ( root.couche != 0 ) {
        Node retour;
        retour.couche = root.couche + 1;

        Node temporaire;
        temporaire.couche = root.couche ;
        temporaire.indice_futur = 18446744073709551615ul;// -1 en unsigned
    //les zeros de taille appropriée existent nécessairement, puisqu'ils ont une taille inférieure
    // à celle de root
        temporaire.indice_no = 0 ;//pointeur vers le 0 de taille inférieure à notre root
        temporaire.indice_ne = 0 ;
        temporaire.indice_so = 0 ;
    //0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
    // quart supérieur gauche: Nord Ouest
        temporaire.indice_se = root.indice_no;
        retour.indice_no = VerifierCache(temporaire);
        temporaire.indice_se = 0;

    // quart supérieur droit: Nord Est
        temporaire.indice_so = root.indice_ne;
        retour.indice_ne = VerifierCache(temporaire);
        temporaire.indice_so = 0;
        
    // quart inférieur gauche: Sud Ouest
        temporaire.indice_ne = root.indice_so;
        retour.indice_so = VerifierCache(temporaire);
        temporaire.indice_ne = 0;
        
    // quart inférieur droit: Sud Est
        temporaire.indice_no = root.indice_se;
        retour.indice_se = VerifierCache(temporaire);

        retour.indice_futur = 18446744073709551615ul;// -1 en unsigned
        return VerifierCache(retour);
    }
    else {
        printf("Error: On entoure une node de couche 0\n");
        return -1;
    }
}

// peut sans doute être modifié et optimisé, pour ne pas faire appel à verifiercache(nod)
// si l'on peut isoler une node entourée de 0, retourne l'adresse de cette node dans le cache, sinon, retourne celle de la node passée en argument
unsigned long Detourer( Node nod, unsigned int* couche ) {
    if ( *couche > 1 ) {
        Node retour;
        retour.couche = nod.couche - 1;
        if ( 
        cache[nod.couche - 1][nod.indice_no].indice_no == 0 && 
        cache[nod.couche - 1][nod.indice_no].indice_so == 0 && 
        cache[nod.couche - 1][nod.indice_no].indice_ne == 0) {
            retour.indice_no = cache[nod.couche - 1][nod.indice_no].indice_se;
        }
        else {
            return VerifierCache(nod);
        }
        if ( 
        cache[nod.couche - 1][nod.indice_ne].indice_ne == 0 && 
        cache[nod.couche - 1][nod.indice_ne].indice_se == 0 && 
        cache[nod.couche - 1][nod.indice_ne].indice_no == 0) {
            retour.indice_ne = cache[nod.couche - 1][nod.indice_ne].indice_so;
        }
        else {
            return VerifierCache(nod);
        }
        if ( 
        cache[nod.couche - 1][nod.indice_so].indice_so == 0 && 
        cache[nod.couche - 1][nod.indice_so].indice_no == 0 && 
        cache[nod.couche - 1][nod.indice_so].indice_se == 0) {
            retour.indice_so = cache[nod.couche - 1][nod.indice_so].indice_ne;
        }
        else {
            return VerifierCache(nod);
        }
        if ( 
        cache[nod.couche - 1][nod.indice_se].indice_se == 0 && 
        cache[nod.couche - 1][nod.indice_se].indice_so == 0 && 
        cache[nod.couche - 1][nod.indice_se].indice_ne == 0) {
            retour.indice_se = cache[nod.couche - 1][nod.indice_se].indice_no;
        }
        else {
            return VerifierCache(nod);
        }
        retour.indice_futur = 18446744073709551615ul;// -1 en unsigned
        *couche -= 1;
        unsigned long tempo = Detourer( retour , couche );
        return ( tempo );
    }
    else {
        printf("Error: On ne peut pas detourer une node de couche 1 ou 0\n");
        return VerifierCache(nod);
    }
}

// cette fonction ne peut etre appelée qu'une fois par node de niveau 2 existant
// c'est à dire, que dans le programme, elle sera appelée au plus 2**(4*4) = 65536 fois
// Calcule la node à "l'itération" suivante, dans le cas d'une node couche 2
// le resultat est caché
unsigned long CalculerSuccesseur4x4( Node nod ) {
    int ligne1 = 0;
    int ligne2 = 0;
    int ligne3 = 0;
    Node retour;
    retour.couche = 1;
    retour.indice_futur = 18446744073709551615ul;// -1 en unsigned
// Cas de la cellule Nord-Ouest
    ligne1 = cache[ 1 ][nod.indice_no].indice_no + cache[ 1 ][nod.indice_no].indice_ne + cache[ 1 ][nod.indice_ne].indice_no;
    ligne2 = cache[ 1 ][nod.indice_no].indice_so + cache[ 1 ][nod.indice_ne].indice_so;
    ligne3 = cache[ 1 ][nod.indice_so].indice_no + cache[ 1 ][nod.indice_so].indice_ne + cache[ 1 ][nod.indice_se].indice_no;
// les règles basiques du jeu de la vie
    if ( (ligne1 + ligne2 + ligne3 == 3) || 
    ( cache[ 1 ][nod.indice_no].indice_se == 1 && ligne1 + ligne2 + ligne3 == 2 ) ) {
        retour.indice_no = 1;
    }
    else {
        retour.indice_no = 0 ;
    }
// Cas de la cellule Nord-Est
    ligne1 += cache[ 1 ][nod.indice_ne].indice_ne - cache[ 1 ][nod.indice_no].indice_no;
    ligne2 = cache[ 1 ][nod.indice_no].indice_se + cache[ 1 ][nod.indice_ne].indice_se;
    ligne3 += cache[ 1 ][nod.indice_se].indice_ne  - cache[ 1 ][nod.indice_so].indice_no;
// les règles basiques du jeu de la vie
    if ( (ligne1 + ligne2 + ligne3 == 3) ||
    ( cache[ 1 ][nod.indice_ne].indice_so == 1 && ligne1 + ligne2 + ligne3 == 2 ) ) {
// cache[0] + 1 est le pointeur vers une cellule vivante
        retour.indice_ne = 1;
    }
    else {
// cache[0] est le pointeur vers une cellule morte
        retour.indice_ne = 0;
    }

// Cas de la cellule Sud-Est
    ligne1 = ligne2 + cache[ 1 ][nod.indice_ne].indice_so;
    ligne2 = ligne3 - cache[ 1 ][nod.indice_se].indice_no;
    ligne3 = cache[ 1 ][nod.indice_se].indice_so + cache[ 1 ][nod.indice_se].indice_se + cache[ 1 ][nod.indice_so].indice_se;
// les règles basiques du jeu de la vie
    if ( (ligne1 + ligne2 + ligne3 == 3) ||
    ( cache[ 1 ][nod.indice_se].indice_no == 1 && ligne1 + ligne2 + ligne3 == 2 ) ) {
// cache[0] + 1 est le pointeur vers une cellule vivante
        retour.indice_se = 1;
    }
    else {
// cache[0] est le pointeur vers une cellule morte
        retour.indice_se = 0;
    }
    
// Cas de la cellule Sud-Ouest
    ligne1 += cache[ 1 ][nod.indice_no].indice_so - cache[ 1 ][nod.indice_ne].indice_se;
    ligne2 = cache[ 1 ][nod.indice_so].indice_no + cache[ 1 ][nod.indice_se].indice_no;
    ligne3 += cache[ 1 ][nod.indice_so].indice_so - cache[ 1 ][nod.indice_se].indice_se;
// les règles basiques du jeu de la vie
    if ( (ligne1 + ligne2 + ligne3 == 3) ||
    ( cache[ 1 ][nod.indice_so].indice_ne == 1 && ligne1 + ligne2 + ligne3 == 2 ) ) {
// cache[0] + 1 est le pointeur vers une cellule vivante
        retour.indice_so = 1;
    }
    else {
// cache[0] est le pointeur vers une cellule morte
        retour.indice_so = 0;
    }
    return VerifierCache(retour);
}

// Calcule le successeur de la node 2^(couche- 2) génération dans le futur, une couche inférieure
// voir les détails de l'algorithme
// la résultat est déjà caché, et sans doute hashé
unsigned long CalculerSuccesseur( unsigned int couche, unsigned long ind) {
// si le successeur n'a jamais été calculé
// il y a une redondance avec les appels récursifs de temporaire1-9
    if ( cache[couche][ind].indice_futur == 18446744073709551615ul ) {
// On utilise cette variable par précaution, si l'adresse du cache a été modifiée par l'appel à la fonction
        unsigned long tempo;

// en théorie, on n'a jamais à calculer le successeur d'une node de dimensions 2x2 ou 1x1
        if ( couche <= 2 ) {
// le cas de base de notre récursion est une cellule 4x4
            if ( couche == 2) {
        // si la node n'a pas encore eu son successeur calculé, on met le résultat dans le cache de celle-ci
                tempo = CalculerSuccesseur4x4( cache[2][ind] );
                cache[2][ind].indice_futur = tempo;
            }
            else {
                printf("probleme\n");
                return -1;
            }
        }
        else {
// ces nodes temporaires stockent les successeurs des 9 sous-nodes intermédiaires
        unsigned long temporaire1 = cache[ couche ][ ind ].indice_no;
        if ( cache[ couche - 1][ temporaire1].indice_futur == 18446744073709551615ul ) {
            tempo = CalculerSuccesseur( couche - 1, temporaire1 );
            cache[ couche - 1 ][ temporaire1 ].indice_futur = tempo;
        }
        temporaire1 = cache[ couche - 1][ temporaire1 ].indice_futur;

        unsigned long temporaire2 = Joindre( couche - 1,
            cache[couche - 1][cache[ couche ][ ind ].indice_no ].indice_ne,
            cache[couche - 1][cache[ couche ][ ind ].indice_ne ].indice_no,
            cache[couche - 1][cache[ couche ][ ind ].indice_no ].indice_se,
            cache[couche - 1][cache[ couche ][ ind ].indice_ne ].indice_so );
        if ( cache[ couche - 1 ][ temporaire2 ].indice_futur == 18446744073709551615ul ) {
            tempo = CalculerSuccesseur( couche - 1, temporaire2 );
            cache[ couche - 1 ][ temporaire2 ].indice_futur = tempo;
        }
        temporaire2 = cache[ couche - 1][ temporaire2 ].indice_futur;

        unsigned long temporaire3 = cache[ couche ][ ind ].indice_ne;
        if ( cache[ couche - 1 ][ temporaire3 ].indice_futur == 18446744073709551615ul ) {
            tempo = CalculerSuccesseur( couche - 1, temporaire3 );
            cache[ couche - 1 ][ temporaire3 ].indice_futur = tempo;
        }
        temporaire3 = cache[ couche - 1][ temporaire3 ].indice_futur;

        unsigned long temporaire4 = Joindre( couche - 1,
            cache[couche - 1][ cache[ couche ][ ind ].indice_no ].indice_so,
            cache[couche - 1][ cache[ couche ][ ind ].indice_no ].indice_se,
            cache[couche - 1][ cache[ couche ][ ind ].indice_so ].indice_no,
            cache[couche - 1][ cache[ couche ][ ind ].indice_so ].indice_ne );
        if ( cache[ couche - 1 ][ temporaire4 ].indice_futur == 18446744073709551615ul ) {
            tempo = CalculerSuccesseur( couche - 1, temporaire4 );
            cache[ couche - 1 ][ temporaire4 ].indice_futur = tempo;
        }
        temporaire4 = cache[ couche - 1][ temporaire4 ].indice_futur;

        unsigned long temporaire5 = Joindre( couche - 1,
            cache[couche - 1][ cache[ couche ][ ind ].indice_no ].indice_se,
            cache[couche - 1][ cache[ couche ][ ind ].indice_ne ].indice_so,
            cache[couche - 1][ cache[ couche ][ ind ].indice_so ].indice_ne,
            cache[couche - 1][ cache[ couche ][ ind ].indice_se ].indice_no );
        if ( cache[ couche - 1 ][ temporaire5 ].indice_futur == 18446744073709551615ul ) {
            tempo = CalculerSuccesseur( couche - 1, temporaire5 );
            cache[ couche - 1 ][ temporaire5 ].indice_futur = tempo;
        }
        temporaire5 = cache[ couche - 1][ temporaire5 ].indice_futur;

        unsigned long temporaire6 = Joindre( couche - 1,
            cache[couche - 1][ cache[ couche ][ ind ].indice_ne ].indice_so,
            cache[couche - 1][ cache[ couche ][ ind ].indice_ne ].indice_se,
            cache[couche - 1][ cache[ couche ][ ind ].indice_se ].indice_no,
            cache[couche - 1][ cache[ couche ][ ind ].indice_se ].indice_ne );
        if ( cache[ couche - 1 ][ temporaire6 ].indice_futur == 18446744073709551615ul ) {
            tempo = CalculerSuccesseur( couche - 1, temporaire6 );
            cache[ couche - 1 ][ temporaire6 ].indice_futur = tempo;
        }
        temporaire6 = cache[ couche - 1][ temporaire6 ].indice_futur;

        unsigned long temporaire7 = cache[ couche ][ ind ].indice_so;
        if ( cache[ couche - 1][ temporaire7 ].indice_futur == 18446744073709551615ul ) {
            tempo = CalculerSuccesseur( couche - 1, temporaire7 );
            cache[ couche - 1 ][ temporaire7 ].indice_futur = tempo;
        }
        temporaire7 = cache[ couche - 1][ temporaire7 ].indice_futur;

        unsigned long temporaire8 = Joindre( couche - 1,
            cache[couche - 1][cache[ couche ][ ind ].indice_so ].indice_ne,
            cache[couche - 1][cache[ couche ][ ind ].indice_se ].indice_no,
            cache[couche - 1][cache[ couche ][ ind ].indice_so ].indice_se,
            cache[couche - 1][cache[ couche ][ ind ].indice_se ].indice_so );
        if ( cache[ couche - 1][ temporaire8 ].indice_futur == 18446744073709551615ul ) {
            tempo = CalculerSuccesseur( couche - 1, temporaire8 );
            cache[ couche - 1 ][ temporaire8 ].indice_futur = tempo;
        }
        temporaire8 = cache[ couche - 1][ temporaire8 ].indice_futur;

        unsigned long temporaire9 = cache[ couche ][ ind ].indice_se;
        if ( cache[ couche - 1][ temporaire9 ].indice_futur == 18446744073709551615ul ) {
            tempo = CalculerSuccesseur( couche - 1, temporaire9 );
            cache[ couche - 1 ][ temporaire9 ].indice_futur = tempo;
        }
        temporaire9 = cache[ couche - 1][ temporaire9].indice_futur;

// ######################################################################################################
// l'adresse du cache peut etre modifiée entre les appels
        temporaire1 = Joindre( couche - 1 , temporaire1, temporaire2, temporaire4, temporaire5);
        temporaire1 = CalculerSuccesseur( couche - 1 , temporaire1 );
        temporaire3 = Joindre( couche - 1 , temporaire2, temporaire3, temporaire5, temporaire6);
        temporaire3 = CalculerSuccesseur( couche - 1 , temporaire3 );
        temporaire7 = Joindre( couche - 1 , temporaire4, temporaire5, temporaire7, temporaire8);
        temporaire7 = CalculerSuccesseur( couche - 1 , temporaire7 );
        temporaire9 = Joindre( couche - 1 , temporaire5, temporaire6, temporaire8, temporaire9);
        temporaire9 = CalculerSuccesseur( couche - 1 , temporaire9 );
        tempo = Joindre( couche - 1 , temporaire1, temporaire3, temporaire7, temporaire9 );
        cache[couche][ind].indice_futur = tempo;
// la double récursion permet d'avoir un successeur avancé 2**(k-2) générations dans le temps
// pour profiter au maximum des règles du jeu: de l'espace entre 2**k -1 et 2**(k-1)
        }
    }
    return cache[couche][ind].indice_futur;
}
