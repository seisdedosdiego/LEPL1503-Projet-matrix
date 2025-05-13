#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#ifdef __APPLE__
    #include "../headers/endian.h"
#else
    #include <endian.h>
#endif
#include "../headers/file.h"
#include "../headers/vector.h"
#include "../headers/matrix.h"

// Écriture d’un uint64_t en big-endian
bool write_u64_be(uint64_t val, FILE *f) {
    if (!f) {
        fprintf(stderr, "write_u64_be: fichier NULL\n");
        return false; // Ne pas exit, retour sécurisé
    }
    uint64_t tmp = htobe64(val);
    if (fwrite(&tmp, sizeof(uint64_t), 1, f) != 1) {
        perror("fwrite (uint64_t)");
        return false;
    }
    return true;
}

// Lecture d’un uint64_t en big-endian
bool read_u64_be(FILE *f, uint64_t *tmp) {
    if (!f || !tmp) {
        fprintf(stderr, "read_u64_be: argument NULL\n");
        return false; // Ne pas exit, retour sécurisé
    }
    uint64_t tmpBE = 0;
    if (fread(&tmpBE, sizeof(uint64_t), 1, f) != 1) {
        perror("fread (uint64_t)");
        return false;  // Ne pas exit, retour sécurisé
    }
    *tmp = be64toh(tmpBE);
    return true;
}

// Écriture d’un double en little-endian
void write_double(double val, FILE *f) {
    if (!f) {
        fprintf(stderr, "write_double: fichier NULL\n");
        return; // Ne pas exit, retour sécurisé
    }
    uint64_t bits;
    memcpy(&bits, &val, sizeof(double));
    bits = htole64(bits);
    if (fwrite(&bits, sizeof(uint64_t), 1, f) != 1) {
        perror("fwrite (double)");
        return;
    }
}

// Lecture d’un double en little-endian
double read_double(FILE *f) {
    if (!f) {
        fprintf(stderr, "read_double: fichier NULL\n");
        return NAN; // Ne pas exit, retourne valeur invalide
    }
    uint64_t bits;
    if (fread(&bits, sizeof(uint64_t), 1, f) != 1) {
        perror("fread (double)");
        return NAN;  // Ne pas exit, retourne valeur invalide
    }
    bits = le64toh(bits);
    double val;
    memcpy(&val, &bits, sizeof(double)); 
    return val;
}

// VECTEURS
void write_vector(vector *v, FILE *f) {
    if (!v || !f) {
        fprintf(stderr, "write_vector: vector ou fichier NULL\n");
        return;
    }
    //uint64_t vm = htobe64((uint64_t) v->m);
    if (!write_u64_be((uint64_t) v->m, f)) {
        fprintf(stderr, "write_vector: écriture de la taille (m) échoue\n");
        return;
    }
    if (fwrite(v->values, sizeof(double), v->m, f) != v->m) {
        fprintf(stderr, "write_vector: écriture du vecteur échoue\n");
        return;
    }
}

vector *read_vector(FILE *f) {
    if (!f) {
        fprintf(stderr, "read_vector: fichier NULL\n");
        return NULL;
    }
    fseek(f, 0, SEEK_SET); // Remet le curseur au début du fichier

    uint64_t size = 0;
    if (!read_u64_be(f, &size)) {
        fprintf(stderr, "read_vector: lecture de la taille (m) échoue\n");
        return NULL;
    }

    vector *v = init_vector(size);
    if (v == NULL) {
        fprintf(stderr, "read_vector: allocation de l'espace mémoire pour un vecteur échoue : %s\n", strerror(errno));
        return NULL;
    }

    if (fread(v->values, sizeof(double), v->m, f) != v->m){
        perror("read_vector : lecture du vecteur échoue\n");
        free_vector(v);
        return NULL;
    } 

    //for (uint64_t i = 0; i < v->m; i++) {
    //    if (isnan(v->values[i]) || isinf(v->values[i])) {
    //        fprintf(stderr, "Valeur invalide à l’index %lu : %f\n", i, v->values[i]);
    //        free_vector(v);
    //        return NULL; // ou exit
    //    }
    //}

    return v;
}

// MATRICES
void write_matrix(matrix *A, FILE *f) {
    if (!A || !f) {
        fprintf(stderr, "write_matrix: matrix ou fichier NULL\n");
        return;
    }
    //uint64_t Am = htobe64((uint64_t) A->m);
    //uint64_t An = htobe64((uint64_t) A->n);
    if (!write_u64_be(A->m, f) || !write_u64_be(A->n, f)) {
        fprintf(stderr, "write_matrix: écriture de la taille (m, n) échoue\n");
        return;
    }

    if (A->m == 0 && A->n == 0) {
        return;
    }

    for (uint64_t i = 0; i < A->m; i++) {
        uint64_t ROW = htobe64(i);
        if (fwrite(&ROW, sizeof(uint64_t), 1, f) != 1) {
            fprintf(stderr, "write_matrix: écriture d'index de ligne échoue\n");
            return;
        }
        if(fwrite(A->values[i],sizeof(double),A->n,f) != A->n){
            perror("write_matrix: écriture de la ligne échoue");
            exit(EXIT_FAILURE);
        }
        
        // for (uint64_t j = 0; j < A->n; j++) {
        //     uint64_t ROW = htobe64((uint64_t) i);
        //     write_u64_be(ROW, f);
        //     if (fwrite(A->values[i], sizeof(double), A->n, f) != A->n) {
        //         fprintf(stderr, "read_matrix: écriture ligne de doubles échoue");
        //         return;
        //     }
        // }
    }
}

matrix *read_matrix(FILE *f) {
    if (!f) {
        fprintf(stderr, "read_matrix: fichier NULL\n");
        return NULL;
    }
    fseek(f, 0, SEEK_SET); // Remet le curseur au début du fichier

    uint64_t m = 0;
    uint64_t n = 0;
    if (!read_u64_be(f, &m) || !read_u64_be(f, &n)) {// dimensions converties (big endian to host)
        fprintf(stderr, "read_matrix: lecture de la taille (m, n) échoue\n");
        return NULL;
    }

    matrix *A = init_matrix(m, n);
    if (!A || !A->values) {
        fprintf(stderr, "read_matrix: échec d'initialisation\n");
        return NULL;
    }

    for (uint64_t i = 0; i < A->m; i++) {

        uint64_t index;
        if (fread(&index, sizeof(uint64_t), 1, f) != 1) {
            fprintf(stderr, "read_matrix: lecture d'index de ligne échoue");
            free_matrix(A);
            return NULL;
        }

        if (fread(A->values[i], sizeof(double), A->n, f) != A->n) {
            fprintf(stderr, "read_matrix: lecture de la ligne échoue\n");
            free_matrix(A);
            return NULL;
        }

        //for (uint64_t j = 0; j < A->n; j++) {
        //    if (isnan(A->values[i][j]) || isinf(A->values[i][j])) {
        //        fprintf(stderr, "Valeur invalide détectée à [%lu][%lu] : %f\n", i, j, A->values[i][j]);
        //        free_matrix(A);
        //        return NULL;
        //    }
        //}
    }

    return A;
}

// MATRICES QR
void write_QR(matrix *Q, matrix *R, FILE *file) {
    if (!Q || !R || !file) {
        fprintf(stderr, "write_QR: il y a un pointeur NULL en argument\n");
        return;
    }
    write_matrix(Q, file);

    for (uint64_t i = 0; i < R->m; i++) {

        uint64_t ROW = htobe64(i);
        if (fwrite(&ROW, sizeof(uint64_t), 1, file) != 1) {
            fprintf(stderr, "write_QR: écriture d'index de ligne échoue\n");
            return;
        }
        if(fwrite(R->values[i],sizeof(double),R->n,file) != R->n){
            perror("write_QR: écriture de la ligne échoue\n");
            exit(EXIT_FAILURE);
        }
        // for (uint64_t j = 0; j < R->n; j++) {
        //     uint64_t ROW = htobe64((uint64_t) i);
        //     write_u64_be(ROW, file);
        //     if (fwrite(R->values[i], sizeof(double), R->n, file) != R->n) {
        //         fprintf(stderr, "read_matrix: écriture ligne de doubles échoue");
        //         return;
        //     }
        // }
    }

    if (ferror(file)) {
        perror("fwrite");
    }
    fflush(file);
}

void read_QR(FILE *file, matrix *Q, matrix *R) {
    if (!file || !Q || !R) {
        fprintf(stderr, "read_QR: pointeur NULL\n");
        return;
    }

    uint64_t m_be, n_be;
    if (fread(&m_be, sizeof(uint64_t), 1, file) != 1 ||
        fread(&n_be, sizeof(uint64_t), 1, file) != 1) {
        perror("read_QR: lecture des dimensions échoue");
        return;
    }

    uint64_t m = be64toh(m_be);
    uint64_t n = be64toh(n_be);

    if (m != Q->m || n != Q->n || m != R->m || n != R->n) {
        fprintf(stderr, "read_QR: dimensions incompatibles avec Q ou R\n");
        return;
    }

    for (uint64_t i = 0; i < m; ++i) {
        uint64_t row_be;
        if (fread(&row_be, sizeof(uint64_t), 1, file) != 1) {
            perror("read_QR: lecture index ligne Q");
            return;
        }

        if (fread(Q->values[i], sizeof(double), n, file) != n) {
            perror("read_QR: lecture ligne Q");
            return;
        }

        //for (uint64_t j = 0; j < n; ++j) {
        //    if (isnan(Q->values[i][j]) || isinf(Q->values[i][j])) {
        //        fprintf(stderr, "read_QR: valeur invalide dans Q[%lu][%lu] : %f\n", i, j, Q->values[i][j]);
        //        return;
        //    }
        //}
    }

    for (uint64_t i = 0; i < m; ++i) {
        uint64_t row_be;
        if (fread(&row_be, sizeof(uint64_t), 1, file) != 1) {
            perror("read_QR: lecture index ligne R");
            return;
        }

        if (fread(R->values[i], sizeof(double), n, file) != n) {
            perror("read_QR: lecture ligne R");
            return;
        }

        //for (uint64_t j = 0; j < n; ++j) {
        //    if (isnan(R->values[i][j]) || isinf(R->values[i][j])) {
        //        fprintf(stderr, "read_QR: valeur invalide dans R[%lu][%lu] : %f\n", i, j, R->values[i][j]);
        //        return;
        //    }
        //}
    }
}
