#include "funcao.h"
#include "utils.h" 

double avaliar_solucao(int *sol, int m) {
    double soma = 0.0;
    
    for (int i = 0; i < m - 1; i++) {
        for (int j = i + 1; j < m; j++) {
            soma += distancias[sol[i]][sol[j]];
        }
    }
    return soma / (double)m;
}