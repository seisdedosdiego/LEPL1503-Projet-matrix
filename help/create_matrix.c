#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../headers/file.h"
#include "../headers/matrix.h"

// Parse une ligne "1.0 2.0 3.0" vers un tableau de doubles
void parse_line_to_array(const char *line, double *array, uint64_t *count) {
    char *tmp = strdup(line);
    char *token = strtok(tmp, " ");
    *count = 0;
    while (token) {
        array[(*count)++] = atof(token);
        token = strtok(NULL, " ");
    }
    free(tmp);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s [name] \"[[a b] [c d] ...]\"\n", argv[0]);
        return -1;
    }

    const char *name = argv[1];
    const char *input = argv[2];
    int len = strlen(input);

    if (input[0] != '[' || input[len - 1] != ']') {
        fprintf(stderr, "Erreur : la matrice doit être entre doubles crochets. Exemple : \"[[1.0 2.0] [3.0 4.0]]\"\n");
        return -1;
    }

    // Nettoyer les crochets extérieurs
    char *copy = strdup(input + 1);
    copy[len - 2] = '\0';

    char *lines[256]; // max 256 lignes
    uint64_t rows = 0, cols = 0;

    // Parser manuellement les lignes entre crochets
    char *p = copy;
    while (*p) {
        while (*p && *p != '[') p++;
        if (!*p) break;
        p++; // sauter le [

        char *start = p;
        while (*p && *p != ']') p++;
        if (!*p) break;

        size_t linelen = p - start;
        char *line = malloc(linelen + 1);
        strncpy(line, start, linelen);
        line[linelen] = '\0';

        // Vérification des colonnes
        uint64_t current_cols = 0;
        double dummy[256];
        parse_line_to_array(line, dummy, &current_cols);

        if (cols == 0) {
            cols = current_cols;
        } else if (current_cols != cols) {
            fprintf(stderr, "Erreur : ligne %llu a %llu colonnes au lieu de %llu.\n", rows + 1, current_cols, cols);
            free(line);
            for (uint64_t i = 0; i < rows; i++) free(lines[i]);
            free(copy);
            return -1;
        }

        lines[rows++] = line;
        p++; // sauter le ]
    }

    if (rows == 0 || cols == 0) {
        fprintf(stderr, "Erreur : matrice vide ou invalide.\n");
        free(copy);
        return -1;
    }

    // Initialiser la matrice
    matrix *m = init_matrix(rows, cols);
    if (!m) {
        fprintf(stderr, "Erreur d'allocation.\n");
        for (uint64_t i = 0; i < rows; i++) free(lines[i]);
        free(copy);
        return -1;
    }

    // Remplir la matrice
    for (uint64_t r = 0; r < rows; r++) {
        uint64_t count = 0;
        parse_line_to_array(lines[r], m->values[r], &count);
        free(lines[r]);
    }

    // Écrire le fichier
    char filename[256];
    snprintf(filename, sizeof(filename), "%s.bin", name);
    FILE *f = fopen(filename, "wb");
    if (!f) {
        perror("fopen");
        free_matrix(m);
        free(copy);
        return -1;
    }

    write_matrix(m, f);
    fclose(f);

    free_matrix(m);
    free(copy);

    printf("Matrice sauvegardée dans %s (%llux%llu)\n", filename, rows, cols);
    return 0;
}