#ifdef __APPLE__
    #include "../headers/endian.h"
#else
    #include <endian.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../headers/file.h"
#include "../headers/vector.h"

int main(int argc, char **argv) {
    
    // cas 1 - vecteur quelconque ••••••••••••••••••••••••••••••
    uint64_t m = 3;
    vector *v = init_vector(m);
    v->values[0] = -2.0;
    v->values[1] = 3.4;
    v->values[2] = 1.7;
    char *file_name = "vector_3.bin";

    FILE *file = fopen(file_name, "w+");
    write_vector(v, file);
    free(v->values);
    free(v);
    fclose(file);

    // cas 2 - vecteur nul ••••••••••••••••••••••••••••••••••••
    m = 3;
    vector *w = init_vector(m);
    char *name2 = "vector_3_zeroes.bin";

    FILE *f2 = fopen(name2, "w+");
    write_vector(w, f2);
    free(w->values);
    free(w);
    fclose(f2);

    // cas 3 - long vecteur •••••••••••••••••••••••••••••••••••
    m = 1000000;
    vector *z = init_vector(m);
    double val = 1.0;
    for (uint64_t i = 0 ; i < m ; i++) {
        z->values[i] = val;
        val += 1.0;
    }
    char *name3 = "vector_long.bin";

    FILE *f3 = fopen(name3, "w+");
    write_vector(z, f3);
    free(z->values);
    free(z);
    fclose(f3);

    // cas 4 - vecteur négatif •••••••••••••••••••••••••••••••
    m = 3;
    vector *y = init_vector(m);
    val = -110.0;
    for (uint64_t i = 0 ; i < m ; i++) {
        y->values[i] = val;
        val += 1.0;
    }
    char *name4 = "vector_3_neg.bin";

    FILE *f4 = fopen(name4, "w+");
    write_vector(y, f4);
    free(y->values);
    free(y);
    fclose(f4);

    // cas 5 - tests finaux

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
    lstsq(A, b, coeff, 1);

    char *a5 = "vector_X.bin";
    char *a6 = "vector_Y.bin";
    char *a7 = "vector_a.bin";

    FILE *f5 = fopen(a5, "w+"); 
    write_vector(x_data, f5);
    free(x_data->values);
    free(x_data);
    fclose(f5);

    FILE *f6 = fopen(a6, "w+");
    write_vector(y_data, f6);
    free(y_data->values);
    free(y_data);
    fclose(f6);

    FILE *f7 = fopen(a7, "w+");
    write_vector(coeff, f7);
    free(coeff->values);
    free(coeff);
    free_matrix(A);
    fclose(f7);
    

    // Cas 6 - vecteur verylong

    m = 100000000;
    z = init_vector(m);
    val = 1.0;
    for (uint64_t i = 0 ; i < m ; i++) {
        z->values[i] = val;
        val += 1.0;
    }
    name3 = "vector_verylong.bin";
    f3 = fopen(name3, "w+");
    write_vector(z, f3);
    free(z->values);
    free(z);
    fclose(f3);

}