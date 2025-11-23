#ifndef __HASSE_H__
#define __HASSE_H__

#include "vertex.h"
#include "utils.h"

// Structure d'un lien
typedef struct {
    int from; // Indice de la classe de départ
    int to;   // Indice de la classe d'arrivée
} t_link;

// Structure du tableau de liens
typedef struct {
    t_link *links; // Tableau dynamique
    int log_size;  // Nombre actuel de liens
    int phy_size;  // Taille mémoire allouée
} t_link_array;


void removeTransitiveLinks(t_link_array *p_link_array);

// Etape 2 et 3

// Crée le tableau des liens entre les classes
t_link_array createLinkArray(t_partition *p, t_list_adj *graph);

// Exporte le diagramme en Mermaid (Etape 2)
void exportHasseMermaid(t_partition *p, t_link_array *links, const char *filename);

// Affiche les propriétés (Transitoire, Persistant, Absorbant) (Etape 3)
void printGraphProperties(t_partition *p, t_link_array *links);

// Libère la mémoire
void freeLinkArray(t_link_array *l);

#endif