#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <stdio.h>
#include <stdlib.h>
typedef struct node {
    unsigned int couche;// la couche de la node
// le cache contient les nodes, mais les pointeurs vers ces nodes peuvent changer de valeurs
// on ne peut donc pas se contenter d'utiliser des pointeurs simples, car ci la valeur du pointeur change dans le cache
// elle ne sera pas actualisée dans la structure
    unsigned long indice_no;// l'adresse, dans le cache, du Nord-Ouest de la node  cache[node.couche - 1][indice_no]
    unsigned long indice_ne;// l'adresse, dans le cache, du Nord-Est de la node  cache[node.couche - 1][indice_ne]
    unsigned long indice_so;// l'adresse, dans le cache, du Sud-Ouest de la node  cache[node.couche - 1][indice_so]
    unsigned long indice_se;// l'adresse, dans le cache, du Sud-Est de la node  cache[node.couche - 1][indice_se]
    unsigned long indice_futur;// l'adresse, dans le cache, de la node à "l'itération" suivante  cache[node.couche + 1][indice_futur]
} Node;

typedef struct tableau_coordonnees {
    unsigned int longueur;
    unsigned int** tab;
} TabCoord;

unsigned long long* dimensions_cache;
unsigned long long taille_cache;
//dimensions_cache[0] =  la taille maximale contenue dans le cache, c'est à dire la longueur de dimension_cache
//dimensions_cache[i] = le nombre de node différentes, de couche i, contenues dans le cache, ie: taille(cache[i])
Node** cache;
//les nodes de couche 0| les feuilles ne sont pas contenues dans le cache
// cache[i] : nodes de couche i contenues dans le cache, triés par ordre croissant de hash
// les zeros doivent faire partie du cache, par conséquent, cache[i][0] correspond toujours au zéro de couche i

int max(int a, int b);
unsigned long long Puissance2(int exposant);
void LibererTableau(TabCoord tableau);
void CreerZeros(int niveau);
unsigned long VerifierCache(Node truc);
unsigned long Joindre(int couche, unsigned long a, unsigned long b, unsigned long c, unsigned long d);
unsigned long Entourer(Node root);
unsigned long Detourer( Node nod, unsigned int* couche );
unsigned long CalculerSuccesseur4x4( Node nod );
unsigned long CalculerSuccesseur( unsigned int couche, unsigned long ind);

#endif