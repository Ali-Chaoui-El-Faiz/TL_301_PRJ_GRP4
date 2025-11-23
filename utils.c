#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
// partie 1
t_cell *createcell(int som, float prob) {
    t_cell *cell = (t_cell*)malloc(sizeof(t_cell));
    cell->prob = prob;
    cell->arr  = som;
    cell->next = NULL;
    return cell;
}
t_list createemptylist() {
    t_list newlist;
    newlist.head = NULL;
    return newlist;

}
void addcell(t_list *list, int som, float prob) {
    t_cell *c = createcell(som, prob);   // alloue dynamiquement
    c->next = list->head;
    list->head = c;
}
t_list_adj createlistadj(int n) {
    t_list_adj new_adj;
    new_adj.n = n;
    new_adj.tab = (t_list*)malloc(n * sizeof(t_list));
    for (int i = 0; i < n; i++) new_adj.tab[i] = createemptylist();
    return new_adj;
}
void displaylist(t_list list) {
    t_cell *cell;
    cell = list.head;
    printf("[head @] ->\n");
    while (cell != NULL) {
        printf(" (%d,%.2f) @->",cell->arr,cell->prob);
        cell = cell->next;
    }
    printf("\n");
}
void displaylistadj (t_list_adj list) {
    for (int i = 0; i < list.n; i++) {
        printf("Liste pour le sommet %d:",i+1);
        displaylist(list.tab[i]);
    }
}

t_list_adj readGraph(const char *filename) {
    FILE *file = fopen(filename, "rt"); // read-only, text
    int nbvert, depart, arrivee;
    float proba;
    t_list_adj list_adj;

    if (file==NULL)
    {
        perror("Could not open file for reading");
        exit(EXIT_FAILURE);
    }
    // first line contains number of vertices
    if (fscanf(file, "%d", &nbvert) != 1)
    {
        perror("Could not read number of vertices");
        exit(EXIT_FAILURE);
    }
    list_adj = createlistadj(nbvert);
    while (fscanf(file, "%d %d %f", &depart, &arrivee, &proba) == 3)
    {
        // on obtient, pour chaque ligne du fichier les valeurs
        // depart, arrivee, et proba
        addcell(&list_adj.tab[depart - 1], arrivee, proba);

    }
    fclose(file);
    return list_adj;
}
char *getID(int num)
{
    char *id = malloc(10 * sizeof(char));  // Allocation dynamique
    int len = 0;
    int i = num;

    while (i >= 0) {
        i--;
        id[len++] = 'A' + (i % 26);
        i= (i / 26) - 1;
    }

    // Reverse the string to get the correct order
    for (int i = 0; i < len / 2; i++) {
        char temp = id[i];
        id[i] = id[len - 1 - i];
        id[len - 1 - i] = temp;
    }

    id[len] = '\0';
    return id;
}
// Partie 2 ; Vérification du graphe de Markov
// Ici on cherche à voir si la somme des probas sortantes de chaque sommet est compris entre 0.99 et 1

void verifierGrapheMarkov(t_list_adj *graphe) {

    int erreurTrouvee = 0;

    printf("\n--- Verification du graphe de Markov... ---\n");

    // On parcourt chaque sommet
    for (int i = 0; i < graphe->n; i++) {

        float somme = 0.0;
        t_cell *courant = graphe->tab[i].head; // ptr pour parcourir la liste

        // On parcourt la liste chainee de ce sommet en additionnant la proba à chaque fois
        while (courant != NULL) {
            somme += courant->prob;
            courant = courant->next;
        }

        // On vérifie la somme pour le sommet i
        if (somme < 0.99 || somme > 1.0) {
            printf(" Erreur : la somme des probabilites du sommet %d est %.2f\n", i + 1, somme);
            erreurTrouvee = 1;
        }
    }

    if (erreurTrouvee) {
        printf("--- Le graphe n'est pas un graphe de Markov ---\n");
    } else {
        printf("OK...\n");
        printf("--- Le graphe est un graphe de Markov ---\n");
    }
}
//SAMY étape 3
void exportToMermaid(t_list_adj *graphe, const char *filename) {
    FILE *f = fopen(filename, "wt");
    if (!f) {
        perror("Erreur d'ouverture du fichier Mermaid");
        return;
    }

    fprintf(f, "---\nconfig:\n   layout: elk\n   theme: neo\n   look: neo\n---\n\n");
    fprintf(f, "flowchart LR\n");

    // Sommets
    for (int i = 0; i < graphe->n; i++) {
        char *node_id = getID(i + 1);
        fprintf(f, "%s((%d))\n", node_id, i + 1);
        free(node_id);
    }

    fprintf(f, "\n");
    // Arêtes
    for (int i = 0; i < graphe->n; i++) {
        t_cell *c = graphe->tab[i].head;
        while (c != NULL) {
            char *source_id = getID(i + 1);
            char *dest_id = getID(c->arr);

            printf("DEBUG: i=%d, c->arr=%d, source=%s, dest=%s\n", i, c->arr, source_id, dest_id);

            fprintf(f, "%s -->|%.2f|%s\n", source_id, c->prob, dest_id);


            free(source_id);
            free(dest_id);

            c = c->next;
        }
    }

    fclose(f);
    printf("\n Fichier Mermaid exporte : %s\n", filename);
}