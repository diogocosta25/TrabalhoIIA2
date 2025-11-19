#ifndef UTILS_H
#define UTILS_H

extern double distancias[500][500];

void init_rand();
int random_l_h(int min, int max);
float rand_01();

int carregar_instancia(char *path, int *C, int *m);
void gera_sol_inicial(int *sol, int m, int C);
void escreve_sol(int *sol, int m);

#endif