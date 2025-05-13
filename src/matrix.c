#include "../headers/matrix.h"
#include "../headers/vector.h"

pthread_mutex_t matrix_mutex = PTHREAD_MUTEX_INITIALIZER; 
/* ceci initialise le mutex en charge de protéger les fonctions liées aux vecteurs */

matrix *init_matrix(uint64_t m, uint64_t n) {
    matrix *A = (matrix *)malloc(sizeof(matrix));
    if (A == NULL) {
        fprintf(stderr,
                "Problème lors de l'allocation de l'espace mémoire pour une "
                "matrice : %s\n",
                strerror(errno));
        free(A); 
        exit(EXIT_FAILURE);
    }
    A->type = MATRIX;
    A->m = m;
    A->n = n;
    A->values = (double **)malloc(m * sizeof(double *));
    if (A->values == NULL) {
        fprintf(stderr,
                "Problème lors de l'allocation de l'espace mémoire pour une "
                "matrice : %s\n",
                strerror(errno));
        free_matrix(A); 
        exit(EXIT_FAILURE);
    }
    for (uint64_t i = 0; i < m; i++) {
        A->values[i] = (double *)malloc(n * sizeof(double));
        if (A->values[i] == NULL) {
            fprintf(
                stderr,
                "Problème lors de l'allocation de l'espace mémoire pour une "
                "matrice : %s\n",
                strerror(errno));
            free_matrix(A); 
            exit(EXIT_FAILURE);
        }
        for (uint64_t j = 0; j < n; j++) {
            A->values[i][j] = 0;
        }
    }
    return A;
}

void print_matrix(matrix *A) {
    if (A->m == 0) {
        printf("[[]]\n");
        return;
    }
    printf("[[%f", A->values[0][0]);
    for (uint64_t j = 1; j < A->n; j++) {
        printf(" %f", A->values[0][j]);
    }
    if (A->m == 1) {
        printf("]]\n");
        return;
    }
    printf("]\n");
    for (uint64_t i = 1; i < A->m; i++) {
        printf(" [%f", A->values[i][0]);
        for (uint64_t j = 1; j < A->n; j++) {
            printf(" %f", A->values[i][j]);
        }
        if (i == A->m - 1) {
            printf("]]\n");
            return;
        }
        printf("]\n");
    }
}

void free_matrix(matrix *M) {
    if (M == NULL) return;
    for (uint64_t i = 0; i < M->m; i++) {
        if (M->values[i] != NULL) {
            free(M->values[i]); 
        } 
    }
    if (M->values != NULL) {
        free(M->values);
    }
    free(M);
}

void *add_m_m_thread(void *args) { /* MULTITHREAD OK */
    thread_matrix *data = (thread_matrix *) args;
    for (uint64_t i = data->start; i < data->end; i++) {
        for (uint64_t j = 0; j < data->A->n; j++) {
            data->C->values[i][j] = data->A->values[i][j] + data->B->values[i][j];
            // pas de mutex utilisé ici car pas de superposition entre les threads
        }
    }
    return NULL;
}

int add_m_m(matrix *A, matrix *B, matrix *C, int8_t nb_threads) { /* MULTITHREAD OK */
    if (C == NULL) {
        fprintf(stderr, "La matrice C n'a pas été initialisée.\n");
        return -1;
    }
    if (A == NULL && B == NULL) {
        fprintf(stderr, "Les matrices A et B n'ont pas été initialisées.\n");
        return -1;
    }
    if (A == NULL && B != NULL) {
        if (C->m != B->m || C->n != B->n) {
            fprintf(stderr, "La matrice C n'a pas la même taille que B.\n");
            return -1;
        }
        for (uint64_t i = 0; i < B->m; i++) {
            for (uint64_t j = 0; j < B->n; j++) {
                C->values[i][j] = B->values[i][j];
            }
        }
        return 0;
    }
    if (B == NULL && A != NULL) {
        if (C->m != A->m || C->n != A->n) {
            fprintf(stderr, "La matrice C n'a pas la même taille que A.\n");
            return -1;
        }
        for (uint64_t i = 0; i < A->m; i++) {
            for (uint64_t j = 0; j < A->n; j++) {
                C->values[i][j] = A->values[i][j];
            }
        }
        return 0;
    }
    if (A->m != B->m || A->n != B->n) {
        fprintf(stderr, "Les deux matrices sont de tailles différentes.\n");
        return -1;
    }
    if (C->m != A->m || C->n != A->n) {
        fprintf(stderr, "La matrice C n'a pas la même taille que A et B.\n");
        return -1;
    }

    uint64_t m = A->m;
    pthread_t threads[nb_threads];
    thread_matrix data[nb_threads];
    nb_threads = (m == 0) ? 1 : nb_threads; // changé, pas sur de l'utilité
    uint64_t block = m/nb_threads;

    for (uint64_t k = 0 ; k < nb_threads ; k++) {
        data[k].A = A;
        data[k].B = B;
        data[k].C = C;
        data[k].start = k * block;
        data[k].end = (k == nb_threads - 1) ? m : (k + 1) * block;

        if (pthread_create(&threads[k], NULL, add_m_m_thread, (void *)&data[k]) != 0) {
            fprintf(stderr, "Erreur lors de la création du thread : %s\n", strerror(errno));
            return -1;
        }
    }

    for (uint64_t l = 0 ; l < nb_threads ; l++) {
        pthread_join(threads[l], NULL);
    }
    return 0;
}

// sub_m_m en MULTITHREADING
pthread_mutex_t mutex_m = PTHREAD_MUTEX_INITIALIZER;

void* sub_m_m_thread(void *args) {
    thread_matrix *data = (thread_matrix *)args;
    matrix *A = data->A;
    matrix *B = data->B;
    matrix *C = data->C;
    uint64_t start = data->start;
    uint64_t end = data->end;

    // pthread_mutex_lock(&mutex_m);  // bloque le mutex
    for (uint64_t i = start; i < end; i++) {
        for (uint64_t j = 0; j < A->n; j++) {
            C->values[i][j] = A->values[i][j] - B->values[i][j];
        }
    }
    // pthread_mutex_unlock(&mutex_m);  // débloque le mutex
    pthread_exit(NULL);
}

int sub_m_m(matrix *A, matrix *B, matrix *C, int8_t nb_threads){
    if (C == 0 || (A == 0 && B == 0)){
        fprintf(stderr,"La matrice C n'a pas été initialisé ou A et B n'ont pas été initalisés\n");
        return -1;
    }
    if (A == 0){
        for (uint64_t i = 0; i < B->m; i++) {
            for (uint64_t j = 0; j < B->n; j++) {
                C->values[i][j] =  - B->values[i][j];
            } 
        }
        return 0;
    }
    if (B == 0){
        for(uint64_t i = 0; i < A->m; i++){
            for(uint64_t j = 0; j < A->n; j++){
                C->values[i][j] = A->values[i][j];
            }
        }
        return 0;
    }
    if (A->m != B->m || A->n != B->n) {
        fprintf(stderr, "Les deux matrices sont de tailles différentes.\n");
        return -1;
    }
    if (C->m != A->m || C->n != A->n) {
        fprintf(stderr, "La matrice C n'a pas la même taille que A et B.\n");
        return -1;
    }
    if (nb_threads <= 0) {
        fprintf(stderr,"Erreur : nombre de threads non valide\n");
        return -1;
    }
    if (nb_threads > A->m) {
        fprintf(stdout,"Erreur : nombre de threads supérieur à la taille des matrices\n");
        nb_threads = (A->m == 0) ? 1 : A->m;                          // stdout car pas réellement une erreur qui empêche le code de tourner
    }
    pthread_t threads[nb_threads];
    thread_matrix data[nb_threads];

    int taille =  A->m / nb_threads;
    int reste = A->m % nb_threads;

    for(int i = 0; i < nb_threads; i++) {
        data[i].start = i * taille;
        data[i].end = (i+1) * taille;
        if (i == nb_threads - 1) {
            data[i].end += reste;
        }
        data[i].A = A;
        data[i].B = B;
        data[i].C = C;

        if (pthread_create(&threads[i], NULL, sub_m_m_thread, &data[i]) != 0) {
            fprintf(stderr, "Erreur lors de la création du thread : %s\n", strerror(errno));
            return -1;
        }
    }
    for (int i = 0; i < nb_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}


void *mult_m_m_thread(void *args) {
    thread_matrix *data = (thread_matrix *)args;
    matrix *A = data->A;
    matrix *B = data->B;
    matrix *C = data->C;
    for (uint64_t i = data->start; i < data->end; i++) {
        for (uint64_t j = 0; j < B->n; j++) {
            double sum = 0.0;
            for (uint64_t k = 0; k < A->n; k++) {
                sum += A->values[i][k] * B->values[k][j];
            }
            C->values[i][j] = sum;
        }
    }

    pthread_exit(NULL);
}

int mult_m_m(matrix *A, matrix *B, matrix *C, int8_t nb_threads){
    if (A == NULL || B == NULL || C == NULL) {
        fprintf(stderr, "La matrice A, B ou C n'a pas été initialisée.\n");
        return -1;
    }
    if (A->n != B->m) {
        fprintf(stderr, "Les dimensions des matrices A et B ne sont pas compatibles pour la multiplication.\n");
        return -1;
    }
    if(nb_threads <= 0){
        fprintf(stderr, "Erreur : nombre de threads non valide\n");
        return -1;
    }
    if(nb_threads > A->m){
        nb_threads = (A->m == 0) ? 1 : A->m;
        fprintf(stdout, "Message système : nombre de threads borné par le nombre de ligne de A, on le réduit\n"); //%lu de merde
    }                                                        
    uint64_t m = A->m;
    uint64_t n = B->n;

    if (C->m != m || C->n != n) {
        fprintf(stderr, "Message système : Dimensions de C non compatibles\n"); 
        return -1;
    }

    pthread_t threads[nb_threads];
    thread_matrix data [nb_threads];

    uint64_t rows_per_thread = A->m / nb_threads;
    uint64_t remaining_rows = A->m % nb_threads;
    uint64_t current_row = 0;

    for (uint64_t i = 0; i < nb_threads; i++) {
        data[i].A = A;
        data[i].B = B;
        data[i].C = C;
        data[i].start = current_row;
        data[i].end = current_row + rows_per_thread + (i < remaining_rows ? 1 : 0);
        if (pthread_create(&threads[i], NULL, mult_m_m_thread, &data[i]) != 0) {
            fprintf(stderr, "Erreur lors de la création du thread : %s\n", strerror(errno));
            return -1;
        }
        current_row = data[i].end;
    }

    for (uint64_t i = 0; i < nb_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}

void *transp_thread(void *args) {
    thread_matrix *data = (thread_matrix *) args;
    for (uint64_t j = 0; j < data->A->n; j++) {
        for (uint64_t i = data->start; i < data->end; i++) {
            data->C->values[j][i] = data->A->values[i][j];
        } // pas de mutex utilisé ici car pas de superposition entre les threads
    }
    return NULL;
}

int transp(void *oldA, void *C, int8_t nb_threads) {
    if (oldA == NULL || C == NULL) {
        fprintf(stderr, "Erreur : Matrice source ou destination NULL\n");
        return -1;
    }
    matrix *A = (matrix *)oldA;
    int needs_free = 0;
    

    if (A->type != MATRIX) {
        vector *v = (vector *)oldA;
        if (v->type != VECTOR || v->m == 0) {
            fprintf(stderr, "Erreur : objet ni matrice ni vecteur valide\n");
            return -1;
        }
        
        C=oldA;
        return 0;

        A = init_matrix(v->m, 1);
        if (!A) {
            fprintf(stderr, "Erreur : allocation échouée pour la matrice transposée\n");
            return -1;
        }

        for (uint64_t i = 0; i < v->m; i++) {
            A->values[i][0] = v->values[i];
        }
        needs_free = 1;
    }
    matrix * newC = (matrix *)C;

    if (A->m != newC->n || A->n != newC->m) {
        fprintf(stderr, "Erreur : dimensions incompatibles A (%lux%lu), C (%lux%lu)\n",
                A->m, A->n, newC->m, newC->n);
        if (needs_free) free_matrix(A);
        return -1;
    }

    if (nb_threads > A->m) {
        fprintf(stdout, "Erreur : nombre de threads supérieur à la taille de la matrice A, il est réduit\n");
        nb_threads = A->m;
    }
    if (nb_threads <= 0) {
        fprintf(stderr, "Erreur : nombre de threads invalide (%d)\n", nb_threads);
        return -1;
    }


    pthread_t threads[nb_threads];
    thread_matrix data[nb_threads];
    uint64_t block = A->m / nb_threads;

    for (int k = 0; k < nb_threads; k++) {
        data[k].A = A;
        data[k].C = C;
        data[k].start = k * block;
        data[k].end = (k == nb_threads - 1) ? A->m : (k + 1) * block;

        if (pthread_create(&threads[k], NULL, transp_thread, &data[k]) != 0) {
            fprintf(stderr, "Erreur pthread_create: %s\n", strerror(errno));
            if (needs_free) free_matrix(A);
            return -1;
        }
    }

    for (int k = 0; k < nb_threads; k++) {
        pthread_join(threads[k], NULL);
    }

    if (needs_free) free_matrix(A);
    return 0;
}



int qr(matrix *A, matrix *Q, matrix *R, int8_t nb_threads) {
    if (!A || !Q || !R) {
        fprintf(stderr, "La matrice A, Q ou R n'a pas été initialisée.\n");
        return -1;
    }

    uint64_t m = A->m;
    uint64_t n = A->n;
    if (m < n) {
        fprintf(stderr, "Nous traitons seulement les matrices A ∈ R^(m*n) avec m >= n.\n");
        return -1;
    }
    if (Q->m != m || Q->n != n || R->m != n || R->n != n) {
        fprintf(stderr, "Les dimensions de Q ou R ne correspondent pas.\n");
        return -1;
    }
    uint64_t count = 0;
    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            if (A->values[i][j] != 0) {
                count++;
            }
        }
    }
    if (count == 0) {
        for(uint64_t i = 0; i < m; i++) {
            for (uint64_t j = 0; j < n; j++) {
                Q->values[i][j] = 0;
            
            }

        }
        for (uint64_t i = 0; i < n; i++) {
            for (uint64_t j = 0; j < n; j++) {
                R->values[i][j] = 0;
            }
        }
        return 0;
    }

    // Initialisation de Q ← A (copie colonne par colonne)
    for (uint64_t j = 0; j < n; j++) {
        for (uint64_t i = 0; i < m; i++) {
            Q->values[i][j] = A->values[i][j];
        }

        // Orthonormalisation (Gram-Schmidt modifié)
        for (uint64_t k = 0; k < j; k++) {
            double r = 0.0;
            for (uint64_t i = 0; i < m; i++) {
                r += Q->values[i][k] * Q->values[i][j];
            }
            R->values[k][j] = r;
            for (uint64_t i = 0; i < m; i++) {
                Q->values[i][j] -= r * Q->values[i][k];
            }
        }

        // Norme de la colonne j
        double norm_col = 0.0;
        for (uint64_t i = 0; i < m; i++) {
            norm_col += Q->values[i][j] * Q->values[i][j];
        }
        norm_col = sqrt(norm_col);
        if (norm_col < DBL_EPSILON) {
            fprintf(stderr, "Colonne %lu de A est linéairement dépendante des précédentes (norme nulle).\n", j);
            return -1;
        }

        R->values[j][j] = norm_col;
        for (uint64_t i = 0; i < m; i++) {
            Q->values[i][j] /= norm_col;
        }
    }

    return 0;
}


int is_upper_triang(matrix *U) {
    if (U == NULL) {
        fprintf(stderr, "La matrice U n'a pas été initialisée.\n");
        return -1;
    }
    if (U->m != U->n) {
        fprintf(stderr, "La matrice U n'est pas carrée.\n");
        return -1;
    }

    for (uint64_t i = 0; i < U->m; i++) {
        for (uint64_t j = 0; j < i; j++) {
            if (fabs(U->values[i][j]) > 1e-10) {
                return -1;
            }
        }
    }
    return 0;
}