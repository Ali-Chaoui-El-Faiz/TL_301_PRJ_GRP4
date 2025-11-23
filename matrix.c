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

//   ETAPE 3 E
// PGCD
int gcd(int *vals, int nbvals) {
    if (nbvals == 0) return 0;

    int result = vals[0];

    for (int i = 1; i < nbvals; i++) {
        int a = result;
        int b = vals[i];

        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        result = a;
    }

    return result;
}

// Calcule periode
int getPeriod(t_matrix sub_matrix) {
    int n = sub_matrix.rows;

    int *periods = malloc(n * sizeof(int));
    int period_count = 0;

    t_matrix power = createMatrix(n, n);
    t_matrix next = createMatrix(n, n);
    copyMatrix(&power, sub_matrix);

    for (int k = 1; k <= n; k++) {
        int diag_nonzero = 0;

        for (int i = 0; i < n; i++) {
            if (power.data[i][i] > 0.0) {
                diag_nonzero = 1;
                break;
            }
        }

        if (diag_nonzero) {
            periods[period_count] = k;
            period_count++;
        }

        t_matrix tmp = multiplyMatrices(power, sub_matrix);
        copyMatrix(&next, tmp);
        freeMatrix(&tmp);
        copyMatrix(&power, next);
    }

    int result = gcd(periods, period_count);

    free(periods);
    freeMatrix(&power);
    freeMatrix(&next);

    return result;
}

t_matrix stationaryDistribution(t_matrix mat) {
    int n = mat.rows;

    // Distribution uniforme initiale
    t_matrix dist = createMatrix(1, n);
    double init_val = 1.0 / n;
    for (int j = 0; j < n; j++) {
        dist.data[0][j] = init_val;
    }

    double epsilon = 1e-8;
    double diff;
    int max_iter = 1000;

    for (int iter = 0; iter < max_iter; iter++) {
        t_matrix dist_next = createMatrix(1, n);

        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int i = 0; i < n; i++) {
                sum += dist.data[0][i] * mat.data[i][j];
            }
            dist_next.data[0][j] = sum;
        }

        diff = 0.0;
        for (int j = 0; j < n; j++) {
            diff += fabs(dist_next.data[0][j] - dist.data[0][j]);
        }

        // Copie manuelle des donnees
        for (int j = 0; j < n; j++) {
            dist.data[0][j] = dist_next.data[0][j];
        }

        freeMatrix(&dist_next);

        if (diff < epsilon) {
            break;
        }
    }

    return dist;
}

t_matrix* periodicStationaryDistributions(t_matrix sub_matrix, int period, int* num_distributions) {
    *num_distributions = period;
    t_matrix* distributions = malloc(period * sizeof(t_matrix));

    if (period == 1) {
        // Cas apériodique
        t_matrix dist = stationaryDistribution(sub_matrix);
        distributions[0] = createMatrix(1, sub_matrix.cols);
        for (int j = 0; j < sub_matrix.cols; j++) {
            distributions[0].data[0][j] = dist.data[0][j];
        }
        freeMatrix(&dist);
        return distributions;
    }

    // Cas periodique
    t_matrix M_period = createMatrix(sub_matrix.rows, sub_matrix.cols);
    copyMatrix(&M_period, sub_matrix);

    for (int i = 1; i < period; i++) {
        t_matrix temp = multiplyMatrices(M_period, sub_matrix);
        copyMatrix(&M_period, temp);
        freeMatrix(&temp);
    }

    t_matrix base_dist = stationaryDistribution(M_period);
    distributions[0] = createMatrix(1, sub_matrix.cols);
    for (int j = 0; j < sub_matrix.cols; j++) {
        distributions[0].data[0][j] = base_dist.data[0][j];
    }
    freeMatrix(&base_dist);

    for (int phase = 1; phase < period; phase++) {
        distributions[phase] = createMatrix(1, sub_matrix.cols);
        for (int j = 0; j < sub_matrix.cols; j++) {
            double sum = 0.0;
            for (int i = 0; i < sub_matrix.rows; i++) {
                sum += distributions[phase-1].data[0][i] * sub_matrix.data[i][j];
            }
            distributions[phase].data[0][j] = sum;
        }
    }

    freeMatrix(&M_period);
    return distributions;
}