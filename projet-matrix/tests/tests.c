#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "../headers/matrix.h"
#include "../headers/vector.h"
#include "../headers/file.h"

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    VALIDITÉ DES RÉSULTATS    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
uint8_t nb_threads = 1;
void test_add_v_v(void) {
    uint64_t m = 30;

    // Vecteurs valides
    vector *v = init_vector(m); 
    vector *w = init_vector(m);
    vector *z = init_vector(m); 

    // Vecteurs de taille différente
    vector *v2 = init_vector(m + 10);
    vector *z2 = init_vector(m + 10);

    // Initialisation avec des valeurs aléatoires
    for (uint64_t i = 0; i < m; i++) {
        v->values[i] = (double)rand() / RAND_MAX;
        w->values[i] = (double)rand() / RAND_MAX;
    }
    
    // Cas 1: Addition de deux vecteurs valides
    int t1 = add_v_v(v, w, z, nb_threads);
    CU_ASSERT_EQUAL(t1, 0);
    for (uint64_t i = 0; i < m; i++) {
        CU_ASSERT_DOUBLE_EQUAL(z->values[i], v->values[i] + w->values[i], 1e-3);
    }

    // Cas 2: x et y de tailles différentes
    int t2 = add_v_v(v2, w, z, nb_threads);
    CU_ASSERT_EQUAL(t2, -1);

    // Cas 3 : z de taille différente
    int t3 = add_v_v(v, w, z2, nb_threads);
    CU_ASSERT_EQUAL(t3, -1);

    // Cas 4 : y est NULL
    int t4 = add_v_v(v, NULL, z, nb_threads);
    CU_ASSERT_EQUAL(t4, 0);
    for (uint64_t i = 0; i < m; i++) {
        CU_ASSERT_DOUBLE_EQUAL(z->values[i], v->values[i], 1e-3);
    }

    // Cas 5 : x est NULL
    int t5 = add_v_v(NULL, w, z, nb_threads);
    CU_ASSERT_EQUAL(t5, 0);
    for (uint64_t i = 0; i < m; i++) {
        CU_ASSERT_DOUBLE_EQUAL(z->values[i], w->values[i], 1e-3);
    }

    // Cas 6 : x et y sont NULL
    int t6 = add_v_v(NULL, NULL, z, nb_threads);
    CU_ASSERT_EQUAL(t6, -1);

    // Cas 7 : z est NULL
    int t7 = add_v_v(v, w, NULL, nb_threads);
    CU_ASSERT_EQUAL(t7, -1);

    // Cas 8 : x et y de taille 0 (vides)
    vector *zero = init_vector(0);
    vector *exitzero = init_vector(5);
    int t8 = add_v_v(zero, zero, exitzero, nb_threads);
    CU_ASSERT_EQUAL(t8, 0);
    CU_ASSERT_EQUAL(exitzero->m, 0);

    // Cas 9 : nb_threads <= 0 
    int t9 = add_v_v(v, w, z, -5);
    CU_ASSERT_EQUAL(t9, -1);
    t9 = add_v_v(v, w, z, 0);
    CU_ASSERT_EQUAL(t9, -1);

    // Cas 10 = nb_threads > m``
 
    int t10 = add_v_v(v, w, z, m+5);
    CU_ASSERT_EQUAL(t10, 0);

    // Libération de la mémoire
    free_vector(v);
    free_vector(w);
    free_vector(z);
    free_vector(v2);
    free_vector(z2);
    free_vector(zero);
    free_vector(exitzero);
}

void test_add_m_m(void) {
    uint64_t m = 30;
    uint64_t n = 20;

    // Matrices valides
    matrix *A = init_matrix(m, n);
    matrix *B = init_matrix(m, n);
    matrix *C = init_matrix(m, n);

    // Matrices de mauvaise taille
    matrix *A2 = init_matrix(m + 10, n + 10);
    matrix *C2 = init_matrix(m + 10, n + 10);

    // Valeurs aléatoires pour A et B
    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            A->values[i][j] = (double)rand() / RAND_MAX;
            B->values[i][j] = (double)rand() / RAND_MAX;
        }
    }

    // Cas 1 : Somme de deux matrices valides
    int t1 = add_m_m(A, B, C, nb_threads);
    CU_ASSERT_EQUAL(t1, 0);
    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            CU_ASSERT_DOUBLE_EQUAL(C->values[i][j], A->values[i][j] + B->values[i][j], 1e-3);
        }
    }

    // Cas 2 : A et B ont des tailles différentes
    int t2 = add_m_m(A2, B, C, nb_threads);
    CU_ASSERT_EQUAL(t2, -1);

    // Cas 3 : C a une taille différente
    int t3 = add_m_m(A, B, C2, nb_threads);
    CU_ASSERT_EQUAL(t3, -1);

    // Cas 4 : B est NULL
    int t4 = add_m_m(A, NULL, C, nb_threads);
    CU_ASSERT_EQUAL(t4, 0);
    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            CU_ASSERT_DOUBLE_EQUAL(C->values[i][j], A->values[i][j], 1e-3);
        }
    }

    // Cas 5 : A est NULL
    int t5 = add_m_m(NULL, B, C, nb_threads);
    CU_ASSERT_EQUAL(t5, 0);
    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            CU_ASSERT_DOUBLE_EQUAL(C->values[i][j], B->values[i][j], 1e-3);
        }
    }

    // Cas 6 : A et B sont NULL
    int t6 = add_m_m(NULL, NULL, C, nb_threads);
    CU_ASSERT_EQUAL(t6, -1);

    // Cas 7 : C est NULL
    int t7 = add_m_m(A, B, NULL, nb_threads);
    CU_ASSERT_EQUAL(t7, -1);

    // Cas 8 : A est NULL, dimensions différentes
    int t8 = add_m_m(NULL, B, C2, nb_threads);
    CU_ASSERT_EQUAL(t8, -1);

    // Cas 9 : B est NULL, dimensions différentes
    int t9 = add_m_m(A2, NULL, C, nb_threads);
    CU_ASSERT_EQUAL(t9, -1);

    // Libération de la mémoire
    free_matrix(A);
    free_matrix(B);
    free_matrix(C);
    free_matrix(A2);
    free_matrix(C2);
}

void test_sub_v_v(void) {
    uint64_t m = 30;

    // Vecteurs valides
    vector *v = init_vector(m);
    vector *w = init_vector(m);
    vector *z = init_vector(m);

    // Vecteurs de taille différente
    vector *v2 = init_vector(m + 10);
    vector *z2 = init_vector(m + 10);


    // Initialisation aléatoire
    for (uint64_t i = 0; i < m; i++) {
        v->values[i] = (double)rand() / RAND_MAX;
        w->values[i] = (double)rand() / RAND_MAX;
    }

    // Cas 1 : Différence de deux vecteurs valides
    int t1 = sub_v_v(v, w, z, nb_threads);
    CU_ASSERT_EQUAL(t1, 0);
    for (uint64_t i = 0; i < m; i++) {
        CU_ASSERT_DOUBLE_EQUAL(z->values[i], v->values[i] - w->values[i], 1e-3);
    }

    // Cas 2 : x et y de tailles différentes
    int t2 = sub_v_v(v2, w, z, nb_threads);
    CU_ASSERT_EQUAL(t2, -1);

    // Cas 3 : z de taille différente
    int t3 = sub_v_v(v, w, z2, nb_threads);
    CU_ASSERT_EQUAL(t3, -1);

    // Cas 4 : y est NULL
    int t4 = sub_v_v(v, NULL, z, nb_threads);
    CU_ASSERT_EQUAL(t4, 0);
    for (uint64_t i = 0; i < m; i++) {
        CU_ASSERT_DOUBLE_EQUAL(z->values[i], v->values[i], 1e-3);
    }

    // Cas 5 : x est NULL
    int t5 = sub_v_v(NULL, v, z, nb_threads);
    CU_ASSERT_EQUAL(t5, 0);
    for (uint64_t i = 0; i < m; i++) {
        CU_ASSERT_DOUBLE_EQUAL(z->values[i], -v->values[i], 1e-3);
    }

    // Cas 6 : x et y sont NULL
    int t6 = sub_v_v(NULL, NULL, z, nb_threads);
    CU_ASSERT_EQUAL(t6, -1);

    // Cas 7 : z est NULL
    int t7 = sub_v_v(v, w, NULL, nb_threads);
    CU_ASSERT_EQUAL(t7, -1);

    // Cas 8 : x et y de taille 0 (vides)
    vector *zero = init_vector(0);
    int t8 = sub_v_v(zero, zero, z2, nb_threads);
    CU_ASSERT_EQUAL(t8, 0);
    CU_ASSERT_EQUAL(z2->m, 0);

    // Cas 9 : nb_threads <= 0 
    int t9 = sub_v_v(v, w, z, -5);
    CU_ASSERT_EQUAL(t9, -1);
    t9 = sub_v_v(v, w, z, 0);
    CU_ASSERT_EQUAL(t9, -1);

    // Cas 10 = nb_threads > m
 
    int t10 = sub_v_v(v, w, z, m+5);
    CU_ASSERT_EQUAL(t10, 0);

    // Libération de la mémoire
    free_vector(v);
    free_vector(w);
    free_vector(z);
    free_vector(v2);
    free_vector(z2);
    free_vector(zero);
}

void test_sub_m_m(void) {
    uint64_t m = 30;
    uint64_t n = 20;

    // Matrices valides
    matrix *A = init_matrix(m, n);
    matrix *B = init_matrix(m, n);
    matrix *C = init_matrix(m, n);

    // Matrices de mauvaise taille
    matrix *A2 = init_matrix(m + 10, n + 10);
    matrix *C2 = init_matrix(m + 10, n + 10);

    // Valeurs aléatoires pour A et B
    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            A->values[i][j] = (double)rand() / RAND_MAX;
            B->values[i][j] = (double)rand() / RAND_MAX;
        }
    }

    // Cas 1 : Différence de deux matrices valides
    int t1 = sub_m_m(A, B, C, nb_threads);
    CU_ASSERT_EQUAL(t1, 0);
    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            CU_ASSERT_DOUBLE_EQUAL(C->values[i][j], A->values[i][j] - B->values[i][j], 1e-3);
        }
    }

    // Cas 2 : A et B ont des tailles différentes
    int t2 = sub_m_m(A2, B, C, nb_threads);
    CU_ASSERT_EQUAL(t2, -1);

    // Cas 3 : C a une taille différente
    int t3 = sub_m_m(A, B, C2, nb_threads);
    CU_ASSERT_EQUAL(t3, -1);

    // Cas 4 : B est NULL
    int t4 = sub_m_m(A, NULL, C, nb_threads);
    CU_ASSERT_EQUAL(t4, 0);
    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            CU_ASSERT_DOUBLE_EQUAL(C->values[i][j], A->values[i][j], 1e-3);
        }
    }

    // Cas 5 : A est NULL
    int t5 = sub_m_m(NULL, A, C, nb_threads);
    CU_ASSERT_EQUAL(t5, 0);
    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            CU_ASSERT_DOUBLE_EQUAL(C->values[i][j], -A->values[i][j], 1e-3);
        }
    }

    // Cas 6 : A et B sont NULL
    int t6 = sub_m_m(NULL, NULL, C, nb_threads);
    CU_ASSERT_EQUAL(t6, -1);

    // Cas 7 : C est NULL
    int t7 = sub_m_m(A, B, NULL, nb_threads);
    CU_ASSERT_EQUAL(t7, -1);

    // Cas 8 : nb_threads <= 0
    int8_t invalid_threads = -5;
    int t8 = sub_m_m(A, B, C, invalid_threads);
    CU_ASSERT_EQUAL(t8, -1);

    // Cas 9 : nb_threads > A->m
    int8_t too_many_threads = m+5;
    int t9 = sub_m_m(A, B, C, too_many_threads);
    CU_ASSERT_EQUAL(t9, 0);

    // Libération de la mémoire
    free_matrix(A);
    free_matrix(B);
    free_matrix(C);
    free_matrix(A2);
    free_matrix(C2);
}

void test_dot_prod(void) {
    uint64_t m = 300;

    // Cas 1 : Produit scalaire de deux vecteurs valides
    vector *p = init_vector(m);
    vector *q = init_vector(m);
    double expected = 0.0;
    for (uint64_t i = 0; i < m; i++) {
        p->values[i] = (double)rand() / RAND_MAX;
        q->values[i] = (double)rand() / RAND_MAX;
        expected += p->values[i] * q->values[i];
    }
    double result1 = dot_prod(p, q, nb_threads);
    CU_ASSERT_DOUBLE_EQUAL(result1, expected, 1e-3);

    // Cas 2 : Vecteurs de tailles différentes
    vector *r = init_vector(5);
    vector *s = init_vector(3);
    double result2 = dot_prod(r, s, nb_threads);
    CU_ASSERT_TRUE(isnan(result2));

    // Cas 3 : Vecteurs de taille 0
    vector *t = init_vector(0);
    vector *u = init_vector(0);
    double result3 = dot_prod(t, u, nb_threads);
    CU_ASSERT_DOUBLE_EQUAL(result3, 0.0, 1e-3);

    // Cas 4 : Un des vecteurs est NULL
    vector *null_vec = NULL;
    vector *non_null_vec = init_vector(5);
    double result4a = dot_prod(null_vec, non_null_vec, nb_threads);
    double result4b = dot_prod(non_null_vec, null_vec, nb_threads);
    double result4c = dot_prod(NULL, NULL, nb_threads);
    CU_ASSERT_TRUE(isnan(result4a));
    CU_ASSERT_TRUE(isnan(result4b));
    CU_ASSERT_TRUE(isnan(result4c));
    free_vector(non_null_vec);

    // Cas 5 : Le nombre de threads est invalide
    int invalid_threads = -5;
    vector *v = init_vector(m);
    vector *w = init_vector(m);
    double result5 = dot_prod(v, w, invalid_threads);
    CU_ASSERT_TRUE(isnan(result5));
    result5 = dot_prod(v, w, 0);
    CU_ASSERT_TRUE(isnan(result5));

    // Cas 6 : Les arguments contiennent un NAN
    vector *x = init_vector(2);
    vector *y = init_vector(2);
    x->values[0] = 1.0;
    x->values[1] = NAN;
    y->values[0] = 2.0;
    y->values[1] = 3.0;
    double result6a = dot_prod(x, y, 1);
    double result6b = dot_prod(x, y, 4);
    CU_ASSERT_TRUE(isnan(result6a));
    CU_ASSERT_TRUE(isnan(result6b));

    // Libération de la mémoire
    free_vector(p);
    free_vector(q);
    free_vector(r);
    free_vector(s);
    free_vector(t);
    free_vector(u);
    free_vector(v);
    free_vector(w);
    free_vector(x);
    free_vector(y);
}

void test_norm(void) {

    // Cas 1 : Vecteur de taille 1
    vector *v1 = init_vector(1);
    v1->values[0] = (double)rand() / RAND_MAX; 
    CU_ASSERT_DOUBLE_EQUAL(norm(v1, nb_threads), fabs(v1->values[0]), 1e-3);  // norme = |x| si 1 seul élément

    // Cas 2 : Vecteur avec valeurs fixes
    vector *v2 = init_vector(4);
    v2->values[0] = -7.0;
    v2->values[1] = 0.0;
    v2->values[2] = 4.0;
    v2->values[3] = -4.0;
    CU_ASSERT_DOUBLE_EQUAL(norm(v2, nb_threads), 9.0, 1e-3); // sqrt(49 + 0 + 16 + 16)

    // Cas 3 : Vecteur avec valeurs aléatoires
    uint64_t n = 10;
    vector *v3 = init_vector(n);
    double expected_sum = 0;
    for (uint64_t i = 0; i < n; i++) {
        double r = (double)rand() / RAND_MAX;
        v3->values[i] = r;
        expected_sum += r * r;
    }
    CU_ASSERT_DOUBLE_EQUAL(norm(v3, nb_threads), sqrt(expected_sum), 1e-3);

    // Cas 4 : Vecteur de taille 0
    vector *v4 = init_vector(0);
    CU_ASSERT_DOUBLE_EQUAL(norm(v4, 1), 0.0, 1e-3);

    // Cas 5 : Vecteur NULL
    vector *v5 = NULL;
    CU_ASSERT_TRUE(isnan(norm(v5, nb_threads)));

    // Cas 6 : Les arguments contiennent un NAN
    vector *v6 = init_vector(2);
    v6->values[0] = NAN;
    v6->values[1] = 1.0;
    CU_ASSERT_TRUE(isnan(norm(v6, 1)))

    // Cas 8 : x et y de taille 0 (vides)
    vector *zero = init_vector(0);
    double t8 = norm(zero, nb_threads);
    CU_ASSERT_DOUBLE_EQUAL(t8, 0.0, 1e-3);

    // Cas 9 : nb_threads <= 0 
    double t9 = norm(v2, -5);
    CU_ASSERT_TRUE(isnan(t9));
    t9 = norm(v2, 0);
    CU_ASSERT_TRUE(isnan(t9));

    // Cas 10 = nb_threads > m
    int t10 = norm(v2, (v2->m +5));
    CU_ASSERT_DOUBLE_EQUAL(t10, 9.0, 1e-3);

    // Libération de la mémoire
    free_vector(v1);
    free_vector(v2);
    free_vector(v3);
    free_vector(v4);
    free_vector(v5);
    free_vector(v6);
    free_vector(zero);
}

void test_mult_m_v(void) {
    uint64_t m = 5;
    uint64_t n = 3; 

    // Cas 1 : Multiplication valide avec valeurs fixes
    // Matrice 2x2 et vecteur 2x1
    matrix *fixed_mat = init_matrix(2, 2);
    vector *fixed_vec = init_vector(2);
    vector *fixed_res = init_vector(2);

    fixed_mat->values[0][0] = 1.0; fixed_mat->values[0][1] = 2.0;
    fixed_mat->values[1][0] = 3.0; fixed_mat->values[1][1] = 4.0;
    fixed_vec->values[0] = 5.0;
    fixed_vec->values[1] = 6.0;

    int r0 = mult_m_v(fixed_mat, fixed_vec, fixed_res, nb_threads);
    CU_ASSERT_EQUAL(r0, 0);
    CU_ASSERT_DOUBLE_EQUAL(fixed_res->values[0], 17.0, 1e-3); // 1*5 + 2*6
    CU_ASSERT_DOUBLE_EQUAL(fixed_res->values[1], 39.0, 1e-3); // 3*5 + 4*6

    // Cas 2 : Multiplication valide avec valeurs aléatoires
    matrix *mat = init_matrix(m, n);
    vector *v = init_vector(n);
    vector *res = init_vector(m);
    double expected[5] = {0};

    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            mat->values[i][j] = (double)rand() / RAND_MAX;
        }
    }
    for (uint64_t j = 0; j < n; j++) {
        v->values[j] = (double)rand() / RAND_MAX;
    }
    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            expected[i] += mat->values[i][j] * v->values[j];
        }
    }
    int r1 = mult_m_v(mat, v, res, nb_threads);
    CU_ASSERT_EQUAL(r1, 0);
    for (uint64_t i = 0; i < m; i++) {
        CU_ASSERT_DOUBLE_EQUAL(res->values[i], expected[i], 1e-3);
    }

    // Cas 3 : Mauvaise dimension du vecteur d’entrée
    matrix *mat2 = init_matrix(m, n);
    vector *v_wrong = init_vector(n + 1);
    vector *res2 = init_vector(m);
    int r2 = mult_m_v(mat2, v_wrong, res2, nb_threads);
    CU_ASSERT_EQUAL(r2, -1);

    // Cas 4 : Mauvaise dimension du vecteur de sortie
    matrix *mat3 = init_matrix(m, n);
    vector *v3 = init_vector(n);
    vector *res_wrong = init_vector(m + 2);
    int r3 = mult_m_v(mat3, v3, res_wrong, 1);
    CU_ASSERT_EQUAL(r3, 0);

    // Cas 5 : Un des paramètres est NULL
    matrix *mat4 = init_matrix(m, n);
    vector *v4 = init_vector(n);
    vector *res4 = init_vector(m);
    CU_ASSERT_EQUAL(mult_m_v(NULL, v4, res4, nb_threads), -1);
    CU_ASSERT_EQUAL(mult_m_v(mat4, NULL, res4, nb_threads), -1);
    CU_ASSERT_EQUAL(mult_m_v(mat4, v4, NULL, nb_threads), -1);

    // Cas 6 : Multiplication valide avec valeurs fixes, 2threads
    vector *fixed_res2 = init_vector(2);
    int r5 = mult_m_v(fixed_mat, fixed_vec, fixed_res2, 2);
    CU_ASSERT_EQUAL(r5, 0);
    CU_ASSERT_DOUBLE_EQUAL(fixed_res2->values[0], 17.0, 1e-3); // 1*5 + 2*6
    CU_ASSERT_DOUBLE_EQUAL(fixed_res2->values[1], 39.0, 1e-3); // 3*5 + 4*6
    
    // Cas 7 : Multiplication valide avec valeurs fixes, 4threads
    vector *fixed_res4 = init_vector(2);
    int r6 = mult_m_v(fixed_mat, fixed_vec, fixed_res4, 4);
    CU_ASSERT_EQUAL(r6, 0);
    CU_ASSERT_DOUBLE_EQUAL(fixed_res4->values[0], 17.0, 1e-3); // 1*5 + 2*6
    CU_ASSERT_DOUBLE_EQUAL(fixed_res4->values[1], 39.0, 1e-3); // 3*5 + 4*6

    // Cas 8 : nb_threads <= 0
    int8_t invalid_threads = -5;
    int r7 = mult_m_v(mat, v, res, invalid_threads);
    CU_ASSERT_EQUAL(r7, -1);

    // Cas 9 : arguments vides 
    vector *zero = init_vector(0);
    int r8 = mult_m_v(mat, zero, res, nb_threads);
    CU_ASSERT_EQUAL(r8, -1);
    matrix *zeroM = init_matrix(0,0);
    r8 = mult_m_v(zeroM, v, res, nb_threads);
    CU_ASSERT_EQUAL(r8, -1);
    r8 = mult_m_v(zeroM, zero, res, nb_threads);
    CU_ASSERT_EQUAL(r8, -1);
    r8 = mult_m_v(mat, zero, zero, nb_threads);
    CU_ASSERT_EQUAL(r8, -1);

    // Libération de la mémoire
    free_matrix(fixed_mat);
    free_vector(fixed_vec);
    free_vector(fixed_res);
    free_vector(fixed_res2);
    free_vector(fixed_res4);
    free_matrix(mat);
    free_vector(v);
    free_vector(res);
    free_matrix(mat2);
    free_vector(v_wrong);
    free_vector(res2);
    free_matrix(mat3);
    free_vector(v3);
    free_vector(res_wrong);
    free_matrix(mat4);
    free_vector(v4);
    free_vector(res4);
    free_vector(zero);
    free_matrix(zeroM);
}

void test_mult_m_m(void) {
    uint64_t m = 5;
    uint64_t n = 4;
    uint64_t p = 3;

    // Cas 1 : Multiplication valide avec valeurs fixes, 1 thread
    matrix *A = init_matrix(3, 4);
    matrix *B = init_matrix(4, 2);
    matrix *C = init_matrix(3, 2);

    A->values[0][0] = 1.0; A->values[0][1] = 2.0; A->values[0][2] = 3.0; A->values[0][3] = 4.0;
    A->values[1][0] = 5.0; A->values[1][1] = 6.0; A->values[1][2] = 7.0; A->values[1][3] = 8.0;
    A->values[2][0] = 9.0; A->values[2][1] =10.0; A->values[2][2] =11.0; A->values[2][3] =12.0;

    B->values[0][0] = 1.0; B->values[0][1] = 2.0;
    B->values[1][0] = 3.0; B->values[1][1] = 4.0;
    B->values[2][0] = 5.0; B->values[2][1] = 6.0;
    B->values[3][0] = 7.0; B->values[3][1] = 8.0;

    int t1 = mult_m_m(A, B, C, nb_threads);
    CU_ASSERT_EQUAL(t1, 0);
    CU_ASSERT_DOUBLE_EQUAL(C->values[0][0],  50.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(C->values[0][1],  60.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(C->values[1][0], 114.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(C->values[1][1], 140.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(C->values[2][0], 178.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(C->values[2][1], 220.0, 1e-3);

    // Cas 2 : Dimensions incompatibles
    matrix *B_wrong = init_matrix(3, 2);
    int t2 = mult_m_m(A, B_wrong, C, nb_threads);
    CU_ASSERT_EQUAL(t2, -1);

    // Cas 3 : C a mauvaise taille initialement (3x3)
    matrix *C_wrong = init_matrix(3, 3);
    int t3 = mult_m_m(A, B, C_wrong, 1);  // plus de redimensionnement automatique
    CU_ASSERT_EQUAL(t3, -1); 

    // Cas 4 : A est NULL
    int t4 = mult_m_m(NULL, B, C, nb_threads);
    CU_ASSERT_EQUAL(t4, -1);

    // Cas 5 : B est NULL
    int t5 = mult_m_m(A, NULL, C, nb_threads);
    CU_ASSERT_EQUAL(t5, -1);

    // Cas 6 : C est NULL
    int t6 = mult_m_m(A, B, NULL, nb_threads);
    CU_ASSERT_EQUAL(t6, -1);

    // Cas 7 : Multiplication avec matrices aléatoires
    matrix *A_rand = init_matrix(m, n);
    matrix *B_rand = init_matrix(n, p);
    matrix *C_rand = init_matrix(m, p);

    for (uint64_t i = 0; i < m; i++)
        for (uint64_t j = 0; j < n; j++)
            A_rand->values[i][j] = (double)rand() / RAND_MAX;

    for (uint64_t i = 0; i < n; i++)
        for (uint64_t j = 0; j < p; j++)
            B_rand->values[i][j] = (double)rand() / RAND_MAX;

    double **expected = (double **)malloc(m * sizeof(double *));
    for (uint64_t i = 0; i < m; i++) {
        expected[i] = (double *)calloc(p, sizeof(double));
        for (uint64_t j = 0; j < p; j++) {
            for (uint64_t k = 0; k < n; k++) {
                expected[i][j] += A_rand->values[i][k] * B_rand->values[k][j];
            }
        }
    }

    int t7 = mult_m_m(A_rand, B_rand, C_rand, nb_threads);
    CU_ASSERT_EQUAL(t7, 0);
    for (uint64_t i = 0; i < m; i++)
        for (uint64_t j = 0; j < p; j++)
            CU_ASSERT_DOUBLE_EQUAL(C_rand->values[i][j], expected[i][j], 1e-3);

    // Cas 8 : Matrice vide
    matrix *A_vide = init_matrix(0, 5);  // m = 0, donc pas de lignes
    matrix *B_vide = init_matrix(5, 3);
    matrix *C_vide = init_matrix(0, 3);  // devrait correspondre à A×B

    int result = mult_m_m(A_vide, B_vide, C_vide, 1);
    CU_ASSERT_EQUAL(result, 0); // devrait fonctionner (0 itérations)

    free_matrix(A_vide);
    free_matrix(B_vide);
    free_matrix(C_vide);

    // Cas 9 : Multiplication valide avec valeurs fixes, 2 threads
    matrix *C2 = init_matrix(3, 2);
    int t8 = mult_m_m(A, B, C2, 2);
    CU_ASSERT_EQUAL(t8, 0);
    CU_ASSERT_DOUBLE_EQUAL(C2->values[0][0],  50.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(C2->values[0][1],  60.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(C2->values[1][0], 114.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(C2->values[1][1], 140.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(C2->values[2][0], 178.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(C2->values[2][1], 220.0, 1e-3);

    // Cas 10 : Multiplication valide avec valeurs fixes, 4 threads
    matrix *C4 = init_matrix(3, 2);
    int t9 = mult_m_m(A, B, C4, 4);
    CU_ASSERT_EQUAL(t9, 0);
    CU_ASSERT_DOUBLE_EQUAL(C4->values[0][0],  50.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(C4->values[0][1],  60.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(C4->values[1][0], 114.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(C4->values[1][1], 140.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(C4->values[2][0], 178.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(C4->values[2][1], 220.0, 1e-3);

    // Libération de la mémoire
    free_matrix(A);
    free_matrix(B);
    free_matrix(C);
    free_matrix(B_wrong);
    free_matrix(C_wrong);
    free_matrix(A_rand);
    free_matrix(B_rand);
    free_matrix(C_rand);
    for (uint64_t i = 0; i < m; i++) {
        free(expected[i]);
    }
    free(expected);
    free_matrix(C2);
    free_matrix(C4);
}

void test_transp(void) {
    uint64_t m = 10;
    uint64_t n = 20;

    // Cas 1 : Transposition standard d'une matrice 10x20
    matrix *A = init_matrix(m, n);
    matrix *C = init_matrix(n, m);

    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            A->values[i][j] = (double)rand() / RAND_MAX;
        }
    }

    CU_ASSERT_EQUAL(transp(A, C, nb_threads), 0);
    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            CU_ASSERT_DOUBLE_EQUAL(C->values[j][i], A->values[i][j], 1e-3);
        }
    }

    // Cas 2 : Matrice de sortie incompatible (5x6 -> 10x10)
    matrix *B = init_matrix(5, 6);
    matrix *D = init_matrix(10, 10);
    CU_ASSERT_EQUAL(transp(B, D, nb_threads), -1);

    // Cas 3 : Matrice A est NULL
    matrix *E = NULL;
    matrix *F = init_matrix(3, 3);
    CU_ASSERT_EQUAL(transp(E, F, nb_threads), -1);

    // Cas 4 : Matrice C est NULL
    matrix *G = init_matrix(3, 3);
    matrix *H = NULL;
    CU_ASSERT_EQUAL(transp(G, H, nb_threads), -1);

    // Cas 7 : Dimensions compatibles mais incorrectes

    matrix *K = init_matrix(10, 20);
    matrix *L = init_matrix(10, 20); // mauvais : devrait être 20x10

    int result = transp(K, L, nb_threads);
    CU_ASSERT_EQUAL(result, -1);

    // // Cas 8 : Vecteur
    // vector *v = init_vector(5);
    // matrix *V = init_matrix(5,1);
    // vector *Mv =init_vector(5);
    // matrix *MV = init_matrix(1, 5);
    // for (uint64_t i = 0; i < 5; i++) {
    //     double r = (double)rand() / RAND_MAX;
    //     v->values[i] = r;
    //     V->values[i][0] = r;
    // }
    // CU_ASSERT_EQUAL(transp(v, Mv, nb_threads), 0);
    // CU_ASSERT_EQUAL(transp(V, MV, nb_threads), 0);
    // for (uint64_t i = 0; i < 5; i++) {
    //     CU_ASSERT_DOUBLE_EQUAL(Mv->values[i], v->values[i], 1e-3);
    //     CU_ASSERT_DOUBLE_EQUAL(MV->values[0][i], V->values[i][0], 1e-3);
    // }

    // Cas 9 : A = structure invalide     
    matrix *chose = init_matrix(1, 1);
    chose->type = 42; // ni MATRIX ni VECTOR
    CU_ASSERT_EQUAL(transp(chose, C, 1), -1);

    // Libération de la mémoire
    free_matrix(A);
    free_matrix(C);
    free_matrix(B);
    free_matrix(D);
    free_matrix(F);
    free_matrix(G);
    free_matrix(K);
    free_matrix(L);
    // free_vector(v);
    // free_vector(Mv);
    // free_matrix(V);
    // free_matrix(MV);
    free_matrix(chose);
}

void test_back_sub(void) {

    // Cas 0 : Exemple simple et vérifiable à la main
    // U = [ [1, 2],
    //       [0, 3] ]
    // v = [5, 3]  → solution attendue : w = [3, 1]
    matrix *U0 = init_matrix(2, 2);
    vector *v0 = init_vector(2);
    vector *w0 = init_vector(2);

    U0->values[0][0] = 1; U0->values[0][1] = 2;
    U0->values[1][0] = 0; U0->values[1][1] = 3;
    v0->values[0] = 5;
    v0->values[1] = 3;

    CU_ASSERT_EQUAL(back_sub(U0, v0, w0), 0);
    CU_ASSERT_DOUBLE_EQUAL(w0->values[0], 3.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(w0->values[1], 1.0, 1e-3);

    // Cas 1 : Substitution arrière normale avec 3x3
    matrix *U = init_matrix(3, 3);
    vector *v = init_vector(3);
    vector *w = init_vector(3);

    U->values[0][0] = 2;  U->values[0][1] = -1; U->values[0][2] = 3;
    U->values[1][0] = 0;  U->values[1][1] = 1;  U->values[1][2] = 4;
    U->values[2][0] = 0;  U->values[2][1] = 0;  U->values[2][2] = 2;
    v->values[0] = 5;
    v->values[1] = 6;
    v->values[2] = 4;

    int r1 = back_sub(U, v, w);
    CU_ASSERT_EQUAL(r1, 0);
    CU_ASSERT_DOUBLE_EQUAL(w->values[2], 2.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(w->values[1], -2.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(w->values[0], -1.5, 1e-3);

    // Cas 2 : Matrice non triangulaire supérieure
    matrix *M = init_matrix(2, 2);
    M->values[0][0] = 1; M->values[0][1] = 0;
    M->values[1][0] = 2; M->values[1][1] = 3;
    CU_ASSERT_EQUAL(back_sub(M, v, w), -1);

    // Cas 3 : Taille incompatible
    vector *v_wrong = init_vector(2);
    CU_ASSERT_EQUAL(back_sub(U, v_wrong, w), -1);

    // Cas 4 : Pivot nul, v[i] ≠ 0 → aucune solution
    matrix *Z = init_matrix(2, 2);
    Z->values[0][0] = 1; Z->values[0][1] = 1;
    Z->values[1][0] = 0; Z->values[1][1] = 0; // pivot nul
    vector *v_bad = init_vector(2);
    v_bad->values[0] = 5;
    v_bad->values[1] = 3;
    vector *w_bad = init_vector(2);
    CU_ASSERT_EQUAL(back_sub(Z, v_bad, w_bad), -1);

    // Cas 5 : Pivot nul, v[i] == 0 → infinité de solutions
    v_bad->values[1] = 0;
    CU_ASSERT_EQUAL(back_sub(Z, v_bad, w_bad), 0);
    CU_ASSERT_DOUBLE_EQUAL(w_bad->values[1], 0.0, 1e-3);

    // Cas 6 : Un des arguments est NULL
    CU_ASSERT_EQUAL(back_sub(NULL, v, w), -1);
    CU_ASSERT_EQUAL(back_sub(U, NULL, w), -1);
    CU_ASSERT_EQUAL(back_sub(U, v, NULL), -1);

    // Libération de la mémoire
    free_matrix(U0);
    free_vector(v0);
    free_vector(w0);
    free_matrix(U);
    free_vector(v);
    free_vector(w);
    free_matrix(M);
    free_vector(v_wrong);
    free_matrix(Z);
    free_vector(v_bad);
    free_vector(w_bad);
}

void test_qr(void) {
    uint64_t m = 3;
    uint64_t n = 3;

    // Cas 1 : Décomposition QR valide (A = QR, Qᵗ·Q ≈ I, R triangulaire)
    matrix *A = init_matrix(m, n);
    matrix *Q = init_matrix(m, n);
    matrix *R = init_matrix(n, n);

    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            // A->values[i][j] = (double)rand() / RAND_MAX;
            A->values[i][j] = 2.0 * ((double)rand() / RAND_MAX) - 1.0;
        }
    }

    CU_ASSERT_EQUAL(qr(A, Q, R, nb_threads), 0);

    // Vérifie que Qᵗ·Q ≈ I
    matrix *Q_transp = init_matrix(n, m);
    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            Q_transp->values[j][i] = Q->values[i][j];
        }
    }

    matrix *QtQ = init_matrix(n, n);
    mult_m_m(Q_transp, Q, QtQ, 1);

    for (uint64_t i = 0; i < n; i++) {
        for (uint64_t j = 0; j < n; j++) {
            if (i == j) {
                CU_ASSERT_DOUBLE_EQUAL(QtQ->values[i][j], 1.0, 1e-3);
            } else {
                CU_ASSERT_DOUBLE_EQUAL(QtQ->values[i][j], 0.0, 1e-3);
            }
        }
    }

    // Vérifie que R est triangulaire supérieure
    for (uint64_t i = 1; i < n; i++) {
        for (uint64_t j = 0; j < i; j++) {
            CU_ASSERT_DOUBLE_EQUAL(R->values[i][j], 0.0, 1e-3);
        }
    }

    // Vérifie que A ≈ Q·R
    matrix *A_check = init_matrix(m, n);
    mult_m_m(Q, R, A_check, 1);
    for (uint64_t i = 0; i < m; i++) {
        for (uint64_t j = 0; j < n; j++) {
            CU_ASSERT_DOUBLE_EQUAL(A_check->values[i][j], A->values[i][j], 1e-3);
        }
    }

    // Cas 2 : A est NULL
    matrix *q1 = init_matrix(2, 2);
    matrix *r1 = init_matrix(2, 2);
    CU_ASSERT_EQUAL(qr(NULL, q1, r1, nb_threads), -1);

    // Cas 3 : Q est NULL
    matrix *a2 = init_matrix(2, 2);
    matrix *r2 = init_matrix(2, 2);
    CU_ASSERT_EQUAL(qr(a2, NULL, r2, nb_threads), -1);

    // Cas 4 : R est NULL
    matrix *a3 = init_matrix(2, 2);
    matrix *q3 = init_matrix(2, 2);
    CU_ASSERT_EQUAL(qr(a3, q3, NULL, nb_threads), -1);

    // Cas 5 : A avec m < n
    matrix *a4 = init_matrix(2, 3);
    matrix *q4 = init_matrix(2, 3);
    matrix *r4 = init_matrix(3, 3);
    CU_ASSERT_EQUAL(qr(a4, q4, r4, nb_threads), -1);

    // Cas 6 : Q ou R ont les mauvaises dimensions
    matrix *a5 = init_matrix(3, 2);
    matrix *q5 = init_matrix(2, 2);
    matrix *r5 = init_matrix(2, 2);
    CU_ASSERT_EQUAL(qr(a5, q5, r5, nb_threads), -1);

    // Cas 7 : Colonne nulle
    matrix *A_0 = init_matrix(3, 3);
    matrix *Q_0 = init_matrix(3, 3);
    matrix *R_0 = init_matrix(3, 3);

    // colonne 0 = normale
    A_0->values[0][0] = 1.0; A_0->values[1][0] = 2.0; A_0->values[2][0] = 3.0;
    // colonne 1 = nulle
    A_0->values[0][1] = 0.0; A_0->values[1][1] = 0.0; A_0->values[2][1] = 0.0;
    // colonne 2 = normale
    A_0->values[0][2] = 4.0; A_0->values[1][2] = 5.0; A_0->values[2][2] = 6.0;
    
    int result = qr(A_0, Q_0, R_0, nb_threads);
    CU_ASSERT(result == 0 || result == -1);


    // Libération de la mémoire
    free_matrix(A);
    free_matrix(Q);
    free_matrix(R);
    free_matrix(Q_transp);
    free_matrix(QtQ);
    free_matrix(A_check);

    free_matrix(q1);
    free_matrix(r1);
    free_matrix(a2);
    free_matrix(r2);
    free_matrix(a3);
    free_matrix(q3);
    free_matrix(a4);
    free_matrix(q4);
    free_matrix(r4);
    free_matrix(a5);
    free_matrix(q5);
    free_matrix(r5);

    free_matrix(A_0);
    free_matrix(Q_0);
    free_matrix(R_0);
}

void test_is_upper_triang(void) {

    // Cas 1 : Matrice triangulaire supérieure
    matrix *A = init_matrix(3, 3);
    A->values[0][0] = 1; A->values[0][1] = 2; A->values[0][2] = 3;
    A->values[1][0] = 0; A->values[1][1] = 4; A->values[1][2] = 5;
    A->values[2][0] = 0; A->values[2][1] = 0; A->values[2][2] = 6;
    CU_ASSERT_EQUAL(is_upper_triang(A), 0);

    // Cas 2 : Un élément non nul sous la diagonale
    matrix *B = init_matrix(3, 3);
    B->values[1][0] = 1; // pas 0 → pas triangulaire
    CU_ASSERT_EQUAL(is_upper_triang(B), -1);

    // Cas 3 : Matrice non carrée
    matrix *C = init_matrix(2, 3);
    CU_ASSERT_EQUAL(is_upper_triang(C), -1);

    // Cas 4 : Matrice NULL
    matrix *D = NULL;
    CU_ASSERT_EQUAL(is_upper_triang(D), -1);

    // Cas 5 : Matrice diagonale
    matrix *E = init_matrix(3, 3);
    E->values[0][0] = 1;
    E->values[1][1] = 2;
    E->values[2][2] = 3;
    CU_ASSERT_EQUAL(is_upper_triang(E), 0);

    // Cas 6 : Matrice entièrement nulle
    matrix *F = init_matrix(3, 3);
    CU_ASSERT_EQUAL(is_upper_triang(F), 0);

    // Libération de la mémoire
    free_matrix(A);
    free_matrix(B);
    free_matrix(C);
    free_matrix(E);
    free_matrix(F);
}

void test_lstsq(void) {
    // Cas 1 : Régression polynomiale parfaite (y = 1 + 2x + 3x²)
    uint64_t n = 5;
    uint64_t degree = 2;

    vector *x_data = init_vector(n);
    vector *y_data = init_vector(n);

    x_data->values[0] = 0.0; y_data->values[0] = 1.0;
    x_data->values[1] = 1.0; y_data->values[1] = 6.0;
    x_data->values[2] = 2.0; y_data->values[2] = 17.0;
    x_data->values[3] = 3.0; y_data->values[3] = 34.0;
    x_data->values[4] = 4.0; y_data->values[4] = 57.0;

    matrix *A = init_matrix(n, degree + 1);
    for (uint64_t i = 0; i < n; i++) {
        A->values[i][0] = 1.0;
        A->values[i][1] = x_data->values[i];
        A->values[i][2] = x_data->values[i] * x_data->values[i];
    }

    vector *b = y_data;
    vector *coeff = init_vector(degree + 1);
    int verif = lstsq(A, b, coeff, nb_threads);
    CU_ASSERT_EQUAL(verif, 0);

    CU_ASSERT_DOUBLE_EQUAL(coeff->values[0], 1.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(coeff->values[1], 2.0, 1e-3);
    CU_ASSERT_DOUBLE_EQUAL(coeff->values[2], 3.0, 1e-3);

    // Cas 2 : A == NULL
    vector *b2 = init_vector(3);
    vector *x2 = init_vector(2);
    CU_ASSERT_EQUAL(lstsq(NULL, b2, x2, nb_threads), -1);

    // Cas 3 : b == NULL
    matrix *A3 = init_matrix(3, 2);
    vector *x3 = init_vector(2);
    CU_ASSERT_EQUAL(lstsq(A3, NULL, x3, nb_threads), -1);

    // Cas 4 : x == NULL
    matrix *A4 = init_matrix(3, 2);
    vector *b4 = init_vector(3);
    CU_ASSERT_EQUAL(lstsq(A4, b4, NULL, nb_threads), -1);

    // Cas 5 : A non surdéterminée (m < n)
    matrix *A5 = init_matrix(2, 3); // m < n
    vector *b5 = init_vector(2);
    vector *x5 = init_vector(3);
    CU_ASSERT_EQUAL(lstsq(A5, b5, x5, nb_threads), -1);

    // Cas 6 : A->m != b->m
    matrix *A6 = init_matrix(3, 2);
    vector *b6 = init_vector(4); // incompatible
    vector *x6 = init_vector(2);
    CU_ASSERT_EQUAL(lstsq(A6, b6, x6, nb_threads), -1);

    // Cas 7 : A->n != x->m
    matrix *A7 = init_matrix(3, 2);
    vector *b7 = init_vector(3);
    vector *x7 = init_vector(3); // incompatible
    CU_ASSERT_EQUAL(lstsq(A7, b7, x7, nb_threads), -1);

    // Cas 8 : Matrice ou vecteur vide
    matrix *A8 = init_matrix(0, 0);
    vector *b8 = init_vector(0);
    vector *x8 = init_vector(0);
    CU_ASSERT_EQUAL(lstsq(A8, b8, x8, nb_threads), -1);

    // Libération mémoire
    free_vector(x_data);
    free_vector(y_data);
    free_matrix(A);
    free_vector(coeff);

    free_vector(b2);
    free_vector(x2);
    free_matrix(A3);
    free_vector(x3);
    free_matrix(A4);
    free_vector(b4);
    free_matrix(A5);
    free_vector(b5);
    free_vector(x5);
    free_matrix(A6);
    free_vector(b6);
    free_vector(x6);
    free_matrix(A7);
    free_vector(b7);
    free_vector(x7);
    free_matrix(A8);
    free_vector(b8);
    free_vector(x8);
}

void test_print_matrix_should_not_segfault_on_empty(void) {
    matrix *M = init_matrix(0, 0);
    CU_ASSERT_PTR_NOT_NULL(M);

    print_matrix(M);
    CU_PASS("print_matrix empty OK");
    free_matrix(M);

    // Cas 1 : taille 1xn
    matrix *A = init_matrix(1, 3);
    A->values[0][0] = 1.0;
    A->values[0][1] = 2.0;
    A->values[0][2] = 3.0;
    print_matrix(A);
    CU_PASS("print_matrix 1xn OK");
    free_matrix(A);

    // Cas 2 : taille 2x2
    A = init_matrix(2, 2);
    A->values[0][0] = 1.0; A->values[0][1] = 2.0;
    A->values[1][0] = 3.0; A->values[1][1] = 4.0;
    print_matrix(A);
    CU_PASS("print_matrix 2x2 OK");
    free_matrix(A);


}

void test_print_vector_should_not_segfault_on_empty(void) {
    vector *v = init_vector(0);
    CU_ASSERT_PTR_NOT_NULL(v);

    print_vector(v);
    CU_PASS("print_vector empty OK");
    free_vector(v);

    // Cas 1 : taille 1
    v = init_vector(1);
    v->values[0] = 1.0;
    print_vector(v);
    CU_PASS("print_vector 1 ok");
    free_vector(v);

    // Cas 2 : taille 3
    v = init_vector(3);
    v->values[0] = 1.0;
    v->values[1] = 2.0;
    v->values[2] = 3.0;
    print_vector(v);
    CU_PASS("print_vector 3 OK");
    free_vector(v);
}

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    FONCTIONS DANS FILE.C    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

void test_file_conversions(void) { /* tests des fonctions de conversion de doubles big_endian <-> host format */
    const char *filename = "tmp_u64.bin";
    uint64_t original = 0x0123456789ABCDEF;
    uint64_t readback = 0;

    // Cas 1 : Écriture + lecture standard
    FILE *f = fopen(filename, "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    CU_ASSERT_TRUE(write_u64_be(original, f));
    fclose(f);

    f = fopen(filename, "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    CU_ASSERT_TRUE(read_u64_be(f, &readback));
    fclose(f);
    CU_ASSERT_EQUAL(original, readback);

    // Cas 2 : FILE* NULL à l’écriture
    CU_ASSERT_FALSE(write_u64_be(original, NULL));

    // Cas 3 : FILE* NULL à la lecture
    CU_ASSERT_FALSE(read_u64_be(NULL, &readback));

    // Cas 4 : pointeur NULL pour le uint64_t en lecture
    FILE *f2 = fopen(filename, "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f2);
    CU_ASSERT_FALSE(read_u64_be(f2, NULL));
    fclose(f2);

    // Cas 5 : fichier vide
    FILE *f3 = fopen("tmp_empty.bin", "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f3);
    fclose(f3);
    f3 = fopen("tmp_empty.bin", "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f3);
    CU_ASSERT_FALSE(read_u64_be(f3, &readback));
    fclose(f3);

    // Cas 6 : valeur sous mauvais format
    FILE *f4 = fopen(filename, "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f4);
    uint64_t wrong = 0x12345678ABCDEF00; 
    size_t written = fwrite(&wrong, 1, sizeof(uint64_t) - 1, f4); // Écrit moins que sizeof(uint64_t)
    CU_ASSERT_EQUAL(written, sizeof(uint64_t) - 1);
    fclose(f4);

    f4 = fopen(filename, "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f4);
    CU_ASSERT_FALSE(read_u64_be(f4, &readback)); // Devrait échouer car la taille est incorrecte
    fclose(f4);

    remove(filename);
    remove("tmp_empty.bin");
}

void test_file_double_rw(void) {
    const char *filename = "tmp_scalar.bin";
    double original = 42.123456789;
    double readback;

    // Écriture
    FILE *f = fopen(filename, "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    write_double(original, f);
    fflush(f); 
    fclose(f);

    // Lecture
    f = fopen(filename, "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    readback = read_double(f);
    fclose(f);

    CU_ASSERT_DOUBLE_EQUAL(original, readback, 1e-9);
    remove(filename);

    // Cas 1 : read - fichier NULL
    CU_ASSERT_TRUE(isnan(read_double(NULL)));

    // Cas 2 : read - fichier vide
    FILE *empty = fopen("empty_double.bin", "wb");
    fclose(empty);
    empty = fopen("empty_double.bin", "rb");
    double r = read_double(empty);
    CU_ASSERT_TRUE(isnan(r));
    fclose(empty);
    remove("empty_double.bin");

    // Cas 3 : read_u64_be 
    FILE *f64 = fopen("short_u64.bin", "wb");
    uint8_t one_byte = 0xFF;
    fwrite(&one_byte, 1, 1, f64); // moins que sizeof(uint64_t)
    fclose(f64);

    f64 = fopen("short_u64.bin", "rb");
    uint64_t out = 0;
    bool ok = read_u64_be(f64, &out);
    CU_ASSERT_FALSE(ok);
    fclose(f64);
    remove("short_u64.bin");

    // Cas 4 : write - fichier NULL 
    write_double(original, NULL); 
    CU_PASS("write_double (NULL) ne segfault pas");

    // Cas 5 : write - valeur NaN
    FILE *NANf = fopen("nan.bin", "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(NANf);
    write_double(NAN, NANf);
    fclose(NANf);
    remove("nan.bin");
    CU_PASS("write_double (NaN) ne segfault pas");

    // Cas 6 : valeur sous mauvais format
    FILE *f4 = fopen(filename, "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f4);
    uint64_t wrong = 0x12345678ABCDEF00; 
    size_t written = fwrite(&wrong, 1, sizeof(uint64_t) - 1, f4); // Écrit moins que sizeof(uint64_t)
    CU_ASSERT_EQUAL(written, sizeof(uint64_t) - 1);
    fclose(f4);

    f4 = fopen(filename, "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f4);
    readback = read_double(f4); // Devrait échouer car la taille est incorrecte
    CU_ASSERT_TRUE(isnan(readback));
    fclose(f4);
}

void test_file_vector_rw(void) {
    vector *v = init_vector(3);
    v->values[0] = 1.23;
    v->values[1] = -4.56;
    v->values[2] = 7.89;

    FILE *f = fopen("tmp_vector.bin", "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    write_vector(v, f);
    fclose(f);

    f = fopen("tmp_vector.bin", "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    vector *r = read_vector(f);
    fclose(f);

    CU_ASSERT_PTR_NOT_NULL_FATAL(r);
    CU_ASSERT_EQUAL(r->m, 3);
    for (uint64_t i = 0; i < 3; i++) {
        CU_ASSERT_DOUBLE_EQUAL(v->values[i], r->values[i], 1e-6);
    }

    // Cas 1 : vecteur NULL
    f = fopen("tmp_vector_null.bin", "wb");
    if (f != NULL) {
        write_vector(NULL, f); // doit ne rien faire
        CU_PASS("write_vector(NULL, f) ne segfault pas");
        CU_ASSERT_PTR_NULL(read_vector(NULL));
        CU_PASS("read_vector(NULL, f) ne segfault pas");
        fclose(f);
        remove("tmp_vector_null.bin");
    } else {
        CU_FAIL("Échec d'ouverture de tmp_vector_null.bin pour le test NULL vector");
    }

    // Cas 2 : fichier NULL
    write_vector(v, NULL); // ne doit pas segfault
    CU_PASS("write_vector(v, NULL) ne segfault pas");

    // Cas 3 : lecture avec fichier NULL
    vector *bad = read_vector(NULL);
    CU_ASSERT_PTR_NULL(bad);
    CU_PASS("read_vector(NULL) renvoie NULL comme prévu");

    free_vector(v);
    free_vector(r);
    remove("tmp_vector.bin");

    // Cas 4 : écriture vecteur vide
    vector *empty = init_vector(0);
    f = fopen("tmp_empty_vector.bin", "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    write_vector(empty, f); // ne doit rien écrire mais ne plante pas
    fclose(f);
    remove("tmp_empty_vector.bin");
    free_vector(empty);

    // Cas 5 : écriture/lecture vecteur avec NaN
    vector *nan_vector = init_vector(3);
    nan_vector->values[0] = 1.0;
    nan_vector->values[1] = NAN;
    nan_vector->values[2] = 1.0;
    f = fopen("tmp_nan.bin", "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    write_vector(nan_vector, f);
    fclose(f);
    CU_PASS("write_vector (NaN) na segfault pas");

    f = fopen("tmp_nan.bin", "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    vector *nan_read = read_vector(f);
    fclose(f);
    CU_ASSERT_TRUE(isnan(nan_read->values[1]));
    CU_ASSERT_DOUBLE_EQUAL(nan_read->values[0], 1.0, 1e-6);
    free_vector(nan_vector);
    free_vector(nan_read);
    remove("tmp_nan.bin");

    // Cas 6 : taille inalide 
    f = fopen("test_invalid_size.bin", "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    uint64_t invalid_size = 0;
    fwrite(&invalid_size, sizeof(uint64_t) - 1, 1, f); // remplit pas toutle uint64_t
    fclose(f);

    f = fopen("test_invalid_size.bin", "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    v = read_vector(f);
    CU_ASSERT_PTR_NULL(v);
    fclose(f);
    remove("test_invalid_size.bin");

    // Cas 7 : fichier avec taille invalide
    f = fopen("test_corrupt_vector1.bin", "wb");
    uint64_t size = htobe64(10); // taille sans vals
    fwrite(&size, sizeof(uint64_t), 1, f);
    fclose(f);

    f = fopen("test_corrupt_vector1.bin", "rb");
    CU_ASSERT_PTR_NULL(read_vector(f));
    fclose(f);
    remove("test_corrupt_vector1.bin");

    // Cas 7 : read_vector échoue car lecture tronquée
    f = fopen("tmp_read_vector_fail.bin", "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    size = htobe64(3); // Taille annoncée = 3
    fwrite(&size, sizeof(uint64_t), 1, f);
    double vals[2] = {1.1, 2.2}; // N'écrit que 2 valeurs sur 3
    fwrite(vals, sizeof(double), 2, f);
    fclose(f);

    f = fopen("tmp_read_vector_fail.bin", "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    vector *v_fail = read_vector(f);
    CU_ASSERT_PTR_NULL(v_fail);
    fclose(f);
    remove("tmp_read_vector_fail.bin");
}


void test_file_matrix_rw(void) {
    matrix *A = init_matrix(2, 2);
    A->values[0][0] = 1.0; A->values[0][1] = 2.0;
    A->values[1][0] = 3.0; A->values[1][1] = 4.0;

    FILE *f = fopen("tmp_matrix.bin", "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    write_matrix(A, f);
    fclose(f);

    f = fopen("tmp_matrix.bin", "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    matrix *B = read_matrix(f);
    fclose(f);

    //CU_ASSERT_PTR_NOT_NULL_FATAL(B);
    CU_ASSERT_EQUAL(B->m, 2);
    CU_ASSERT_EQUAL(B->n, 2);
    for (uint64_t i = 0; i < 2; i++)
        for (uint64_t j = 0; j < 2; j++)
            CU_ASSERT_DOUBLE_EQUAL(A->values[i][j], B->values[i][j], 1e-6);

    // Cas 1 : matrice NULL
    f = fopen("tmp_matrix_null.bin", "wb");
    if (f != NULL) {
        write_matrix(NULL, f);
        fclose(f);
        CU_PASS("write_matrix(NULL, f) ne segfault pas");
        remove("tmp_matrix_null.bin");
    } else {
        CU_FAIL("Échec d'ouverture de tmp_matrix_null.bin pour test NULL matrix");
    }

    // Cas 2 : fichier NULL
    write_matrix(A, NULL);
    CU_PASS("write_matrix(A, NULL) ne segfault pas");

    // Cas 3 : lecture avec fichier NULL
    matrix *bad = read_matrix(NULL);
    CU_ASSERT_PTR_NULL(bad);
    CU_PASS("read_matrix(NULL) renvoie NULL comme prévu");

    free_matrix(A);
    free_matrix(B);
    remove("tmp_matrix.bin");

    // Cas 4 : écriture matrice vide
    matrix *empty = init_matrix(0, 0);
    f = fopen("tmp_empty_matrix.bin", "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    write_matrix(empty, f); // ne doit rien écrire mais ne plante pas
    fclose(f);
    remove("tmp_empty_matrix.bin");
    free_matrix(empty);

    // Cas 5 : écriture/lecture matrice avec NaN
    matrix *nan_matrix = init_matrix(2, 2);
    nan_matrix->values[0][0] = 1.0; nan_matrix->values[0][1] = NAN;
    nan_matrix->values[1][0] = 3.0; nan_matrix->values[1][1] = 4.0;
    f = fopen("tmp_nan_matrix.bin", "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    write_matrix(nan_matrix, f); // fail silencieux
    fclose(f);
    CU_PASS("write_matrix (NaN) ne segfault pas");

    f = fopen("tmp_nan_matrix.bin", "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    matrix *nan_read_m = read_matrix(f);
    fclose(f);
    CU_ASSERT_TRUE(isnan(nan_read_m->values[0][1]));
    free_matrix(nan_matrix);
    free_matrix(nan_read_m);
    remove("tmp_nan_matrix.bin");
    
    // Cas 6 : valeur sous mauvais format
    FILE *f4 = fopen("tmp.bin", "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f4);
    uint64_t wrong = 0x12345678ABCDEF00; 
    size_t written = fwrite(&wrong, 1, sizeof(uint64_t) - 1, f4); // écrit moins que sizeof(uint64_t)
    CU_ASSERT_EQUAL(written, sizeof(uint64_t) - 1);
    fclose(f4);

    f4 = fopen("tmp.bin", "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f4);
    CU_ASSERT_FALSE(read_matrix(f4)); // doit échouer car la taille est incorrecte
    fclose(f4);
    remove("tmp.bin");

    // Cas 7 : read_matrix échoue car ligne incomplète
    f = fopen("tmp_read_matrix_fail.bin", "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    uint64_t dims[2] = {htobe64(1), htobe64(2)};
    fwrite(dims, sizeof(uint64_t), 2, f);
    uint64_t row_index = htobe64(0);
    fwrite(&row_index, sizeof(uint64_t), 1, f);
    double partial_row[1] = {9.9}; // une seule valeur au lieu de 2
    fwrite(partial_row, sizeof(double), 1, f);
    fclose(f);

    f = fopen("tmp_read_matrix_fail.bin", "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    matrix *m_fail = read_matrix(f);
    CU_ASSERT_PTR_NULL(m_fail);
    fclose(f);
    remove("tmp_read_matrix_fail.bin");
}

void test_file_qr_rw(void) {
    matrix *Q = init_matrix(2, 2);
    matrix *R = init_matrix(2, 2);
    Q->values[0][0] = 1; Q->values[0][1] = 0;
    Q->values[1][0] = 0; Q->values[1][1] = 1;
    R->values[0][0] = 3; R->values[0][1] = 4;
    R->values[1][0] = 0; R->values[1][1] = 5;

    FILE *f = fopen("tmp_qr.bin", "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    write_QR(Q, R, f);
    fclose(f);

    matrix *Q2 = init_matrix(2, 2);
    matrix *R2 = init_matrix(2, 2);

    f = fopen("tmp_qr.bin", "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    read_QR(f, Q2, R2);
    fclose(f);

    for (uint64_t i = 0; i < 2; i++)
        for (uint64_t j = 0; j < 2; j++) {
            CU_ASSERT_DOUBLE_EQUAL(Q->values[i][j], Q2->values[i][j], 1e-6);
            CU_ASSERT_DOUBLE_EQUAL(R->values[i][j], R2->values[i][j], 1e-6);
        }

    // Cas 1 : Q est NULL
    f = fopen("tmp_qr_qnull.bin", "wb");
    if (f != NULL) {
        write_QR(NULL, R, f);
        fclose(f);
        CU_PASS("write_QR(NULL, R, f) ne segfault pas");
        remove("tmp_qr_qnull.bin");
    } else {
        CU_FAIL("Impossible d'ouvrir tmp_qr_qnull.bin");
    }

    // Cas 2 : R est NULL
    f = fopen("tmp_qr_rnull.bin", "wb");
    if (f != NULL) {
        write_QR(Q, NULL, f);
        fclose(f);
        CU_PASS("write_QR(Q, NULL, f) ne segfault pas");
        remove("tmp_qr_rnull.bin");
    } else {
        CU_FAIL("Impossible d'ouvrir tmp_qr_rnull.bin");
    }

    // Cas 3 : fichier NULL
    write_QR(Q, R, NULL);
    CU_PASS("write_QR(Q, R, NULL) ne segfault pas");

    // Cas 4 : lecture avec fichier NULL
    read_QR(NULL, Q2, R2);
    CU_PASS("read_QR(NULL, Q2, R2) ne segfault pas");

    // Cas 5 : Q2 ou R2 NULL
    f = fopen("tmp_qr.bin", "rb");
    if (f != NULL) {
        read_QR(f, NULL, R2);
        read_QR(f, Q2, NULL);
        fclose(f);
        CU_PASS("read_QR avec Q2 ou R2 NULL ne segfault pas");
    } else {
        CU_FAIL("Échec d'ouverture de tmp_qr.bin pour test NULL Q2/R2");
    }
    free_matrix(Q);
    free_matrix(R);
    free_matrix(Q2);
    free_matrix(R2);
    remove("tmp_qr.bin");

    // Cas 6 : écriture/lecture matrice avec NaN
    matrix *nan_Q = init_matrix(2, 1);
    matrix *nan_R = init_matrix(2, 1);
    nan_Q->values[0][0] = 1.0; nan_R->values[0][0] = NAN;
    nan_Q->values[1][0] = 3.0; nan_R->values[1][0] = 4.0;
    f = fopen("nanQR.bin", "wb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    write_QR(nan_Q, nan_R, f); // fail silencieux
    fclose(f);
    CU_PASS("write_matrix (NaN) ne segfault pas");

    f = fopen("nanQR.bin", "rb");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);
    matrix *newQ = init_matrix(2, 1);
    matrix *newR = init_matrix(2, 1);
    read_QR(f, newQ, newR);
    fclose(f);

    remove("nanQR.bin");
    free_matrix(nan_Q);
    free_matrix(nan_R);
    free_matrix(newQ);
    free_matrix(newR);
    CU_PASS("read_QR (NaN) ne segfault pas");
    
}




/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    EFFICACITÉ TEMPORELLE    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

// void TPS_test_add_v_v(void) {
//     uint64_t m2 = 1000000;
//     const int N_RUNS = 5;
//     double total_time_1thread = 0.0, total_time_4threads = 0.0;

//     for (int run = 0; run < N_RUNS; run++) {
//         struct timespec start, end;
//         vector *v_multithread = init_vector(m2);
//         vector *w_multithread = init_vector(m2);
//         vector *z_multithread = init_vector(m2);

//         for (uint64_t i = 0; i < m2; i++) {
//             v_multithread->values[i] = (double)rand() / RAND_MAX;
//             w_multithread->values[i] = (double)rand() / RAND_MAX;
//         }

//         clock_gettime(CLOCK_MONOTONIC_RAW, &start);
//         int i1 = add_v_v(v_multithread, w_multithread, z_multithread, 1);
//         clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//         total_time_1thread += (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

//         clock_gettime(CLOCK_MONOTONIC_RAW, &start);
//         int i2 = add_v_v(v_multithread, w_multithread, z_multithread, 4);
//         clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//         total_time_4threads += (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

//         CU_ASSERT_EQUAL(i1, 0);
//         CU_ASSERT_EQUAL(i2, 0);
//         free_vector(v_multithread);
//         free_vector(w_multithread);
//         free_vector(z_multithread);
//     }

//     double avg_1 = total_time_1thread / N_RUNS;
//     double avg_4 = total_time_4threads / N_RUNS;
//     CU_ASSERT(avg_4 < avg_1);
//     CU_ASSERT((avg_1 / avg_4) >= 1.3);
// }

// void TPS_test_sub_v_v(void) {

//     uint64_t m2 = 1000000;  /* initialisation */
//     struct timespec start, end;
//     vector *v_multithread = init_vector(m2); 
//     vector *w_multithread = init_vector(m2);
//     vector *z_multithread = init_vector(m2); 
//     for (uint64_t i = 0; i < m2; i++) {
//         v_multithread->values[i] = (double)rand() / RAND_MAX;
//         w_multithread->values[i] = (double)rand() / RAND_MAX;
//     }

//     clock_gettime(CLOCK_MONOTONIC_RAW, &start); /* 1 thread */
//     int i1 = sub_v_v(v_multithread,w_multithread,z_multithread,1);
//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double time_1thread = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
//     clock_gettime(CLOCK_MONOTONIC_RAW, &start); /* 4 threads */
//     int i2 = sub_v_v(v_multithread,w_multithread,z_multithread,4);
//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double time_4threads = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

//     CU_ASSERT_EQUAL(i1, 0); // Vérifie que sub_v_v() a marché à 1 thread
//     CU_ASSERT_EQUAL(i2, 0); // Vérifie que sub_v_v() a marché à 4 threads
//     CU_ASSERT(time_4threads < time_1thread); /* éval de parformance - 4 threads */
//     double improvement_ratio_1to4 = time_1thread / time_4threads;
//     CU_ASSERT(improvement_ratio_1to4 >= 1.3); // au moins 30% plus rapide ?

//     free_vector(v_multithread);
//     free_vector(w_multithread);
//     free_vector(z_multithread);
// }

// void TPS_test_add_m_m(void) {
//     uint64_t m2 = 10000;  /* initialisation */
//     uint64_t n2 = 100;
//     struct timespec start, end;
//     matrix *A_multithread = init_matrix(m2,n2); 
//     matrix *B_multithread = init_matrix(m2,n2);
//     matrix *C_multithread = init_matrix(m2,n2); 
//     for (uint64_t i = 0; i < m2; i++) {
//         for (uint64_t j = 0; j < n2; j++) {
//             A_multithread->values[i][j] = (double)rand() / RAND_MAX;
//             B_multithread->values[i][j] = (double)rand() / RAND_MAX;
//         }
//     }

//     clock_gettime(CLOCK_MONOTONIC_RAW, &start); /* 1 thread */
//     int i1 = add_m_m(A_multithread,B_multithread,C_multithread,1);
//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double time_1thread = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
//     clock_gettime(CLOCK_MONOTONIC_RAW, &start); /* 4 threads */
//     int i2 = add_m_m(A_multithread,B_multithread,C_multithread,4);
//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double time_4threads = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

//     CU_ASSERT_EQUAL(i1, 0); // Vérifie que add_m_m() a marché à 1 thread
//     CU_ASSERT_EQUAL(i2, 0); // Vérifie que add_m_m() a marché à 4 threads
//     CU_ASSERT(time_4threads < time_1thread); /* éval de parformance - 4 threads */
//     double improvement_ratio_1to4 = time_1thread / time_4threads;
//     CU_ASSERT(improvement_ratio_1to4 >= 1.2); // au moins 20% plus rapide ?

//     free_matrix(A_multithread);
//     free_matrix(B_multithread);
//     free_matrix(C_multithread);
// }

// void TPS_test_sub_m_m(void) {
//     uint64_t m2 = 10000;  /* initialisation */
//     uint64_t n2 = 100;
//     struct timespec start, end;
//     matrix *A_multithread = init_matrix(m2,n2); 
//     matrix *B_multithread = init_matrix(m2,n2);
//     matrix *C_multithread = init_matrix(m2,n2); 
//     for (uint64_t i = 0; i < m2; i++) {
//         for (uint64_t j = 0; j < n2; j++) {
//             A_multithread->values[i][j] = (double)rand() / RAND_MAX;
//             B_multithread->values[i][j] = (double)rand() / RAND_MAX;
//         }
//     }

//     clock_gettime(CLOCK_MONOTONIC_RAW, &start); /* 1 thread */
//     int i1 = sub_m_m(A_multithread,B_multithread,C_multithread,1);
//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double time_1thread = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
//     clock_gettime(CLOCK_MONOTONIC_RAW, &start); /* 4 threads */
//     int i2 = sub_m_m(A_multithread,B_multithread,C_multithread,4);
//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double time_4threads = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

//     CU_ASSERT_EQUAL(i1, 0); // Vérifie que sub_m_m() a marché à 1 thread
//     CU_ASSERT_EQUAL(i2, 0); // Vérifie que sub_m_m() a marché à 4 threads
//     CU_ASSERT(time_4threads < time_1thread); /* éval de parformance - 4 threads */
//     double improvement_ratio_1to4 = time_1thread / time_4threads;
//     CU_ASSERT(improvement_ratio_1to4 >= 1.2); // au moins 20% plus rapide ?

//     free_matrix(A_multithread);
//     free_matrix(B_multithread);
//     free_matrix(C_multithread);
// }

// void TPS_test_dot_prod(void) {

//     uint64_t m2 = 1000000;  /* initialisation */
//     struct timespec start, end;
//     vector *v_multithread = init_vector(m2); 
//     vector *w_multithread = init_vector(m2);
//     for (uint64_t i = 0; i < m2; i++) {
//         v_multithread->values[i] = (double)rand() / RAND_MAX;
//         w_multithread->values[i] = (double)rand() / RAND_MAX;
//     }

//     clock_gettime(CLOCK_MONOTONIC_RAW, &start); /* 1 thread */
//     int n1 = dot_prod(v_multithread, w_multithread, 1);
//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double time_1thread = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
//     clock_gettime(CLOCK_MONOTONIC_RAW, &start); /* 4 threads */
//     int n4 = dot_prod(v_multithread, w_multithread, 4);
//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double time_4threads = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

//     CU_ASSERT_DOUBLE_EQUAL(n1,n4,1e-3);
//     CU_ASSERT(time_4threads < time_1thread); /* éval de performance - 4 threads */
//     double improvement_ratio_1to4 = time_1thread / time_4threads;
//     CU_ASSERT(improvement_ratio_1to4 >= 1.2); // au moins 20% plus rapide ?

//     free_vector(v_multithread);
//     free_vector(w_multithread);
// }


// void TPS_test_norm(void) {

//     uint64_t m2 = 10000000;  /* initialisation */
//     struct timespec start, end;
//     vector *v_multithread = init_vector(m2); 
//     for (uint64_t i = 0; i < m2; i++) {
//         v_multithread->values[i] = (double)rand() / RAND_MAX;
//     }

//     clock_gettime(CLOCK_MONOTONIC_RAW, &start); /* 1 thread */
//     int n1 = norm(v_multithread,1);
//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double time_1thread = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
//     clock_gettime(CLOCK_MONOTONIC_RAW, &start); /* 4 threads */
//     int n4 = norm(v_multithread,4);
//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double time_4threads = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

//     CU_ASSERT_DOUBLE_EQUAL(n1,n4,1e-3);
//     CU_ASSERT(time_4threads < time_1thread); /* éval de performance - 4 threads */
//     double improvement_ratio_1to4 = time_1thread / time_4threads;
//     CU_ASSERT(improvement_ratio_1to4 >= 1.3); // au moins 30% plus rapide ?

//     free_vector(v_multithread);
// }

// void TPS_test_mult_m_v(void) {
//     uint64_t m2 = 10000;  /* initialisation */
//     uint64_t n2 = 100;
//     struct timespec start, end;
//     matrix *A_multithread = init_matrix(m2,n2); 
//     vector *x_multithread = init_vector(n2);
//     vector *z_multithread = init_vector(m2); 
//     for (uint64_t i = 0; i < m2; i++) {
//         for (uint64_t j = 0; j < n2; j++) {
//             A_multithread->values[i][j] = (double)rand() / RAND_MAX;
//             x_multithread->values[j] = (double)rand() / RAND_MAX;
//         }
//     }

//     clock_gettime(CLOCK_MONOTONIC_RAW, &start); /* 1 thread */
//     int i1 = mult_m_v(A_multithread,x_multithread,z_multithread,1);
//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double time_1thread = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
//     clock_gettime(CLOCK_MONOTONIC_RAW, &start); /* 4 threads */
//     int i2 = mult_m_v(A_multithread,x_multithread,z_multithread,4);
//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double time_4threads = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

//     CU_ASSERT_EQUAL(i1, 0); // Vérifie que mult_m_v() a marché à 1 thread
//     CU_ASSERT_EQUAL(i2, 0); // Vérifie que mult_m_v() a marché à 4 threads
//     CU_ASSERT(time_4threads < time_1thread); /* éval de performance - 4 threads */
//     double improvement_ratio_1to4 = time_1thread / time_4threads;
//     CU_ASSERT(improvement_ratio_1to4 >= 1.2); // au moins 20% plus rapide ?

//     free_matrix(A_multithread);
//     free_vector(x_multithread);
//     free_vector(z_multithread);
// }

// void TPS_test_mult_m_m(void) {
//     uint64_t m2 = 10000;  /* initialisation */
//     uint64_t n2 = 100;
//     struct timespec start, end;
//     matrix *A_multithread = init_matrix(m2,n2); 
//     matrix *B_multithread = init_matrix(n2,m2);
//     matrix *C_multithread = init_matrix(m2,m2); 
//     for (uint64_t i = 0; i < m2; i++) {
//         for (uint64_t j = 0; j < n2; j++) {
//             A_multithread->values[i][j] = (double)rand() / RAND_MAX;
//             B_multithread->values[j][i] = (double)rand() / RAND_MAX;
//         }
//     }

//     clock_gettime(CLOCK_MONOTONIC_RAW, &start); /* 1 thread */
//     int i1 = mult_m_m(A_multithread,B_multithread,C_multithread,1);
//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double time_1thread = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
//     clock_gettime(CLOCK_MONOTONIC_RAW, &start); /* 4 threads */
//     int i2 = mult_m_m(A_multithread,B_multithread,C_multithread,4);
//     clock_gettime(CLOCK_MONOTONIC_RAW, &end);
//     double time_4threads = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

//     CU_ASSERT_EQUAL(i1, 0); // Vérifie que mult_m_m() a marché à 1 thread
//     CU_ASSERT_EQUAL(i2, 0); // Vérifie que mult_m_m() a marché à 4 threads
//     CU_ASSERT(time_4threads < time_1thread); /* éval de parformance - 4 threads */
//     double improvement_ratio_1to4 = time_1thread / time_4threads;
//     CU_ASSERT(improvement_ratio_1to4 >= 1.2); // au moins 20% plus rapide ?

//     free_matrix(A_multithread);
//     free_matrix(B_multithread);
//     free_matrix(C_multithread);
// }

// void TPS_test_transp(void) {

//     uint64_t m2 = 100000;  /* initialisation */
//     uint64_t n2 = 1000;
//     struct timespec start, end;
//     matrix *A_multithread = init_matrix(m2,n2); 
//     matrix *C_multithread = init_matrix(n2,m2); 
//     for (uint64_t i = 0; i < m2; i++) {
//         for (uint64_t j = 0; j < n2; j++) {
//             A_multithread->values[i][j] = (double)rand() / RAND_MAX;
//         }
//     }

//     clock_gettime(CLOCK_MONOTONIC, &start); /* 1 thread */
//     int i1 = transp(A_multithread,C_multithread,1);
//     clock_gettime(CLOCK_MONOTONIC, &end);
//     double time_1thread = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
//     clock_gettime(CLOCK_MONOTONIC, &start); /* 4 threads */
//     int i2 = transp(A_multithread,C_multithread,4);
//     clock_gettime(CLOCK_MONOTONIC, &end);
//     double time_4threads = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

//     CU_ASSERT_EQUAL(i1, 0); // Vérifie que transp() a marché à 1 thread
//     CU_ASSERT_EQUAL(i2, 0); // Vérifie que transp() a marché à 4 threads
//     CU_ASSERT(time_4threads < time_1thread); /* éval de parformance - 4 threads */
//     double improvement_ratio_1to4 = time_1thread / time_4threads;
//     CU_ASSERT(improvement_ratio_1to4 >= 1.3); // au moins 30% plus rapide ?

//     free_matrix(A_multithread);
//     free_matrix(C_multithread);
// }

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    EXÉCUTION DES TESTS    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

int main(int argc, char **argv) {
    srand(time(NULL));
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }
    CU_pSuite test_basic_op = CU_add_suite("test_basic_op", NULL, NULL);
    if (test_basic_op == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    CU_pSuite test_perf_tps = CU_add_suite("test_perf_tps", NULL, NULL);
    if (test_perf_tps == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    CU_pSuite test_file = CU_add_suite("test_file", NULL, NULL);
    if (test_file== NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if ((CU_add_test(test_basic_op, "add_v_v", test_add_v_v) == NULL) || 
        (CU_add_test(test_basic_op, "add_m_m", test_add_m_m) == NULL) ||
        (CU_add_test(test_basic_op, "sub_v_v", test_sub_v_v) == NULL) ||
        (CU_add_test(test_basic_op, "sub_m_m", test_sub_m_m) == NULL) ||
        (CU_add_test(test_basic_op, "dot_prod", test_dot_prod) == NULL) ||
        (CU_add_test(test_basic_op, "norm", test_norm) == NULL) ||
        (CU_add_test(test_basic_op, "mult_m_v", test_mult_m_v) == NULL) || 
        (CU_add_test(test_basic_op, "mult_m_m", test_mult_m_m) == NULL) ||
        (CU_add_test(test_basic_op, "transp", test_transp) == NULL) ||
        (CU_add_test(test_basic_op, "back_sub", test_back_sub) == NULL) ||
        (CU_add_test(test_basic_op, "qr", test_qr) == NULL) ||
        (CU_add_test(test_basic_op, "is_upper_triang", test_is_upper_triang) == NULL) ||
        (CU_add_test(test_basic_op, "lstsq", test_lstsq) == NULL) ||

        (CU_add_test(test_basic_op, "print_matrix", test_print_matrix_should_not_segfault_on_empty) == NULL) ||
        (CU_add_test(test_basic_op, "print_vector", test_print_vector_should_not_segfault_on_empty) == NULL) ||

        // (CU_add_test(test_perf_tps, "TPS_add_v_v", TPS_test_add_v_v) == NULL) || /* tests d'efficacité temporelle du multithreading */
        // (CU_add_test(test_perf_tps, "TPS_add_m_m", TPS_test_add_m_m) == NULL) ||
        // (CU_add_test(test_perf_tps, "TPS_sub_v_v", TPS_test_sub_v_v) == NULL) ||
        // (CU_add_test(test_perf_tps, "TPS_sub_m_m", TPS_test_sub_m_m) == NULL) ||
        // (CU_add_test(test_perf_tps, "TPS_dot_prod", TPS_test_dot_prod) == NULL) ||
        // (CU_add_test(test_perf_tps, "TPS_norm", TPS_test_norm) == NULL) ||
        // (CU_add_test(test_perf_tps, "TPS_mult_m_v", TPS_test_mult_m_v) == NULL) || 
        // (CU_add_test(test_perf_tps, "TPS_mult_m_m", TPS_test_mult_m_m) == NULL) ||
        // (CU_add_test(test_perf_tps, "TPS_transp", TPS_test_transp) == NULL) ||
        
        (CU_add_test(test_file, "read/write u64", test_file_conversions) == NULL) ||
        (CU_add_test(test_file, "read/write double", test_file_double_rw) == NULL) ||
        (CU_add_test(test_file, "read/write vector", test_file_vector_rw) == NULL) ||
        (CU_add_test(test_file, "read/write matrix", test_file_matrix_rw) == NULL) ||
        (CU_add_test(test_file, "read/write QR", test_file_qr_rw) == NULL)
        ) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());
    int failed_tests = CU_get_number_of_failures();
    CU_cleanup_registry();
    return (failed_tests > 0) ? 1 : 0;
}

//test