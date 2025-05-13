#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../headers/file.h"
#include "../headers/vector.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s [name] \"[1.0 2.0 ... n]\"\n", argv[0]);
        return -1;
    }

    const char *name = argv[1];
    const char *input = argv[2];
    int len = strlen(input);

    if (input[0] != '[' || input[len - 1] != ']') {
        fprintf(stderr, "Erreur : le vecteur doit être entre crochets. Exemple : \"[1.0 2.0 3.0]\"\n");
        return -1;
    }

    char *copy = strdup(input + 1);
    copy[len - 2] = '\0';       

    // --- Première passe : compter les éléments ---
    uint64_t size = 0;
    char *scan = strdup(copy); // copie temporaire pour le comptage
    char *token = strtok(scan, " ");
    while (token) {
        size++;
        token = strtok(NULL, " ");
    }
    free(scan);

    // --- Allocation dynamique ---
    double *values = malloc(size * sizeof(double));
    if (!values) {
        perror("malloc");
        free(copy);
        return 1;
    }

    // --- Deuxième passe : remplir le tableau ---
    token = strtok(copy, " ");
    for (uint64_t i = 0; i < size; i++) {
        values[i] = atof(token);
        token = strtok(NULL, " ");
    }

    vector *v = init_vector(size);
    for (uint64_t i = 0; i < size; i++) {
        v->values[i] = values[i];
    }

    char filename[256];
    snprintf(filename, sizeof(filename), "%s.bin", name);
    FILE *f = fopen(filename, "wb");
    if (!f) {
        perror("fopen");
        free_vector(v);
        free(values);
        free(copy);
        return -1;
    }

    write_vector(v, f);
    fclose(f);

    free_vector(v);
    free(values);
    free(copy);

    printf("Vecteur sauvegardé dans %s\n", filename);
    return 0;
}