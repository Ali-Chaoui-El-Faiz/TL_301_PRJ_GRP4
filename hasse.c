#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "hasse.h"


void removeTransitiveLinks(t_link_array *p_link_array)
{
    int i = 0;
    while (i < p_link_array->log_size)
    {
        t_link link1 = p_link_array->links[i];
        int j = 0;
        int to_remove = 0;
        while (j < p_link_array->log_size && !to_remove)
        {
            if (j != i)
            {
                t_link link2 = p_link_array->links[j];
                if (link1.from == link2.from)
                {
                    // look for a link from link2.to to link1.to
                    int k = 0;
                    while (k < p_link_array->log_size && !to_remove)
                    {
                        if (k != j && k != i)
                        {
                            t_link link3 = p_link_array->links[k];
                            if ((link3.from == link2.to) && (link3.to == link1.to))
                            {
                                to_remove = 1;
                            }
                        }
                        k++;
                    }
                }
            }
            j++;
        }
        if (to_remove)
        {
            // remove link1 by replacing it with the last link
            p_link_array->links[i] = p_link_array->links[p_link_array->log_size - 1];
            p_link_array->log_size--;
        }
        else
        {
            i++;
        }
    }
}
// Partie 2 étape 2
// Utilitaire : Ajoute un lien s'il n'existe pas déjà
void add_unique_link(t_link_array *arr, int from, int to) {
    // Vérifie si le lien existe déjà
    for (int i = 0; i < arr->log_size; i++) {
        if (arr->links[i].from == from && arr->links[i].to == to) {
            return;
        }
    }

    // Redimensionne si nécessaire
    if (arr->log_size >= arr->phy_size) {
        arr->phy_size *= 2;
        arr->links = realloc(arr->links, arr->phy_size * sizeof(t_link));
    }

    // Ajoute le lien
    arr->links[arr->log_size].from = from;
    arr->links[arr->log_size].to = to;
    arr->log_size++;
}

// Etape 2 : Création des liens
t_link_array createLinkArray(t_partition *p, t_list_adj *graph) {
    t_link_array res;
    res.phy_size = 10; // Taille initiale arbitraire
    res.log_size = 0;
    res.links = malloc(res.phy_size * sizeof(t_link));

    // 1. Tableau pour savoir "sommet X est dans quelle classe ?"
    // indice du tableau = (numéro sommet - 1), valeur = indice classe
    int *vertex_class_map = malloc(graph->n * sizeof(int));

    for (int i = 0; i < p->n; i++) { // Pour chaque classe
        for (int j = 0; j < p->classes[i].n; j++) { // Pour chaque sommet
            int val_sommet = p->classes[i].sommets[j];
            vertex_class_map[val_sommet - 1] = i;
        }
    }

    // Parcours de tous les arcs du graphe
    for (int u = 0; u < graph->n; u++) {
        t_cell *curr = graph->tab[u].head;
        while (curr != NULL) {
            int v = curr->arr - 1; // Sommet destination

            int class_u = vertex_class_map[u];
            int class_v = vertex_class_map[v];

            // Si l'arc va vers une autre classe, c'est un lien Hasse
            if (class_u != class_v) {
                add_unique_link(&res, class_u, class_v);
            }
            curr = curr->next;
        }
    }

    free(vertex_class_map);
    return res;
}

// Etape 2 (Validation) : Export Mermaid
void exportHasseMermaid(t_partition *p, t_link_array *links, const char *filename) {
    FILE *f = fopen(filename, "wt");
    if (!f) return;

    fprintf(f, "graph TD\n");

    // Dessiner les classes (noeuds)
    for (int i = 0; i < p->n; i++) {
        fprintf(f, "    C%d[\"{", i);
        for (int j = 0; j < p->classes[i].n; j++) {
            fprintf(f, "%d", p->classes[i].sommets[j]);
            if (j < p->classes[i].n - 1) fprintf(f, ",");
        }
        fprintf(f, "}\"]\n");
    }

    // Dessiner les liens
    for (int i = 0; i < links->log_size; i++) {
        // C_depart --> C_arrivee
        fprintf(f, "    C%d --> C%d\n", links->links[i].from, links->links[i].to);
    }

    fclose(f);
    printf("Fichier Hasse genere : %s\n", filename);
}

// Etape 3 : Propriétés
void printGraphProperties(t_partition *p, t_link_array *links) {
    printf("\n--- CARACTERISTIQUES DU GRAPHE ---\n");

    // 1. Irréductible ?
    if (p->n == 1) printf("[x] Le graphe est IRREDUCTIBLE.\n");
    else printf("[ ] Le graphe n'est PAS irreductible (%d classes).\n", p->n);

    // 2. Analyse des classes
    for (int i = 0; i < p->n; i++) {
        int est_transitoire = 0;

        // Vérifie si la classe 'i' a des liens sortants
        for (int k = 0; k < links->log_size; k++) {
            if (links->links[k].from == i) {
                est_transitoire = 1;
                break;
            }
        }

        printf("Classe %s : ", p->classes[i].name);
        if (est_transitoire) {
            printf("TRANSITOIRE\n");
        } else {
            printf("PERSISTANTE");
            if (p->classes[i].n == 1) {
                printf(" -> Etat ABSORBANT (%d)\n", p->classes[i].sommets[0]);
            } else {
                printf("\n");
            }
        }
    }
    printf("----------------------------------\n");
}

void freeLinkArray(t_link_array *l) {
    free(l->links);
    l->log_size = 0;
    l->phy_size = 0;
}