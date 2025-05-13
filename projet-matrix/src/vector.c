#include "../headers/vector.h"
#include "../headers/matrix.h"

#include <math.h>

pthread_mutex_t mutex_v = PTHREAD_MUTEX_INITIALIZER; 
/* ceci initialise le mutex en charge de protéger les fonctions liées aux vecteurs */

vector *init_vector(uint64_t m) {
    vector *v = (vector *)malloc(sizeof(vector));
    if (v == NULL) {
        fprintf(
            stderr,
            "Problème lors de l'allocation de l'espace mémoire pour un vecteur "
            ": %s\n",
            strerror(errno));
        exit(EXIT_FAILURE);
    }
    v->type= VECTOR;
    v->m = m;
    v->values = (double *)malloc(m * sizeof(double));
    if (v->values == NULL) {
        fprintf(
            stderr,
            "Problème lors de l'allocation de l'espace mémoire pour un vecteur "
            ": %s\n",
            strerror(errno));
        exit(EXIT_FAILURE);
    }
    for (uint64_t i = 0; i < m; i++) {
        v->values[i] = 0;
    }
    return v;
}

void free_vector(vector *v) {
    if (v == NULL) return;
    if (v->values != NULL) {
        free(v->values);
    }
    free(v);
}

void print_vector(vector *v) {
    if (v->m == 0) {
        printf("[]\n");
        return;
    }
    printf("[%f", v->values[0]);
    for (uint64_t i = 1; i < v->m; i++) {
        printf(" %f", v->values[i]);
    }
    printf("]\n");
}

void *add_v_v_thread(void *args) { /* MULTITHREAD OK */
    thread_vector *data = (thread_vector *) args;
    for (uint64_t i = data->start; i < data->end; i++) {
        // pas de mutex utilisé ici car pas de superposition entre les threads
        data->z->values[i] = data->x->values[i] + data->y->values[i];
    }
    return NULL;
}

int add_v_v(vector *x, vector *y, vector *z, int8_t nb_threads) { /* MULTITHREAD OK */
    if (z == NULL) {
        fprintf(stderr, "Le vecteur z n'a pas été initialisé.\n");
        return -1;
    }
    if (x == NULL && y == NULL) {
        fprintf(stderr, "Les vecteurs x et y n'ont pas été initialisés.\n");
        return -1;
    }
    if (x == NULL && y != NULL){
        for(uint64_t i = 0; i < y->m; i++){
            z->values[i] = y->values[i];
        }
        return 0;
    }
    if (y == NULL && x != NULL){
        for(uint64_t i = 0; i < x->m; i++){
            z->values[i] = x->values[i];
        }
        return 0;
    }
    if (x->m != y->m) {
        fprintf(stderr, "Les deux vecteurs sont de tailles différentes.\n");
        return -1;
    }
    if (x->m == 0 && y->m == 0) {
        free(z->values);
        z->values = NULL;
        z->m = 0;
        return 0;
    }
    if (z->m != x->m) {
        fprintf(stderr, "Le vecteur z n'a pas la même taille que les vecteurs x et y.\n");
        return -1;
    }
    if (nb_threads <= 0) {
        fprintf(stderr,"Erreur : nombre de threads non valide ou zéro\n");
        return -1;
    }
    if (nb_threads > x->m) {
        fprintf(stdout, "Le nombre de threads dépasse la taille du vecteur ; il est réduit.\n"); 
        nb_threads = (x->m == 0) ? 1 : x->m;                // stdout car pas réellement une erreur qui empêche le code de tourner
    }

    uint64_t m = x->m;
    pthread_t threads[nb_threads];
    thread_vector data[nb_threads];
    uint64_t block = m/nb_threads;

    for (uint64_t i = 0; i < nb_threads; i++) {
        data[i].x = x;
        data[i].y = y;
        data[i].z = z;
        data[i].start = i * block;
        data[i].end = (i == nb_threads - 1) ? m : (i + 1) * block; /* assure que le dernier thread prenne toutes les valeurs restantes */

        if (pthread_create(&threads[i], NULL, add_v_v_thread, (void *)&data[i]) != 0) {
            fprintf(stderr, "Erreur lors de la création du thread : %s\n", strerror(errno));
            return -1;
        }
    }

    for (uint64_t j = 0; j < nb_threads; j++) {
        pthread_join(threads[j], NULL);
    }
    return 0;
}

//sub_v_v en MULTITHREADING

void* sub_v_v_thread(void *args) {
    thread_vector *data = (thread_vector*) args;

    vector *x = data->x;
    vector *y = data->y;
    vector *z = data->z;
    uint64_t start = data->start;
    uint64_t end = data->end;

    // pthread_mutex_lock(&mutex_v);  // bloque le mutex
    for (uint64_t i = start; i < end; i++) {
        z->values[i] = x->values[i] - y->values[i];
    }
    // pthread_mutex_unlock(&mutex_v);  // bloque le mutex
    pthread_exit(NULL);
}

int sub_v_v(vector *x, vector *y, vector *z, int8_t nb_threads) {
    if (z == NULL) {
        fprintf(stderr, "Le vecteur z n'a pas été initialisé.\n");
        return -1;
    }
    if (x == NULL && y == NULL) {
        fprintf(stderr, "Les vecteurs x et y n'ont pas été initialisés.\n");
        return -1;
    }
    if (x == NULL && y != NULL){
        for (uint64_t i = 0; i < y->m; i++) {
            z->values[i] = - y->values[i];
        }
        return 0;
    }
    if (y == NULL){
        for(uint64_t i = 0; i < x->m; i++){
            z->values[i] = x->values[i];
        }
        return 0;
    }
    if (x->m != y->m) {
        fprintf(stderr, "Les deux vecteurs sont de tailles différentes.\n");
        return -1;
    }
    if (x->m == 0 && y->m == 0) {
        free(z->values);
        z->values = NULL;
        z->m = 0;
        return 0;
    }
    if (z->m != x->m) {
        fprintf(stderr, "Le vecteur z n'a pas la même taille que les vecteurs x et y.\n");
        return -1;
    }
    if (nb_threads <= 0) {
        fprintf(stderr,"Erreur : nombre de threads non valide\n");
        return -1;
    }
    if (nb_threads > x->m) {
        fprintf(stdout,"Erreur : nombre de threads supérieur à la taille des vecteurs ; il est réduit\n");
        nb_threads = (x->m == 0) ? 1 : x->m;                                   // stdout car pas réellement une erreur qui empêche le code de tourner
    }
    
    pthread_t threads[nb_threads];
    thread_vector data[nb_threads];

    int taille =  x->m / nb_threads;
    int reste = x->m % nb_threads;

    for(int i = 0; i < nb_threads; i++) {
        data[i].start = i * taille;
        data[i].end = (i+1) * taille;
        if (i == nb_threads - 1) {
            data[i].end += reste;
        }

        data[i].x = x;
        data[i].y = y;
        data[i].z = z; 

        if (pthread_create(&threads[i], NULL, sub_v_v_thread, &data[i]) != 0) {
            fprintf(stderr, "Erreur lors de la création du thread : %s\n", strerror(errno));
            return -1;
        }

    }
    for (int i = 0; i < nb_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}

// dot_prod en MULTITHREADING

void* dot_prod_thread(void *args) {
    thread_vector *data = (thread_vector*) args;
    vector *x = data->x;
    vector *y = data->y;
    uint64_t start = data->start;
    uint64_t end = data->end;
    double partial_result = 0.0;

    for (uint64_t i = start; i < end; i++) {
        partial_result += x->values[i] * y->values[i];
    }

    data->result = partial_result;
    return NULL;  // pthread_exit(NULL); est inutile ici
}

double dot_prod(vector *x, vector *y, int8_t nb_threads) {
    if (x == NULL || y == NULL) {
        fprintf(stderr, "Erreur : vecteur NULL\n");
        return NAN;
    }
    if (x->m != y->m) {
        fprintf(stderr, "Erreur : tailles des vecteurs non concordantes\n");
        return NAN;
    }
    if (x->m == 0) return 0.0;

    if (nb_threads <= 0) {
        fprintf(stderr, "Erreur : nombre de threads invalide\n");
        return NAN;
    }
    if (nb_threads > x->m) {
        fprintf(stdout, "Erreur : nombre de threads supérieur à la taille des vecteurs ou nombre de thread négatif ; il est réduit.\n");
        nb_threads = x->m;
    }

    pthread_t threads[nb_threads];
    thread_vector data[nb_threads];

    int base = x->m / nb_threads;
    int reste = x->m % nb_threads;

    uint64_t current = 0;
    for (int i = 0; i < nb_threads; ++i) {
        data[i].start = current;
        data[i].end = current + base + (i < reste ? 1 : 0);
        current = data[i].end;

        data[i].x = x;
        data[i].y = y;
        data[i].result = 0.0;

        if (pthread_create(&threads[i], NULL, dot_prod_thread, &data[i]) != 0) {
            fprintf(stderr, "Erreur création thread %d : %s\n", i, strerror(errno));
            return NAN;
        }
    }

    double resultat = 0.0;
    for (int i = 0; i < nb_threads; ++i) {
        pthread_join(threads[i], NULL);//attendre que tous les threads soient terminés
        resultat += data[i].result;// additionne le résultat de chaque thread    
    }

    if (!isfinite(resultat)) {
        fprintf(stdout, "Overflow dans dot_prod : produit scalaire infini\n");
        return resultat; 
    }

    return resultat;
}

void *norm_thread(void *args) {
    thread_vector *data = (thread_vector *) args;
    double local_norm = 0.0;

    for (uint64_t i = data->start; i < data->end; i++) {
        local_norm = hypot(local_norm, data->x->values[i]);  // √(a^2 + b^2) 
    }
    data->result = local_norm * local_norm;  // stocker la somme des carrés
    return NULL;
}


double norm(vector *v, int8_t nb_threads) {
    if (v == NULL) {
        fprintf(stderr, "Le vecteur n'a pas été initialisé.\n");
        return NAN;
    }
    if (v->m == 0) return 0.0;
    for (uint64_t i = 0; i < v->m; i++){
        if (isnan(v->values[i]) || isinf(v->values[i])) return fabs(v->values[i]); 
    }
    if (nb_threads <= 0) {
        fprintf(stderr, "Le nombre de threads doit être supérieur à O.\n");
        return NAN;
    }
    if (nb_threads > v->m) {
        nb_threads = v->m;  
        fprintf(stdout, "Le nombre de threads dépasse la taille du vecteur ; il est réduit.");
                                    
    }

    uint64_t m = v->m;
    pthread_t threads[nb_threads];
    thread_vector data[nb_threads];
    uint64_t block = v->m/nb_threads;
    double sum = 0.0;

    for (uint64_t i = 0; i < nb_threads; i++) {
        data[i].x = v;
        data[i].result = 0.0;
        data[i].start = i *block;
        data[i].end = (i == nb_threads - 1) ? m : (i + 1) * block;
        if (pthread_create(&threads[i], NULL, norm_thread, (void *) &data[i]) != 0) {
            fprintf(stderr, "Erreur lors de la création du thread : %s\n", strerror(errno));
            return NAN;
        }
    }

    for (uint64_t j = 0; j < nb_threads; j++) {
        pthread_join(threads[j], NULL);
        sum += data[j].result;
    }

    return sqrt(sum);
}

void *mult_m_v_thread(void *args){
    thread_vector *data = (thread_vector *)args;
    matrix *A = data->A;
    vector *v = data->x;
    vector *res = data->z;
    for (uint64_t i = data->start; i < data->end; i++) {
        double sum = 0.0;
        for (uint64_t j = 0; j < A->n; j++) {
            sum += A->values[i][j] * v->values[j];
        }
        pthread_mutex_lock(&mutex_v);
        res->values[i] = sum;
        pthread_mutex_unlock(&mutex_v);
    }

    pthread_exit(NULL);
}

int mult_m_v(matrix *A, vector *v, vector *res, int8_t nb_threads){
    if (A == NULL || v == NULL || res == NULL) {
        fprintf(stderr, "La matrice ou un des vecteurs n'a pas été initialisé.\n");
        return -1;
    }
    if (A->n != v->m) {
        fprintf(stderr, "Taille incompatible entre la matrice et le vecteur initial \n");
        return -1;
    }
    if (A->m == 0 || A->n == 0 || v->m == 0) {
        fprintf(stdout, "Arguments vides");
        return -1;
    }
    if(nb_threads <= 0){
        fprintf(stderr, "Erreur : nombre de threads non valide\n");
        return -1;
    }
    if(nb_threads > A->m){
        nb_threads = (A->m == 0) ? 1 : A->m;  
        fprintf(stdout, "Message système : nombre de threads borné par le nombre de ligne de A, on le réduit.\n");
    }                                                                       
    
    if (res->m != A->m) {
        fprintf(stdout, "Message système : Dimensions de result non compatibles, redimensionnement automatique.\n"); 
        free(res->values);                                                      
        res->values = (double *)malloc(A->m * sizeof(double));
        if (!res->values) {
            fprintf(stderr, "Erreur d'allocation mémoire pour result\n");
            return -1;
        }
        res->m = A->m;
    }

    pthread_t threads[nb_threads];
    thread_vector data [nb_threads];

    uint64_t rows_per_thread = A->m / nb_threads;
    uint64_t remaining_rows = A->m % nb_threads;
    uint64_t current_row = 0;

    for (uint64_t i = 0; i < nb_threads; i++) {
        data[i].A = A;
        data[i].x = v;
        data[i].z = res;
        data[i].start = current_row;
        data[i].end = current_row + rows_per_thread + (i < remaining_rows ? 1 : 0);
        if (pthread_create(&threads[i], NULL, mult_m_v_thread, &data[i]) != 0) { // et pas enum
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

int back_sub(matrix *U, vector *v, vector *w) {
    if (U == NULL || v == NULL || w == NULL) {
        fprintf(stderr, "La matrice U ou les vecteurs v ou w n'ont pas été initialisés.\n");
        return -1;
    }
    if (is_upper_triang(U) == -1) {
        fprintf(stderr, "La matrice U n'est pas triangulaire supérieure.\n");
        return -1;
    }
    if (U->m != v->m || v->m != w->m) {
        fprintf(stderr, "Les tailles de la matrice U et des vecteurs v et w ne sont pas compatibles.\n");
        return -1;
    }

    int infinite_solutions = 0;
    for (int i = (int)(U->m) - 1; i >= 0; i--) {
        if (fabs(U->values[i][i]) < DBL_EPSILON) {
            if (fabs(v->values[i]) > DBL_EPSILON) {
                fprintf(stderr, "Aucune solution : pivot nul et valeur non nulle dans le vecteur.\n");
                return -1;
            } else {
                w->values[i] = 0; // ou autre valeur arbitraire
                infinite_solutions = 1;
                continue;
            }
        }
        w->values[i] = v->values[i];
        for (int j = i + 1; j < U->n; j++) {
            if ((fabs(U->values[i][j]) == 0 && isinf(fabs(w->values[j]))) ||  // gestion des cas 0 * ∞
                (isinf(fabs(U->values[i][j])) && fabs(w->values[j]) == 0)) { 
                w->values[i] = INFINITY;
            } else {
                w->values[i] -= U->values[i][j] * w->values[j];
            }
        }
        w->values[i] /= U->values[i][i];
    }

    if (infinite_solutions) {
        fprintf(stderr, "Infinité de solutions détectée.\n");
    }
    return 0;
}


int lstsq(matrix *A, vector *b, vector *x, int8_t nb_threads) {
    if (A == NULL || b == NULL || x == NULL) {
        fprintf(stderr, "La matrice A ou les vecteurs b ou x n'ont pas été initialisés.\n");
        return -1;
    }
    if (A->m != b->m) {
        fprintf(stderr, "Les dimensions de la matrice A et du vecteur b ne correspondent pas.\n");
        return -1;
    }
    if (A->n != x->m) {
        fprintf(stderr, "Le vecteur x ne correspond pas au nombre de colonnes de A.\n");
        return -1;
    }
    if (A->m < A->n) {
        fprintf(stderr, "La matrice A n'est pas surdéterminée (m <= n).\n");
        return -1;
    }
    if (A->m == 0 || A->n == 0 || b->m == 0 || x->m == 0) {
        fprintf(stderr, "Erreur : matrice ou vecteur vide.\n");
        return -1;
    }

    matrix *Q = init_matrix(A->m, A->n);
    if (Q == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour la matrice Q.\n");
        return -1;
    }
    matrix *R = init_matrix(A->n, A->n);
    if (R == NULL) {
        free_matrix(Q);
        fprintf(stderr, "Erreur d'allocation mémoire pour la matrice R.\n");
        return -1;
    }
    if (qr(A, Q, R, nb_threads) == -1) {
        free_matrix(Q);
        free_matrix(R);
        return -1;
    }

    matrix *QT = init_matrix(Q->n, Q->m);
    if (QT == NULL) {
        free_matrix(Q);
        free_matrix(R);
        fprintf(stderr, "Erreur d'allocation mémoire pour la matrice QT.\n");
        return -1;
    }
    if(transp(Q, QT, nb_threads)==-1){
        free_matrix(Q);
        free_matrix(R);
        free_matrix(QT);
        return -1;
    }
    vector *QTb = init_vector(QT->m); // A->n
    if (QTb == NULL) {
        free_matrix(Q);
        free_matrix(R);
        free_matrix(QT);
        fprintf(stderr, "Erreur d'allocation mémoire pour le vecteur QTb.\n");
        return -1;
    }
    if (mult_m_v(QT, b, QTb, nb_threads)==-1) {
        free_matrix(Q);
        free_matrix(R);
        free_matrix(QT);
        free_vector(QTb);
        return -1;
    }

    int back_result = back_sub(R, QTb, x);

    free_matrix(Q);
    free_matrix(R);
    free_matrix(QT);
    free_vector(QTb);

    return back_result;
}