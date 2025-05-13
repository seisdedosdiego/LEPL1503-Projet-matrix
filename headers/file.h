#ifdef __APPLE__
    #include "../headers/endian.h"
#else
    #include <endian.h>
#endif

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "matrix.h"
#include "vector.h"

#ifndef _FILE_H_
#define _FILE_H_

/**
 * Écrit un uint64_t dans un fichier
 * 
 * @param uint64_t val la valeur à convertir + écrire
 * @param FILE* f le fichier dans lequel il faut écrire
 */
bool write_u64_be(uint64_t, FILE *);

/**
 * Lit un uint64_t dans un fichier 
 * 
 * @param FILE* f le fichier dans lequel il faut lire
 * @param uint64_t* tmp un pointeur vers la variable dans laquelle il faut stocker le nombre lu + converti
 */
bool read_u64_be(FILE *, uint64_t*);

/**
 * Lit le contenu d'un fichier contenant un double
 *
 * @param FILE* un pointeur vers un fichier ouvert contenant un double
 *
 * @return double le double qui était dans le fichier
 *
 * Remarque : cette fonction n'est là que pour vous aider à vérifier que votre
 * résultat est correct, mais vous ne devez pas l'implémenter (sauf si vous le
 * désirez :))
 */
double read_double(FILE *);

/**
 * Écrit un double dans un fichier
 *
 * @param double la valeur à écrire
 * @param FILE*  un pointeur vers un fichier ouvert dans lequel il faut écrire
 * le double
 *
 * @result la valeur a été écrite dans le fichier
 */
void write_double(double, FILE *);

/**
 * Lit le contenu d'un fichier contenant un vecteur
 *
 * @param FILE* un pointeur vers un fichier ouvert contenant un vecteur
 *
 * @return vector* un pointeur vers un vecteur ayant pour contenu le vecteur
 * représenté dans le fichier
 */
vector *read_vector(FILE *);

/**
 * Écrit le contenu d'un vecteur dans un fichier
 *
 * @param vector* un pointeur vers un vecteur
 * @param FILE*   un pointeur vers un fichier ouvert dans lequel il faut écrire
 * le contenu du vecteur
 *
 * @result le contenu du vecteur a été écrit dans le fichier
 */
void write_vector(vector *, FILE *);

/**
 * Lit le contenu d'un fichier contenant une matrice
 *
 * @param FILE* un pointeur vers un fichier ouvert contenant une matrice
 *
 * @return matrix* un pointeur vers une matrice ayant pour contenu la matrice
 * représentée dans le fichier
 */
matrix *read_matrix(FILE *);

/**
 * Écrit le contenu d'une matrice dans un fichier
 *
 * @param matrix* un pointeur vers une matrice
 * @param FILE* un pointeur vers un fichier ouvert dans lequel il faut écrire le
 * contenu de la matrice
 *
 * @result le contenu de la matrice a été écrit dans le fichier
 */
void write_matrix(matrix *, FILE *);

/**
 * Lit le contenu d'un fichier contenant la décomposition QR d'une matrice
 *
 * @param FILE*   un pointeur vers un fichier ouvert contenant la décomposition
 * QR
 * @param matrix* un pointeur vers une matrice dans laquelle Q sera stockée
 * @param matrix* un pointeur vers une matrice dans laquelle R sera stockée
 *
 * @result les matrices Q et R ont été lues et mises dans les matrices
 * correspondantes
 *
 * Remarque : cette fonction n'est là que pour vous aider à vérifier que votre
 * résultat est correct, mais vous ne devez pas l'implémenter (sauf si vous le
 * désirez :))
 */
void read_QR(FILE *, matrix *, matrix *);

/**
 * Ecrit la décomposition QR d'une matrice dans un fichier
 *
 * @param matrix* Q, un pointeur vers la matrice Q de la décomposition QR
 * @param matrix* R, un pointeur vers la matrice R de la décomposition QR
 * @param FILE* un pointeur vers un fichier ouvert dans lequel il faut écrire le
 * contenu des matrices Q et R
 *
 * @result le contenu des matrices Q et R a été écrit dans le fichier, suivant
 * le format de l'énoncé
 */
void write_QR(matrix *, matrix *, FILE *);

#endif /* _FILE_H_ */