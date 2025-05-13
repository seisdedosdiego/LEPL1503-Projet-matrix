#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // pour les vérifications de CPU

#include "../headers/file.h"
#include "../headers/matrix.h"
#include "../headers/vector.h"

typedef struct {
    bool verbose;
    uint8_t nb_threads;
    FILE *output_stream;
    uint8_t deg;
    char *op;
    FILE *input_file_A;
    FILE *input_file_B;
} args_t;

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%      INITIALISATION ET FONCTIONS DE BASE     %%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */


void free_all(args_t *args) { // nouveau
    if (args) {
        if (args->input_file_A) {fclose(args->input_file_A); args->input_file_A = NULL;}
        if (args->input_file_B) {fclose(args->input_file_B); args->input_file_B = NULL;}
        if (args->output_stream && args->output_stream != stdout) {fclose(args->output_stream); args->output_stream = NULL;}
        free(args);
    }
}

void open_or_exit(FILE **file, const char *filename, const char *mode) { // nouveau
    *file = fopen(filename, mode);
    if (!*file) {
        fprintf(stderr, "Erreur d'ouverture du fichier %s : %s\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void usage(char *prog_name) { // stdout car pas réellement une erreur qui empêche le code de tourner
    fprintf(stdout, "UTILISATION :\n");
    fprintf(stdout, "   %s [OPTIONS] name_op input_file_A [input_file_B]\n",
            prog_name);
    fprintf(stdout, "   name_op : nom de l'opération à effectuer.\n");
    fprintf(stdout,
            "   input_file_A : chemin relatif ou absolu vers le fichier "
            "contenant le "
            "premier vecteur/matrice de l'opération, au format binaire.\n");
    fprintf(stdout,
            "   input_file_B : optionnel ; chemin relatif ou absolu vers "
            "le fichier contenant le second vecteur/matrice de "
            "l'opération s'il y en a deux, au format binaire.\n");
    fprintf(stdout, "   -v : autorise les messages de debug sur la sortie "
                    "d'erreur standard. Défaut : false.\n");
    fprintf(stdout,
            "   -n n_threads : spécifie le nombre de threads qui peuvent "
            "être utilisés. Défaut : 1.\n");
    fprintf(stdout,
            "   -f output_stream : chemin relatif ou absolu vers le "
            "fichier qui contiendra le vecteur ou la matrice résultante "
            "au format binaire. Défaut : stdout.\n");
    fprintf(stdout, "   -d degree (juste pour lstsq) : degré du polynôme "
                    "d'interpolation (uint8_t). Défaut : 1.\n");
}

int parse_args(args_t *args, int argc, char **argv) {
    // Valeurs par défaut
    args->verbose = false;
    args->nb_threads = 1;
    args->output_stream = stdout;
    args->deg = 1;
    args->op = NULL;
    args->input_file_A = NULL;
    args->input_file_B = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "hn:vf:d:")) != -1) {
        switch (opt) {
        case 'n':
            args->nb_threads = atoi(optarg);
            if (args->nb_threads == 0) {
                fprintf(stderr,
                        "Le nombre de threads doit être supérieur à 0 !\n");
                free_all(args); // modifié
                exit(EXIT_FAILURE);
            }
            break;
        case 'v':
            args->verbose = true;
            break;
        case 'f':
            open_or_exit(&args->output_stream, optarg, "w+"); // nouveau
            break;
        case 'd':
            args->deg = atoi(optarg);
            if (args->deg < 0) {
                fprintf(stderr,
                        "Le degré de l'approximation doit être positif !\n");
                free_all(args); // modifié
                exit(EXIT_FAILURE);
            }
            break;
        case 'h':
            usage(argv[0]);
            free_all(args); // modifié
            exit(EXIT_SUCCESS);
        default:
            usage(argv[0]);
            free_all(args); // modifié
            exit(EXIT_FAILURE);
        }
    }

    if (argc == optind || argc == optind + 1) {
        fprintf(stderr, "Vous devez fournir une opération et les fichiers "
                        "d'instances correspondant.\n");
        free_all(args); // modifié
        exit(EXIT_FAILURE);
    }

    args->op = argv[optind++];
    open_or_exit(&args->input_file_A, argv[optind++], "rb"); // nouveau
    // if (NULL == (args->input_file_A = fopen(argv[optind++], "rb"))) {
    //     fprintf(stderr,
    //             "Impossible d'ouvrir le premier fichier d'entrée %s : %s\n",
    //             argv[optind - 1], strerror(errno));
    //     free(args); // modifié
    //     exit(EXIT_FAILURE);
    // }

    if (strcmp(args->op, "add_v_v") == 0 || strcmp(args->op, "sub_v_v") == 0 ||
        strcmp(args->op, "dot_prod") == 0 || strcmp(args->op, "add_m_m") == 0 ||
        strcmp(args->op, "sub_m_m") == 0 || strcmp(args->op, "mult_m_v") == 0 ||
        strcmp(args->op, "mult_m_m") == 0 ||
        strcmp(args->op, "back_sub") == 0 || strcmp(args->op, "lstsq") == 0) {
        if (optind == argc) {
            fprintf(stderr,
                    "Vous devez fournir un second fichier d'instance pour cette opération.\n");
            free_all(args); // modifié
            exit(EXIT_FAILURE);
        }
        // if (NULL == (args->input_file_B = fopen(argv[optind], "rb"))) {
        //     fprintf(stderr,
        //             "Impossible d'ouvrir le second fichier d'entrée %s : %s\n",
        //             argv[optind], strerror(errno));
        //     free_all(args); // modifié
        //     exit(EXIT_FAILURE);
        // }
        open_or_exit(&args->input_file_B, argv[optind], "rb"); // nouveau
    }

    return 0;
}


/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* %%%%%%%%%%%%%%%%%%%%%%      FONCTIONS STANDARD DE GESTION D'OPÉRATIONS      %%%%%%%%%%%%%%%%%%%%%%%%%% */
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */


void handle_vector_ops(args_t *args) {
    vector *x = read_vector(args->input_file_A);
    if (x == NULL) {
        fprintf(stderr, "Erreur de lecture du 1er fichier d'entrée : %s\n", strerror(errno));
        free_all(args);
        exit(EXIT_FAILURE);
    }
    if (args->verbose) { printf("vector x :\n"); print_vector(x); printf("\n"); }

    if (strcmp(args->op, "norm") == 0) {
        double res = norm(x, args->nb_threads);
        // if (isinf(res)) {
        //     free_vector(x);
        //     free_all(args);
        //     exit(EXIT_FAILURE);
        // }
        (args->output_stream == stdout) ? printf("Norme de x : %f\n", res) : write_double(res, args->output_stream);
        free_vector(x);
        return;
    }

    vector *y = read_vector(args->input_file_B);
    if (y == NULL) {
        fprintf(stderr, "Erreur de lecture du 2eme fichier d'entrée : %s\n", strerror(errno));
        free_all(args);
        exit(EXIT_FAILURE);
    }
    if (args->verbose) { printf("vector y :\n"); print_vector(y); printf("\n"); }

    if (strcmp(args->op, "dot_prod") == 0) {
        double res = dot_prod(x, y, args->nb_threads);
        // if (isinf(res)) {
        //     free_vector(x);
        //     free_vector(y);
        //     free_all(args);
        //     exit(EXIT_FAILURE);
        // }
        (args->output_stream == stdout) ? printf("Produit scalaire : %f\n", res) : write_double(res, args->output_stream);
    } else {
        vector *z = init_vector(x->m);
        int process = 0;
        if (strcmp(args->op, "add_v_v") == 0) { process = add_v_v(x, y, z, args->nb_threads); }
        else if (strcmp(args->op, "sub_v_v") == 0) { process = sub_v_v(x, y, z, args->nb_threads); }

        if (process == -1) {
            free_vector(z);
            free_vector(x);
            free_vector(y);
            free_all(args);
            exit(EXIT_FAILURE);
        }

        (args->output_stream == stdout) ? print_vector(z) : write_vector(z, args->output_stream);
        free_vector(z);
    }

    free_vector(x);
    free_vector(y);
}

void handle_matrix_ops(args_t *args) {
    matrix *A = read_matrix(args->input_file_A);
    vector *V = NULL;

    if (A == NULL && strcmp(args->op, "transp") == 0) {
        V = read_vector(args->input_file_A);
        if (V == NULL) {
            fprintf(stderr, "Erreur de lecture du 1er fichier d'entrée : %s\n", strerror(errno));
            free_all(args);
            exit(EXIT_FAILURE);
        } 
        (args->output_stream == stdout) ? print_vector(V) : write_vector(V, args->output_stream);
        free_vector(V); 
        return;

    }

    if (args->verbose && A != NULL) { printf("matrix A :\n"); print_matrix(A); printf("\n"); }
    else if (args->verbose && V != NULL) { printf("vector A :\n"); print_vector(V); printf("\n"); }

    if (strcmp(args->op, "transp") == 0) {
        if (A!= NULL) {
            matrix *C = init_matrix(A->n, A->m);
            if (A->m == 0 && A->n == 0) return; // vérifie que si la taille de la matrice est 0x0, on ne fait rien
            if (transp(A, C, args->nb_threads) == -1) {
                free_matrix(A); free_matrix(C); free_all(args); exit(EXIT_FAILURE);
            }
            (args->output_stream == stdout) ? print_matrix(C) : write_matrix(C, args->output_stream);
            free_matrix(A); free_matrix(C);
            return;
        } else if (V) {
            matrix *C = init_matrix(1, V->m);
            if (transp(V, C, args->nb_threads) == -1) {
                free_vector(V); free_matrix(C); free_all(args); exit(EXIT_FAILURE);
            }
            (args->output_stream == stdout) ? print_vector(V) : write_vector(V, args->output_stream);
            free_vector(V); 
            free_matrix(C);
            return;

        }
        
    }

    if (strcmp(args->op, "qr") == 0) {
        matrix *Q = init_matrix(A->m, A->n);
        matrix *R = init_matrix(A->n, A->n);
        int nb_threads = args->nb_threads;
        if (qr(A, Q, R, nb_threads) == -1) {
            free_matrix(A); free_matrix(Q); free_matrix(R); free_all(args); exit(EXIT_FAILURE);
        }
        (args->output_stream == stdout) ? (print_matrix(Q), print_matrix(R)) : write_QR(Q, R, args->output_stream);
        free_matrix(A); free_matrix(Q); free_matrix(R);
        return;
    }

    if (strcmp(args->op, "mult_m_v") == 0) {
        vector *b = read_vector(args->input_file_B);
        if (b == NULL) {
            fprintf(stderr, "Erreur de lecture du 2eme fichier d'entrée : %s\n", strerror(errno));
            free_all(args);
            exit(EXIT_FAILURE);
        }
        if (args->verbose) { printf("vector b :\n"); print_vector(b); printf("\n"); }
        vector *x = init_vector(A->m);
        if (mult_m_v(A, b, x, args->nb_threads) == -1) {
            free_vector(b); free_vector(x); free_matrix(A); free_all(args); exit(EXIT_FAILURE);
        }
        (args->output_stream == stdout) ? print_vector(x) : write_vector(x, args->output_stream);
        free_vector(b); free_vector(x); free_matrix(A);
        return;
    }

    if (strcmp(args->op, "back_sub") == 0) {
        vector *b = read_vector(args->input_file_B);
        if (b == NULL) {
            fprintf(stderr, "Erreur de lecture du 2eme fichier d'entrée : %s\n", strerror(errno));
            free_all(args);
            exit(EXIT_FAILURE);
        }
        if (args->verbose) { printf("vector b :\n"); print_vector(b); printf("\n"); }
        vector *y = init_vector(A->n);
        if (back_sub(A, b, y) == -1) {
            free_vector(b); free_vector(y); free_matrix(A); free_all(args); exit(EXIT_FAILURE);
        }
        (args->output_stream == stdout) ? print_vector(y) : write_vector(y, args->output_stream);
        free_vector(b); free_vector(y); free_matrix(A);
        return;
    }

    matrix *B = read_matrix(args->input_file_B);
    if (B == NULL) {
        fprintf(stderr, "Erreur de lecture du 2eme fichier d'entrée : %s\n", strerror(errno));
        free_all(args);
        exit(EXIT_FAILURE);
    }
    if (args->verbose) { printf("matrix B :\n"); print_matrix(B); printf("\n"); }

    matrix *C = NULL;
    if (strcmp(args->op, "add_m_m") == 0 || strcmp(args->op, "sub_m_m") == 0 || strcmp(args->op, "mult_m_m") == 0) {
        uint64_t Cn = (strcmp(args->op, "mult_m_m") == 0) ? B->n : A->n;
        C = init_matrix(A->m, Cn);
        int process;
        if (strcmp(args->op, "add_m_m") == 0) { process = add_m_m(A, B, C, args->nb_threads); }
        else if (strcmp(args->op, "sub_m_m") == 0) { process = sub_m_m(A, B, C, args->nb_threads); }
        else { process = mult_m_m(A, B, C, args->nb_threads); }

        if (process == -1) {
            free_matrix(A); free_matrix(B); free_matrix(C); free_all(args); exit(EXIT_FAILURE);
        }

        (args->output_stream == stdout) ? print_matrix(C) : write_matrix(C, args->output_stream);
        free_matrix(C);
    }

    free_matrix(A);
    free_matrix(B);
}

void handle_lstsq(args_t *args) {
    vector *X = read_vector(args->input_file_A);
    if (X == NULL) {
        fprintf(stderr, "Erreur de lecture du 1er fichier d'entrée : %s\n", strerror(errno));
        free_all(args);
        exit(EXIT_FAILURE);
    }
    vector *Y = read_vector(args->input_file_B);
    if (Y == NULL) {
        fprintf(stderr, "Erreur de lecture du 2eme fichier d'entrée : %s\n", strerror(errno));
        free_vector(X);
        free_all(args);
        exit(EXIT_FAILURE);
    }

    if (X->m != Y->m) {
        fprintf(stderr, "Les dimensions de X et Y sont incompatibles (X->m = %lu, Y->m = %lu)\n", X->m, Y->m);
        free_vector(X);
        free_vector(Y);
        free_all(args);
        exit(EXIT_FAILURE);
    }

    uint64_t m = X->m;
    uint8_t n = (args->deg > m - 1) ? m - 1 : args->deg +1;

    matrix *A = init_matrix(m, n);
    for (uint64_t i = 0; i < m; i++)
        for (uint64_t j = 0; j < n; j++)
            A->values[i][j] = pow(X->values[i], j);

    vector *coefficients = init_vector(n);
    int nb_threads = args->nb_threads;
    if (lstsq(A, Y, coefficients, nb_threads) == -1) {
        free_matrix(A);
        free_vector(X);
        free_vector(Y);
        free_vector(coefficients);
        free_all(args);
        exit(EXIT_FAILURE);
    }

    (args->output_stream == stdout) ? print_vector(coefficients) : write_vector(coefficients, args->output_stream);
    free_matrix(A);
    free_vector(X);
    free_vector(Y);
    free_vector(coefficients);
}




/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      MAIN       %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */


int main(int argc, char **argv) {
    args_t *args = malloc(sizeof(args_t));
    if (!args) exit(EXIT_FAILURE);

    parse_args(args, argc, argv);

    if (strcmp(args->op, "add_v_v") == 0 || strcmp(args->op, "sub_v_v") == 0 ||
        strcmp(args->op, "dot_prod") == 0 || strcmp(args->op, "norm") == 0) {
        handle_vector_ops(args);
    } else if (strcmp(args->op, "add_m_m") == 0 || strcmp(args->op, "sub_m_m") == 0 ||
               strcmp(args->op, "mult_m_m") == 0 || strcmp(args->op, "mult_m_v") == 0 ||
               strcmp(args->op, "back_sub") == 0 || strcmp(args->op, "qr") == 0 ||
               strcmp(args->op, "transp") == 0) {
        handle_matrix_ops(args);
    } else if (strcmp(args->op, "lstsq") == 0) {
        handle_lstsq(args);
    } else {
        fprintf(stderr, "Cette opération n'est pas implémentée...\n");
        free_all(args);
        exit(EXIT_FAILURE);
    }

    free_all(args);
    exit(EXIT_SUCCESS);
}