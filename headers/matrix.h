#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> // multithreading

#include <math.h>  // ajout de cette librairie
#include <float.h> // pour utiliser DBL_EPSILON

#ifndef _MATRIX_H_
#define _MATRIX_H_

typedef enum {
    MATRIX,
    VECTOR

} TypeObjet;

typedef struct {
    TypeObjet type;  // Sers à différencier les matrices des vecteurs
    uint64_t m;      // Nombre de lignes de la matrice
    uint64_t n;      // Nombre de colonnes de la matrice
    double **values; // Les valeurs contenues dans la matrice
} matrix;

typedef struct {
    matrix *A;       // 1ere matrice d'entrée
    matrix *B;       // 2eme matrice d'entrée (optionnel)
    matrix *C;       // matrice de sortie
    uint64_t start;  // indice de début d'un thread
    uint64_t end;    // indice de fin d'un thread
} thread_matrix;

extern pthread_mutex_t mutex_m; /* 'extern' car sinon erreur de duplicate symbols for 'matrix_mutex' */

/**
 * Initialise une matrice de taille m x n avec des 0.
 *
 * @param uint64_t m le nombre de lignes de la matrice
 * @param uint64_t n le nombre de colonnes de la matrice
 *
 * @return un pointeur vers une matrice de taille m x n initialisée avec des 0.
 */
matrix *init_matrix(uint64_t, uint64_t);

/**
 * Libère la mémoire allouée lors de la création d'une matrice : supprime cette matrice. 
 * 
 * @param matrix* M la matrice à supprimer
 */
void free_matrix(matrix *);

/**
 * Imprime sur la sortie standard le contenu d'une matrice.
 *
 * @param matrix* un pointeur vers la matrice à imprimer
 *
 * @result la matrice a été imprimée sur la sortie standard.
 */
void print_matrix(matrix *);

/**
 * Fonction exécutée par chaque thread pour calculer une partie de la somme de deux matrices.
 *
 * @param void* args un pointeur vers une structure thread_matrix contenant A, B, C, start et end
 * 
 * @return NULL
 */
void *add_m_m_thread(void *args);

/**
 * Calcule la somme de deux matrices
 *
 * @param matrix* A la première matrice
 * @param matrix* B la seconde matrice
 * @param matrix* C une matrice avec des valeurs quelconques dans laquelle la
 * somme de A avec B sera stockée
 * @param int8_t nb_threads le nombre de threads à utiliser
 *
 * @result C = A + B
 */
int add_m_m(matrix *, matrix *, matrix *, int8_t);

/**
 * Fonction exécutée par chaque thread pour calculer une partie de la différence de deux matrices.
 *
 * @param void* args un pointeur vers une structure thread_matrix contenant A, B, C, start et end
 * 
 * @return NULL
 */
void *sub_m_m_thread(void *args);

/**
 * Calcule la différence de deux matrices en multithreading
 *
 * @param matrix* A la première matrice
 * @param matrix* B la seconde matrice
 * @param matrix* C une matrice avec des valeurs quelconques dans laquelle la
 * différence de A avec B sera stockée
 * @param int8_t nb_threads le nombre de threads à utiliser
 *
 * @result C = A - B
 */
int sub_m_m(matrix *, matrix *, matrix *, int8_t);

/**
 * Fonction exécutée par chaque thread pour calculer une partie du produit de deux matrices.
 *
 * @param void* args un pointeur vers une structure thread_matrix contenant A, B, C, start et end
 * 
 * @return NULL
 */
void *mult_m_m_thread(void *args);

 /**
 * Calcule le produit de 2 matrices multi-threadé
 * @param matrix* A la première matrice
 * @param matrix* B la seconde matrice
 * @param matrix* C une matrice aux valeurs quelconques dans laquelle le
 * produit matriciel de A et B sera stocké ; si C est non nulle, ses valeurs 
 * sont remplacées par le résultat obtenu
 * @param int8_t nb_threads le nombre de threads à utiliser, compris entre 1 et le nombre de lignes de A
 * 
 * @result C = A x B
 * 
 * @return 0 si l'opération a réussi, -1 sinon
 */
int mult_m_m(matrix *, matrix *, matrix *,int8_t);

/**
 * Fonction exécutée par chaque thread pour calculer une partie de la transposée d'une matrice.
 *
 * @param void* args un pointeur vers une structure thread_matrix contenant A, B (ici B = A^T), start et end
 * 
 * @return NULL
 */
void *transp_thread(void *args);

/**
 *  Transpose une matrice
 * 
 * @param matrix * la matrice à transposer
 * @param matrix * la matrice transposée
 * @param int8_t nb_threads le nombre de threads à utiliser
 * 
 * @result la matrice transposée
 * 
 */
int transp(void *, void *, int8_t);

/**
 * Vérifie si la matrice fournie est triangulaire supérieure
 *
 * @param matrix* A la matrice à vérifier
 *
 * @result 0 si la matrice est triangulaire supérieure, -1 sinon
 */
int is_upper_triang(matrix *);

/**
 * Decomposition QR qu'une matrice (A = QR)
 *
 * @param matrix* A la matrice 
 * @param matrix* Q la matrice ayant des colonnes orthogonales: Q^T * Q = Id
 * @param matrix* R la matrice triangulaire supérieure
 * @param int8_t nb_threads le nombre de threads à utiliser
 *  
 * @result La décomposition de la matrice A ∈ R^(n*m) si m >= n 
 * 
 * @return 0 si l'opération a réussi, -1 sinon
 */
int qr(matrix *, matrix *, matrix *, int8_t);

#endif /* _MATRIX_H_ */