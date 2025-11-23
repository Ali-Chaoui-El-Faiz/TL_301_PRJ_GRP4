#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"

// Création d'une matrice vide (remplie de 0)
t_matrix createMatrix(int rows, int cols) {
    t_matrix mat;
    mat.rows = rows;
    mat.cols = cols;

    // Allocation des lignes
    mat.data = (double**)malloc(rows * sizeof(double*));

    // Allocation des colonnes et initialisation à 0 (calloc)
    for (int i = 0; i < rows; i++) {
        mat.data[i] = (double*)calloc(cols, sizeof(double));
    }

    return mat;
}

// Libération de la mémoire
void freeMatrix(t_matrix *mat) {
    if (mat == NULL || mat->data == NULL) return;
    for (int i = 0; i < mat->rows; i++) {
        free(mat->data[i]);
    }
    free(mat->data);
    mat->data = NULL;
}

// Affichage simple de la matrice
void printMatrix(t_matrix mat) {
    for (int i = 0; i < mat.rows; i++) {
        printf("| ");
        for (int j = 0; j < mat.cols; j++) {
            printf("%6.4f ", mat.data[i][j]); // 4 chiffres après la virgule
        }
        printf("|\n");
    }
}

// -- Etape 1 ---

// 1. Liste d'adjacence vers Matrice
t_matrix graphToMatrix(t_list_adj *graph) {
    int n = graph->n;
    t_matrix mat = createMatrix(n, n); // Matrice n x n initialisée à 0

    for (int i = 0; i < n; i++) {
        // On parcourt la liste chaînée du sommet i
        // Dans utils.h, tab est un pointeur vers t_list, qui contient head
        t_cell *curr = graph->tab[i].head;

        while (curr != NULL) {
    int dest_index = curr->arr - 1;

    if (dest_index >= 0 && dest_index < n) {
        mat.data[i][dest_index] = (double)curr->prob;
    }

    curr = curr->next;
    }
    }
    return mat;
}

// 2. Copie de matrice
void copyMatrix(t_matrix *dest, t_matrix src) {
    // On suppose que dest est déjà allouée à la bonne taille
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            dest->data[i][j] = src.data[i][j];
        }
    }
}

// 3. Multiplication de matrices
t_matrix multiplyMatrices(t_matrix A, t_matrix B) {
    // Vérification dimensions (colonnes A == lignes B)
    if (A.cols != B.rows) {
        printf("Erreur dimension !\n");
        return createMatrix(1, 1); // Retourne une erreur ou vide
    }

    t_matrix res = createMatrix(A.rows, B.cols);

    for (int i = 0; i < A.rows; i++) {
        for (int j = 0; j < B.cols; j++) {
            double sum = 0.0;
            for (int k = 0; k < A.cols; k++) {
                sum += A.data[i][k] * B.data[k][j];
            }
            res.data[i][j] = sum;
        }
    }
    return res;
}

// 4. Différence entre matrices (pour la convergence)
// Formule : Somme |m_ij - n_ij|
double diffMatrix(t_matrix A, t_matrix B) {
    double diff = 0.0;
    for (int i = 0; i < A.rows; i++) {
        for (int j = 0; j < A.cols; j++) {
            diff += fabs(A.data[i][j] - B.data[i][j]);
        }
    }
    return diff;
}
// etape 2
t_matrix subMatrix(t_matrix matrix, t_partition part, int compo_index) {
    t_class c = part.classes[compo_index];
    int k = c.n;

    t_matrix sub = createMatrix(k,k);

    for (int i = 0; i < k; i++) {
        int vi = c.sommets[i] - 1;

        for (int j = 0; j < k; j++) {
            int vj = c.sommets[j] - 1;

            sub.data[i][j] = matrix.data[vi][vj];
        }
    }

    return sub;
}