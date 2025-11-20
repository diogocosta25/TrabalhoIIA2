#ifndef ALGORITMO2_H
#define ALGORITMO2_H

typedef struct {
    int *genes;
    double fitness;
} Individuo;

double algoritmo_evolutivo(int *sol, int m, int C, int pop_size, int num_geracoes);

#endif