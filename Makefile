# vérification nombre de processeurs CPU disponibles, pour forcer à tous les utiliser : 
NB_PROC := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu)
MAKEFLAGS += -j$(NB_PROC)

CC=clang --coverage
CFLAGS=-Wall -Werror -O3
LCUNIT=-lcunit
LFlags=-lm -pthread
OBJECTS=objects
HELP=help
SRC=src

# on peut optimiser la vitesse d'exécution du code en ajoutant `-O3` dans `CFLAGS`, cependant il faut le retirer lors du debugging !

main: main.o matrix.o vector.o file.o
	$(CC) -o $@ $(OBJECTS)/main.o $(OBJECTS)/matrix.o $(OBJECTS)/vector.o $(OBJECTS)/file.o $(LFlags)

generator_matrix: matrix.o vector.o file.o
	$(CC) $(CFLAGS) -o $@ $(HELP)/generator_matrix.c $(OBJECTS)/matrix.o $(OBJECTS)/vector.o $(OBJECTS)/file.o $(LFlags)
	./$@

generator_vector: matrix.o vector.o file.o
	$(CC) $(CFLAGS) -o $@ $(HELP)/generator_vector.c $(OBJECTS)/matrix.o $(OBJECTS)/vector.o $(OBJECTS)/file.o $(LFlags)
	./$@

create_vector: matrix.o vector.o file.o
	$(CC) $(CFLAGS) -o $@ $(HELP)/create_vector.c $(OBJECTS)/matrix.o $(OBJECTS)/vector.o $(OBJECTS)/file.o $(LFlags)

create_matrix: matrix.o vector.o file.o
	$(CC) $(CFLAGS) -o $@ $(HELP)/create_matrix.c $(OBJECTS)/matrix.o $(OBJECTS)/vector.o $(OBJECTS)/file.o $(LFlags)

main.o: $(SRC)/main.c headers/vector.h headers/matrix.h headers/file.h | $(OBJECTS)
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $<

vector.o: $(SRC)/vector.c | $(OBJECTS)
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $<

matrix.o: $(SRC)/matrix.c | $(OBJECTS)
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $<

file.o: $(SRC)/file.c | $(OBJECTS)
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $<

test: tests/tests.c vector.o matrix.o file.o
	$(CC) $(CFLAGS) -o $@ tests/tests.c $(OBJECTS)/vector.o $(OBJECTS)/matrix.o $(OBJECTS)/file.o $(LCUNIT) $(LFlags)
	./test 2>/dev/null
# la commande 2>/dev/null permet de supprimer tout les messages venant des différents fprintf(stderr)
# la commande ??? permet de supprimer tout les messages venant des différents fprintf(stderr, stdout)

all: main generator_matrix generator_vector test

$(OBJECTS):
	mkdir -p $@

.PHONY: clean


#tests de performance
test_perf: matrix.o vector.o file.o
	$(CC) $(CFLAGS) -o $@ $(HELP)/generator_test_perf.c $(OBJECTS)/matrix.o $(OBJECTS)/vector.o $(OBJECTS)/file.o $(LFlags)
	
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./$@ 2>&1 | grep "heap"


# added files to the clean target
clean:
	rm -rf $(OBJECTS)
	rm -f main generator_matrix generator_vector test create_vector create_matrix test_perf
	-rm -f *.bin
	-rm -f approximation.pdf
	rm -f main_exec tests_exec
	rm -rf *.dSYM
	-rm -f *.gcda *.gcno *.html *.gcov coverage_report*
	-rm -f bench_and_plot benchmark_all plot_* *.csv

# LEAKS POUR MACOS 
tests_exec:
	$(CC) -g -o tests_exec tests/tests.c src/file.c src/vector.c src/matrix.c -Iheaders $(LCUNIT)

leaks_tests: tests_exec
	@echo "==> Vérification des fuites mémoire avec leaks"
	@leaks --atExit -- ./tests_exec

main_exec:
	$(CC) -g -o main_exec src/main.c src/file.c src/vector.c src/matrix.c -Iheaders

leaks_main: main_exec generator_matrix generator_vector
	@echo "==> Vérification des fuites mémoire sur main_exec"
	@leaks --atExit -- ./main_exec
