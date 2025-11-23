//
// Created by wacim on 22/11/2025.
//

#include "vertex.h"
#include <stdlib.h>
#include <stdio.h>


// Initialise le tableau des sommets pour Tarjan
t_tarjan_vertex* init_tarjan_vertices(t_list_adj list) {
    int n = list.n;
    t_tarjan_vertex* T = malloc(n * sizeof(*T));
    for (int i = 0; i < n; i++) {
        T[i].id      = i + 1;
        T[i].num     = -1;     // non visité
        T[i].num_acc = -1;
        T[i].treat   = 0;
    }
    return T;
}

void free_tarjan_vertex(t_tarjan_vertex *T) {
    free(T);
}

// Crée un tableau de classes vides
t_class* init_classes(int n) {
    t_class *cls = malloc(n * sizeof(t_class));
    for (int i = 0; i < n; i++) {
        cls[i].name[0] = '\0';
        cls[i].sommets = NULL;
        cls[i].n = 0;
    }
    return cls;
}

// Initialise une partition vide
void init_partition(t_partition *p, int max_classes) {
    p->classes = init_classes(max_classes);
    p->n = 0;
}

// Libère la partition
void free_partition(t_partition *p) {
    for (int i = 0; i < p->n; i++) {
        free(p->classes[i].sommets);
        p->classes[i].sommets = NULL;
        p->classes[i].n = 0;
    }
    free(p->classes);
    p->classes = NULL;
    p->n = 0;
}

// Ajoute un sommet dans une classe
static void add_vertex_to_class(t_class *c, int vertex_id) {
    c->sommets = realloc(c->sommets, (c->n + 1) * sizeof(int));
    c->sommets[c->n] = vertex_id;
    c->n += 1;
}

static int min_int(int a, int b) {
    return (a < b) ? a : b;
}


// DFS de Tarjan sur un sommet v
static void parcours(int v,
                     t_list_adj *graph,
                     t_tarjan_vertex *T,
                     int *index,
                     int *stack,
                     int *top,
                     t_partition *p)
{
    // Affectation du numéro de visite
    T[v].num     = *index;
    T[v].num_acc = *index;
    (*index)++;

    // Empile ce sommet
    (*top)++;
    stack[*top] = v;
    T[v].treat = 1;

    // Pour chaque successeur w
    t_cell *cur = graph->tab[v].head;
    while (cur != NULL) {
        int w = cur->arr - 1;

        if (T[w].num == -1) {
            // w non visité : on descend
            parcours(w, graph, T, index, stack, top, p);
            T[v].num_acc = min_int(T[v].num_acc, T[w].num_acc);
        }
        else if (T[w].treat) {
            // w est dans la pile : arc de retour
            T[v].num_acc = min_int(T[v].num_acc, T[w].num);
        }

        cur = cur->next;
    }

    // Si racine d’une composante
    if (T[v].num_acc == T[v].num) {
        t_class c;
        c.sommets = NULL;
        c.n = 0;
        snprintf(c.name, sizeof(c.name), "C%d", p->n + 1);

        int w;
        do {
            w = stack[*top];
            (*top)--;
            T[w].treat = 0;
            add_vertex_to_class(&c, T[w].id);
        } while (w != v);

        p->classes[p->n] = c;
        p->n += 1;
    }
}


// Algorithme principal
void tarjan(t_list_adj *graph, t_partition *p) {
    int n = graph->n;

    init_partition(p, n);

    t_tarjan_vertex *T = init_tarjan_vertices(*graph);

    int *stack = malloc(n * sizeof(int));
    int top = -1;
    int index = 0;

    for (int v = 0; v < n; v++) {
        if (T[v].num == -1) {
            parcours(v, graph, T, &index, stack, &top, p);
        }
    }

    free(stack);
    free_tarjan_vertex(T);
}


// Affichage simple des classes
void afficher_partition(const t_partition *p) {
    for (int i = 0; i < p->n; i++) {
        const t_class *c = &p->classes[i];
        printf("Composante %s: {", c->name);
        for (int j = 0; j < c->n; j++) {
            printf("%d", c->sommets[j]);
            if (j < c->n - 1) printf(",");
        }
        printf("}\n");
    }
}
