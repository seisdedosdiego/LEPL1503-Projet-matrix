#include "../headers/matrix.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> // multithreading

#ifndef _VECTOR_H_
#define _VECTOR_H_

typedef struct vector{
    TypeObjet type; // Sers à différencier les matrices des vecteurs
    uint64_t m;     // La taille du vecteur
    double *values; // Les valeurs contenues dans le vecteur
} vector;

typedef struct {
    vector *x;       // 1er vecteur d'entrée
    vector *y;       // 2eme vecteur d'entrée (optionnel)
    vector *z;       // vecteur de sortie
    uint64_t start;  // indice de début d'un thread
    uint64_t end;    // indice de fin d'un thread
    double result;   // résultat d'opérations qui renvoient un double
    matrix *A;       // matrice d'entrée (optionnelle)
} thread_vector;

extern pthread_mutex_t mutex_v; /* 'extern' car sinon erreur de duplicate symbols for 'vector_mutex' */


/**
 * Initialise un vecteur de taille m avec des zéros.
 *
 * @param uint64_t m la taille du vecteur
 *
 * @return un pointeur vers un vecteur de taille m initialisé avec des 0.
 */
vector *init_vector(uint64_t);

/**
 * Libère la mémoire allouée lors de la création d'une matrice : supprime cette matrice. 
 * 
 * @param matrix* M la matrice à supprimer
 */
 void free_vector(vector *);

/**
 * Imprime sur la sortie standard le contenu d'un vecteur.
 *
 * @param vector* un pointeur vers le vecteur à imprimer
 *
 * @result le vecteur a été imprimé sur la sortie standard.
 */
void print_vector(vector *);

/**
 * Fonction exécutée par chaque thread pour calculer une partie de la somme de deux vecteurs.
 *
 * @param void* args un pointeur vers une structure thread_vector contenant x, y, z, start et end
 * 
 * @return NULL
 */
void *add_v_v_thread(void *args);

/**
 * Calcule la somme de deux vecteurs
 *
 * @param vector* x le premier vecteur
 * @param vector* y le second vecteur
 * @param vector* z un vecteur avec des valeurs quelconques dans lequel la somme
 * de x avec y sera stockée
 * @param int8_t nb_threads le nombre de threads à utiliser
 *
 * @result z = x + y
 * 
 */
int add_v_v(vector *, vector *, vector *, int8_t);


/**
 * Fonction exécutée par chaque thread pour calculer une partie de la différence de deux vecteurs.
 *
 * @param void* args un pointeur vers une structure thread_vector contenant x, y, z, start et end
 * 
 * @return NULL
 */
void *sub_v_v_thread(void *args);

/**
 * Calcule la différence de deux vecteurs en multithreading
 *
 * @param vector* x le premier vecteur
 * @param vector* y le second vecteur
 * @param vector* z un vecteur avec des valeurs quelconques dans lequel la différence
 * de x avec y sera stockée
 * @param int8_t nb_threads le nombre de threads à utiliser
 *
 * @result z = x - y
 */
int sub_v_v(vector *, vector *, vector *, int8_t);

/**
 * Fonction exécutée par chaque thread pour calculer une partie du produit scalaire de deux vecteurs.
 *
 * @param void* args un pointeur vers une structure thread_vector contenant x, y, start, end et result
 * 
 * @return NULL
 */
void *dot_prod_thread(void *args);

/**
 * Calcule le produit scalaire de deux vecteurs en multithreading
 *
 * @param vector* x le premier vecteur
 * @param vector* y le second vecteur
 * @param double* result un double dans lequel le résultat sera stocké
 * @param int8_t nb_threads le nombre de threads à utiliser
 *
 * @result z = x . y
 */
double dot_prod(vector *, vector *, int8_t);


/**
 * Fonction exécutée par chaque thread pour calculer la somme des carrés des éléments d'un vecteur (pour la norme).
 *
 * @param void* args un pointeur vers une structure thread_vector contenant x, start, end et result
 * 
 * @return NULL
 */
void *norm_thread(void *args);

/**
 * Calcule la norme d'un vecteur
 * 
 * @param  vector* x le vecteur dont on calcule la norme
 * @param int8_t nb_threads le nombre de threads à utiliser
 * 
 * @result ||x||
 */
double norm(vector *, int8_t);


/**
 * Fonction exécutée par chaque thread pour calculer une partie du produit d'une matrice par un vecteur.
 *
 * @param void* args un pointeur vers une structure thread_vector contenant A, x, z, start et end
 * 
 * @return NULL
 */
void *mult_m_v_thread(void *args);

/**
 * Calcule la multiplication d'une matrice et d'un vecteur 
 * @param matrix* A la matrice
 * @param vecteur* v le vecteur
 * @param vecteur* un vecteur aux valeurs quelconques dans laquelle le
 * produit de A et v sera stocké ; si w est non nulle, ses valeurs 
 * sont remplacées par le résultat obtenu
 * @param int8_t nb_threads le nombre de threads à utiliser
 * 
 * @result w = A x v
 * 
 * @return 0 si l'opération a réussi, -1 sinon
 */
int mult_m_v(matrix *, vector *, vector *, int8_t);



/**
 * Applique un algorithme de back substitution sur une matrice triangulaire supérieure
 * 
 * @param matrix *U la matrice triangulaire supérieure
 * @param vector *v le vecteur
 * @param vector *w le vecteur avec des valeurs quelconques dans lequel le résultat sera stocké
 * 
 * @return 0 si l'opération a réussi, -1 sinon
 */
int back_sub(matrix *, vector *, vector *);

/**
 * Résolution d'un système linéaire Ax = b
 * @param matrix* A la matrice
 * @param vector* b le vecteur
 * @param vector* x le vecteur solution
 * @param int8_t nb_threads le nombre de threads à utiliser
 * 
 * @return 0 si l'opération a réussi, -1 sinon
 */
int lstsq(matrix*, vector*, vector*, int8_t);

#endif/*_VECTOR_H_*/