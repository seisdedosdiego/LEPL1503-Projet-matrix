#ifdef __APPLE__
    #include "../headers/endian.h"
#else
    #include <endian.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../headers/file.h"
#include "../headers/vector.h"
#include "../headers/matrix.h"


int main(int argc, char **argv) {

    uint8_t nb_threads = 1;
    if (argc > 1) {
        nb_threads = (uint8_t)atoi(argv[1]);
        if (nb_threads < 1) {
            fprintf(stderr, "Erreur: le nombre de threads doit être supérieur à 0.\n");
            return EXIT_FAILURE;
        }
    }
    printf("Nombre de threads utilisés: %hhu\n",nb_threads);


    printf("Test de performance du programme pour une grande et une petite régression polynomiale\n");   
    
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    // Tests des performances générales du programme

    // cas 1 - tests: Petite regression polynomiale •

    /* Nombre d'observations */
    uint64_t n = 5;
    /* Pour une régression polynomiale d'ordre 2 :
       le polynôme est de degré 2 et la matrice A aura n lignes et (degree+1)=3 colonnes.
    */
    uint64_t degree = 4;
    if (degree + 1> n) {
        degree = n - 1;
    }
    /* Création des vecteurs de données : x et y */
    vector *x_data = init_vector(n);
    vector *y_data = init_vector(n);
    
    x_data->values[0] = 0.0; y_data->values[0] = -5.0;    
    x_data->values[1] = 1.0; y_data->values[1] = 6.0;    
    x_data->values[2] = 2.0; y_data->values[2] = 10.0;   
    x_data->values[3] = 3.0; y_data->values[3] = 34.0;   
    x_data->values[4] = 4.0; y_data->values[4] = 2.0;   
    
    
    /* Construction de la matrice A de Vandermonde.
       Pour chaque observation i:
         A[i][0] = 1
         A[i][1] = x_data->values[i]
         A[i][2] = (x_data->values[i])^2
    */
    matrix *A = init_matrix(n, degree + 1);

    for (uint64_t i = 0; i < n; i++) {
        for (uint64_t j = 0; j < degree + 1; j++) {
            A->values[i][j] = pow(x_data->values[i], j);
        }
    }
    
    /* Le vecteur b correspond aux valeurs y */
    vector *b = y_data;
    vector *coeff = init_vector(degree + 1);
    
    /* Appel de la fonction de régression pour résoudre A * solution = b */
    lstsq(A, b, coeff, nb_threads);

    free_matrix(A);

    FILE *f1 = fopen("vector_X.bin", "w+");
    write_vector(x_data, f1);
    free_vector(x_data);
    fclose(f1);

    FILE *f2 = fopen("vector_Y.bin", "w+");
    write_vector(y_data, f2);
    free_vector(y_data);
    fclose(f2);

    FILE *f3 = fopen("vector_a.bin", "w+");
    write_vector(coeff, f3);
    free_vector(coeff);
    fclose(f3);

    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Temps d'exécution: petite regression : %f secondes\n", cpu_time_used);


    // cas 2 - tests: Grande regression polynomiale •

    start = clock();
    /* Nombre d'observations */
    n = 1000;
    /* Pour une régression polynomiale d'ordre 9 :
       le polynôme est de degré 9 et la matrice A aura n lignes et (degree+1)=10 colonnes.
    */
    degree = 9;
    if (degree + 1> n) {
        degree = n - 1;
    }
    /* Création des vecteurs de données : x et y */
    x_data = init_vector(n);
    y_data = init_vector(n);
    for (uint64_t i = 0; i < n; i++) {
        x_data->values[i] = (double)i;
        y_data->values[i] = (double)(i * i);
    }
    
    A = init_matrix(n, degree + 1);
    for (uint64_t i = 0; i < n; i++) {
        for (uint64_t j = 0; j < degree + 1; j++) {
            A->values[i][j] = pow(x_data->values[i], j);
        }
    }

    /* Le vecteur b correspond aux valeurs y */
    b = y_data;
    coeff = init_vector(degree + 1);

    /* Appel de la fonction de régression pour résoudre A * solution = b */
    lstsq(A, b, coeff, nb_threads);

    free_matrix(A);

    FILE *f4 = fopen("vector_X.bin", "w+");
    write_vector(x_data, f4);
    free_vector(x_data);;
    fclose(f4);

    FILE *f5 = fopen("vector_Y.bin", "w+");
    write_vector(y_data, f5);
    free_vector(y_data);
    fclose(f5);

    FILE *f6 = fopen("vector_a.bin", "w+");
    write_vector(coeff, f6);
    free_vector(coeff);
    fclose(f6);

    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Temps d'exécution: grande regression : %f secondes\n", cpu_time_used);
    exit(EXIT_SUCCESS);
}