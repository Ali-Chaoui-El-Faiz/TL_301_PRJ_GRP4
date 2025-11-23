//
// Created by wacim on 22/11/2025.
//

#ifndef VERTEX_H
#define VERTEX_H
#include "utils.h"

// Sommet utilisé par Tarjan
typedef struct s_tarjan_vertex {
    int id;       // numéro 1..n
    int num;      // numéro de visite
    int num_acc;  // lowlink
    int treat;    // 1 si dans la pile
} t_tarjan_vertex;

// Une classe (composante fortement connexe)
typedef struct s_class {
    char name[10];
    int *sommets; // tableau dynamique
    int n;        // taille
} t_class;

// Partition = ensemble de classes
typedef struct {
    t_class *classes;
    int n;        // nombre de classes
} t_partition;

// Création / destruction des structures Tarjan
t_tarjan_vertex* init_tarjan_vertices(t_list_adj list);
void free_tarjan_vertex(t_tarjan_vertex *T);

t_class* init_classes(int n);

void init_partition(t_partition *p, int max_classes);
void free_partition(t_partition *p);

// Algorithme de Tarjan
void tarjan(t_list_adj *graph, t_partition *p);

// Affichage
void afficher_partition(const t_partition *p);


#endif //VERTEX_H
