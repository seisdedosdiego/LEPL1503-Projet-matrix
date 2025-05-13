#ifdef __APPLE__
    #include "../headers/endian.h"
#else
    #include <endian.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../headers/file.h"
#include "../headers/matrix.h"

int main(int argc, char **argv) {
    
    // cas 1 - vecteur colonne ••••••••••••••••••••••••••••
    uint64_t m = 3;
    uint64_t n = 1;
    matrix *A = init_matrix(m, n);
    A->values[0][0] = -2.0;
    A->values[1][0] = 3.4;
    A->values[2][0] = 1.7;
    char *file_name = "matrix_column.bin";
    
    FILE *file = fopen(file_name, "wb");
    write_matrix(A, file);
    free_matrix(A);
    fclose(file);

    // cas 2 - vecteur ligne •••••••••••••••••••••••••••••
    m = 1;
    n = 3;
    matrix *B = init_matrix(m, n);
    B->values[0][0] = -3.0;
    B->values[0][1] = 4.3;
    B->values[0][2] = 7.1;
    char *name2 = "matrix_line.bin";

    FILE *f2 = fopen(name2, "wb");
    write_matrix(B, f2);
    free_matrix(B);
    fclose(f2);

    // cas 3 - matrice quelconque •••••••••••••••••••••••••
    m = 2;
    n = 3;
    matrix *C = init_matrix(m, n);
    C->values[0][0] = -3.0;
    C->values[0][1] = 4.3;
    C->values[0][2] = 7.1;
    C->values[1][0] = -4.0;
    C->values[1][1] = 4.2;
    C->values[1][2] = 6.9;
    char *name3 = "matrix_2x3.bin";

    FILE *f3 = fopen(name3, "wb");
    write_matrix(C, f3);
    free_matrix(C);
    fclose(f3);

    // cas 4 - matrice quelconque 3x2 •••••••••••••••••••••••
    m = 3;
    n = 2;
    matrix *D = init_matrix(m, n);
    D->values[0][0] = -1.0;
    D->values[0][1] = 2.3;
    D->values[1][0] = 8.1;
    D->values[1][1] = -3.0;
    D->values[2][0] = 1.2;
    D->values[2][1] = 4.2;
    char *name4 = "matrix_3x2.bin";

    FILE *f4 = fopen(name4, "wb");
    write_matrix(D, f4);
    free_matrix(D);
    fclose(f4);

    // cas 5 - matrice identité •••••••••••••••••••••••••
    m = 3;
    matrix *E = init_matrix(m, m);
    for (uint64_t i = 0 ; i < m ; i++) {
        E->values[i][i] = 1;
    }
    char *name5 = "matrix_I3.bin";

    FILE *f5 = fopen(name5, "wb");
    write_matrix(E, f5);
    free_matrix(E);
    fclose(f5);

    // cas 6 - matrice diagonale ••••••••••••••••••••••
    m = 3;
    double start = -2.0;
    matrix *F = init_matrix(m, m);
    for (uint64_t i = 0 ; i < m ; i++) {
        for (uint64_t j = i ; j < m ; j++) {
            F->values[i][j] = start;
            start += 1.0;
        }
    }           // valeurs des éléments : de -2 à 3
    char *name6 = "matrix_3x3_diag_sup.bin";

    FILE *f6 = fopen(name6, "wb");
    write_matrix(F, f6);
    free_matrix(F);
    fclose(f6);

    // cas 7 - matrice nulle •••••••••••••••••••••••••
    m = 3;
    n = 2;
    matrix *G = init_matrix(m, n);
    char *name7 = "matrix_3x2_zeroes.bin";

    FILE *f7 = fopen(name7, "wb");
    write_matrix(G, f7);
    free_matrix(G);
    fclose(f7);

    // cas 8 - matrice quelconque 3x3 •••••••••••••••••••••••
    m = 3;
    matrix *H = init_matrix(m, m);
    H->values[0][0] = -1.0;
    H->values[0][1] = 2.3;
    H->values[0][2] = 8.1;
    H->values[1][0] = -3.0;
    H->values[1][1] = 1.2;
    H->values[1][2] = 4.2;
    H->values[2][0] = -5.0;
    H->values[2][1] = 2.9;
    H->values[2][2] = 3.0;
    char *name8 = "matrix_3x3.bin";

    FILE *f8 = fopen(name8, "wb");
    write_matrix(H, f8);
    free_matrix(H);
    fclose(f8);    
    
}