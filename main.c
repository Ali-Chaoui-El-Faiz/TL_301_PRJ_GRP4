#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "matrix.h"

int main() {
    printf("VALIDATION ETAPE 1\n\n");

    // 1. Chargement du graphe et création de la matrice M
    //nAffichage de la matrice M associée à l'exemple météo
    t_list_adj graph = readGraph("exemple_meteo.txt");
    t_matrix M = graphToMatrix(&graph);

    printf("1 - Matrice Initiale M :\n");
    printMatrix(M);
    printf("\n");

    // 2. Calcul de M^3
    // Calcul de M^3, comparaison avec l'exemple
    t_matrix M2 = multiplyMatrices(M, M);   // M * M
    t_matrix M3 = multiplyMatrices(M2, M);  // M^2 * M

    printf("2 - Matrice M^3 (Previsions a 3 jours) :\n");
    printMatrix(M3);
    printf("\n");


    // 3. Calcul de M^7
    // Calcul de M^7, comparaison avec l'exemple
    t_matrix M4 = multiplyMatrices(M2, M2); // M^4
    t_matrix M7 = multiplyMatrices(M4, M3); // M^4 * M^3

    printf("3 - Matrice M^7 (Previsions a 7 jours - Stationnaire ?) :\n");
    printMatrix(M7);
    printf("\n");


    // 4. Test de convergence
    // Calculer M^n pour lequel la difference est < 0.01
    printf("4. Test de convergence (epsilon = 0.01) :\n");

    t_matrix prev = createMatrix(M.rows, M.cols);
    copyMatrix(&prev, M); // On commence avec M
    t_matrix current;

    double diff = 1.0;
    double epsilon = 0.01;
    int n = 1;

    // On boucle tant que la différence est trop grande
    while (diff > epsilon && n < 1000) {
        // Calcul de M^(n+1) = M^n * M
        current = multiplyMatrices(prev, M);

        // Calcul de la différence entre M^(n+1) et M^n
        diff = diffMatrix(current, prev);

        // Mise à jour pour le tour suivant
        copyMatrix(&prev, current);
        freeMatrix(&current); // On libère la mémoire
        n++;
    }

    printf("   >> Convergence atteinte a n = %d (diff = %.5f)\n", n, diff);

    // Libération mémoire
    freeMatrix(&M);
    freeMatrix(&M2);
    freeMatrix(&M3);
    freeMatrix(&M4);
    freeMatrix(&M7);
    freeMatrix(&prev);

    return 0;
}