# LEPL1503 - Projet Matrix (Groupe P1)

## Présentation

Ce projet implémente un ensemble d'opérations vectorielles et matricielles optimisées avec le multithreading en C, dans le cadre du cours LEPL1503 à l'UCLouvain. \
Le cœur du projet se trouve dans le fichier `main.c`, qui agit comme une interface permettant d'appeler dynamiquement les différentes fonctions via des arguments en ligne de commande. Ce fichier est conçu pour assurer que le programme s'exécute de manière fluide et efficace, sans fuites de mémoires.

## Compilation & exécution

### Exécuter `main`

Le fichier `Makefile` permet de simplifier la compilation de `main`, ainsi que d'effectuer d'autres opérations utiles sur les fichiers de ce projet.
Pour compiler le programme, utilisez une des commandes suivantes dans le terminal :

```sh
# Commande à utiliser dans le terminal :
make 
# Alternative : 
make main
```

L'exécutable `main` est alors généré à la racine du projet. Pour l'utiliser, entrez la commande suivante, qui lance son exécution :

```sh
./main [-v] [-n nb_threads] [-f output_stream] name_op input_file_A [input_file_B]
```

Les arguments entre crochets `[]` sont optionnels :

- `-v` affiche les arguments en début d'exécution.
- `-n nb_threads` parallélise l'exécution de main, via une division du travail (en `nb_treads` threads différents).
- `-f output_stream` enregistre la sortie de l'exécution de main dans un nouveau fichier `.bin`, créé avec le nom `output_stream`.

Plusieurs opérations sont disponibles :

<!-- 
- `add_m_m` : Addition de matrices. Prend 2 arguments `input_file_...` parmis les fichiers créés par `generator_matrix`.
- `add_v_v` : Addition de vecteurs. Prend 2 arguments parmis les fichiers créés par `generator_vector`.
- `sub_m_m` : Soustraction de matrices. Prend 2 arguments "matrix".
- `sub_v_v` : Soustraction de vecteurs. Prend 2 arguments "vector".
- `dot_prod` : Produit scalaire de deux vecteurs. Prend 2 arguments "vector".
- `norm` : Norme d'un vecteur. Prend 1 argument "vector".
- `mult_m_v` : Multiplication matrice-vecteur. Prend en argument 1 fichier "vector" et 1 fichier "matrix".
- `mult_m_m` : Multiplication de matrices. Prend 2 arguments "matrix".
- `transp` : Transposition de matrice. Prend 1 argument "matrix".
- `back_sub` : Substitution arrière, résolution de systèmes d'équations. Prend 1 argument "matrix" et 1 argument "vector".
- `qr` : Décomposition QR d'une matrice A, soit A = QR. Prend 1 argument "matrix". Si 2 fichiers comme `output_stream` sont spécifiés, enregistre les résultats Q et R dans ce fichier.
- `lstsq` : Interpolation polynomiale. Prend 2 arguments "vector" représentant les coordonnées des points à interpoler. Calcule les coefficients du polynôme d'interpolation et les enregistre dans un fichier spécifié comme `output_stream`. -->
<!-- - `is_upper_triang` : Vérifie si la matrice fournie est triangulaire supérieure. Prend 1 argument "matrix". -->

| Opération (`name_op`)    | Description                                        | Fichiers d'entrée (`input_file_...`) |
|--------------------------|----------------------------------------------------|--------------------------------------|
| `add_m_m`                | Addition de deux matrices                          | 2 matrices                           |
| `add_v_v`                | Addition de deux vecteurs                          | 2 vecteurs                           |
| `sub_m_m`                | Soustraction de matrices                           | 2 matrices                           |
| `sub_v_v`                | Soustraction de vecteurs                           | 2 vecteurs                           |
| `dot_prod`               | Produit scalaire de deux vecteurs                  | 2 vecteurs                           |
| `norm`                   | Norme d’un vecteur                                 | 1 vecteur                            |
| `mult_m_v`               | Multiplication matrice-vecteur                     | 1 matrice + 1 vecteur                |
| `mult_m_m`               | Multiplication de matrices                         | 2 matrices                           |
| `transp`                 | Transposition d'une matrice                        | 1 matrice                            |
| `back_sub`               | Substitution arrière (résolution système triang.)  | 1 matrice + 1 vecteur                |
| `qr`                     | Décomposition QR d'une matrice                     | 1 matrice → génère Q et R            |
| `lstsq`                  | Moindres carrés (régression polynomiale)           | 2 vecteurs (X et Y)                  |
<!-- | `is_upper_triang`| Vérifie si une matrice est triangulaire supérieure| 1 matrice                            | -->

### Autres Exécutables & Objets

Pour créer tous les exécutables et objets du programme en une seule commande, utilisez la commande :

```sh
make all
```

Pour créer uniquement une suite de fichiers `.bin` prédéfinis contenant divers vecteurs ou matrices (passables en argument lors de l'exécution de `main`), utilisez :

```sh
# Pour générer des vecteurs : 
make generator_vector
# Pour générer des matrices :
make generator_matrix
```

Pour créer des fichiers `.bin` vecteurs ou matrices sur mesure, l'utilisateur peut exécuter les commandes : 

```sh
# Pour un vecteur nommé "vecteur_DIY.bin", qui ressemble à [1.0 2.22222 42.0] :
make create_vector
./create_vector "vecteur_DIY" "[1.0 2.22222 42.0]"
# Pour une matrice 2x2 nommée "matrice_DIY.bin", qui ressemble à [[19.0 12.0] [8.0 3.0]] : 
make create_matrix
./create_matrix "matrice_DIY" "[[19.0 12.0] [8.0 3.0]]"
```

Pour créer uniquement un exécutable de tests (built-in) `test`, qui peut être lancé grâce à la syntaxe `./`, tapez la commande :

```sh
make test
```

### Fonctionnalités Utiles

Pour nettoyer le projet en enlevant les fichiers objets et les exécutables, utilisez la commande :

```sh
make clean
```

Dans le terminal, pour obtenir des explications sur les différents paramètres lors de l'exécution de `main`, utilisez la commande :

```sh
 ./main -h
```

Pour lancer l'exécutable `test`, utilisez :

```sh
./test
```

## Arborescence Du Programme

```zsh
./
├── headers                         ––> fichiers headers
│   ├── endian.h
│   ├── file.h
│   ├── matrix.h
│   └── vector.h
├── help                            ––> codes pour générer vecteurs \& matrices
│   ├── generator_matrix.c
│   ├── generator_vector.c
|   ├── create_matrix.c
|   ├── create_vector.c
|   └── generator_test_perf.c 
├── objects                         ––> ressources compilées
│   └── ...
├── src                             ––> fichiers sources
│   ├── file.c
│   ├── main.c
│   ├── matrix.c
│   └── vector.c
├── tests                           ––> code des tests
│   └── tests.c
├── Makefile
├── ...                             ––> fichiers binaires vecteurs \& matrices
└── README.md
```

## Comment tester notre code

La fonction de régression polynomiale constitue un bon résumé de notre code. Puisque qu'elle utilise la grande majorité des fonctions que nous avons implémenté, elle ne peut fonctionner que si toutes celles-ci sont justes. Nous avons donc implémenté un moyen simple de tester cette dernière fonction et de comparer notre résultat avec le résultat attendu.

1 : Compiler tous les fichiers :

```sh
make all
```

2 : Lancer le programme python `approximation.py` :

```sh
python3 approximation.py vector_X.bin vector_Y.bin vector_a.bin
```

Les vecteurs `vector_X.bin` et `vector_Y.bin` repérésentent les coordonées des points à interpoller et sont définis dans le fichier `generator_vector.c`.
Le vecteur `vector_a.bin` contient les coefficients de l'interpolation calculés par notre programme.

3 : Compiler et lancer les tests de performance de `lstsq` (avec `nb_threads` le nombre de threads avec lequel on veut effectuer les tests de performance (temporels et de mémoire) de la régression polynomiale) :

```sh
make test_perf
./test_perf nb_thread
```

## Dépendances

Bien que `src/main.c` soit le fichier source central du projet, son exécution dépend des objets créés par la compilation des fichiers additionnels `src/file.c`, `src/matrix.c` et `src/vector.c`. Ils sont respectivement responsables de la gestion des fichiers, des opérations sur les matrices, et des opérations sur les vecteurs. Ainsi, les objets créés : `objects/file.o`, `objects/matrix.o` et `objects/vector.o` assurent le bon fonctionnement et la modularité du programme.

Le programme utilise aussi les bibliothèques externes suivantes :

- `errno.h` : Gestion des codes d'erreur.
- `getopt.h` : Analyse des options de la ligne de commande.
- `stdbool.h` : Types booléens.
- `stdint.h` : Types d'entiers à largeur fixe.
- `stdio.h` : Fonctions d'entrée/sortie standard.
- `stdlib.h` : Fonctions utilitaires générales.
- `string.h` : Fonctions de manipulation de chaînes de caractères.
- `fcntl.h` : Utilisé pour effectuer des opérations sur les descripteurs de fichiers, comme ouvrir ou modifier des fichiers.
- `sys/mman.h` : Sert à gérer la mémoire, notamment pour mapper des fichiers directement en mémoire.
- `sys/stat.h` : Permet d'obtenir des infos sur les fichiers et de modifier leurs attributs.
- `unistd.h` : Contient des constantes et fonctions pour interagir avec le système POSIX (norme pour maintenir la compatibilité entre systèmes d'exploitation).
- `arpa/inet.h` : Fournit des outils pour manipuler les adresses IP et les connexions réseau.
- `math.h` : Permet d'utiliser des fonctions mathématiques standard (trigonométriques, exponentielles, logarithmiques).
- `float.h` : Types floats.
- `pthread.h` : Permet la gestion des threads POSIX utilisés dans le mutlithreading du projet.
- `time.h` : Permet de calculer le temps d'exécution de parties de programmes.

Il utilise également les fichiers headers présents dans le fichier du projet :

- `../headers/file.h` : Fonctions de gestion des fichiers.
- `../headers/matrix.h` : Fonctions de manipulation des matrices.
- `../headers/vector.h` : Fonctions de manipulation des vecteurs.
- `../headers/endian.h` : Fonctions pour gérer les formats d'octets, ajoutées pour garantir que le projet fonctionne correctement sur macOS.

## Bonus

Sur une branche annexe (atteignable en copiant ce tag git@forge.uclouvain.be:lepl15031/students/project-matrix/groupe-p/p1/projet-matrix.git) vous pourrez executer une application interractive en suivant les instructions du nouveau README présent.

## Auteurs

- [Aurélien Tilot](aurelien.tilot@student.uclouvain.be)
- [Diego Seisdedos Stoz](diego.seisdedos@student.uclouvain.be)
- [Eric Van Eetvelde](eric.vaneetvelde@student.uclouvain.be)
- [Lina El Ouarzazi](lina.elouarzazi@student.uclouvain.be)
- [Lisa Dony](lisa.dony@student.uclouvain.be)
- [Thibault Vyt](thibault.vyt@student.uclouvain.be)
