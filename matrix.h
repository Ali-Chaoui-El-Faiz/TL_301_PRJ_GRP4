#ifndef MATRIX_H
#define MATRIX_H

#include "utils.h" // pour t_list_adj

// Structure de la matrice / Etape 1
typedef struct {
    int rows; // Nombre de lignes
    int cols; // Nombre de colonnes
    double **data; // Données
} t_matrix;

// Fonctions de base
t_matrix createMatrix(int rows, int cols);
void freeMatrix(t_matrix *mat);
void printMatrix(t_matrix mat); // Pour afficher le résultat

// Etape 1

// Convertit le graphe (liste d'adjacence) en matrice de probabilités
t_matrix graphToMatrix(t_list_adj *graph);

// Recopie src dans dest
void copyMatrix(t_matrix *dest, t_matrix src);

// Multiplication : Res = A * B
t_matrix multiplyMatrices(t_matrix A, t_matrix B);

// Calcule la différence (somme des valeurs absolues)
double diffMatrix(t_matrix A, t_matrix B);

#endif