#include <stdio.h>
#include <stdlib.h>
#include "algoritmo.h"
#include "funcao.h"
#include "utils.h"

// Gera um vizinho com troca simples (1 troca)
void gera_vizinho(int *sol, int *vizinho, int m, int C) {
    for (int i = 0; i < m; i++) vizinho[i] = sol[i];

    int indice_sair = random_l_h(0, m - 1);
    int id_entrar;
    int valido = 0;

    while (!valido) {
        id_entrar = random_l_h(0, C - 1);
        valido = 1;
        for (int i = 0; i < m; i++) {
            if (vizinho[i] == id_entrar) {
                valido = 0;
                break;
            }
        }
    }

    vizinho[indice_sair] = id_entrar;
}

// Gera um vizinho com troca dupla (2 trocas em posições DIFERENTES)
void gera_vizinho2(int *sol, int *vizinho, int m, int C) {
    for (int i = 0; i < m; i++) vizinho[i] = sol[i];

    // Garante duas posições distintas
    int pos1 = random_l_h(0, m - 1);
    int pos2 = random_l_h(0, m - 1);

    while (pos2 == pos1) {
        pos2 = random_l_h(0, m - 1);
    }

    int posicoes[2] = {pos1, pos2};

    for (int k = 0; k < 2; k++) {
        int idx = posicoes[k];
        int id_entrar;
        int valido = 0;

        while (!valido) {
            id_entrar = random_l_h(0, C - 1);
            valido = 1;

            for (int i = 0; i < m; i++) {
                // Garante que o novo valor não existe, exceto na posição que estamos a mudar
                if (i != idx && vizinho[i] == id_entrar) {
                    valido = 0;
                    break;
                }
            }
        }
        vizinho[idx] = id_entrar;
    }
}

double trepa_colinas(int *sol, int m, int C, int num_iter, int *vizinho) {
    if (!vizinho) {
        fprintf(stderr, "Erro de memoria.\n");
        return -1.0;
    }

    double fitness_atual = avaliar_solucao(sol, m);
    double fitness_vizinho;

    for (int i = 0; i < num_iter; i++) {

        // Troca simples
        // gera_vizinho(sol, vizinho, m, C);

        // Troca dupla
        gera_vizinho2(sol, vizinho, m, C);

        // ----------------------------------

        fitness_vizinho = avaliar_solucao(vizinho, m);


        // Só melhor
        /*
        if (fitness_vizinho > fitness_atual) {
            fitness_atual = fitness_vizinho;
            for (int k = 0; k < m; k++) sol[k] = vizinho[k];
        }
        */

        // Melhor ou Igual
        if (fitness_vizinho >= fitness_atual) {
            fitness_atual = fitness_vizinho;
            for (int k = 0; k < m; k++) sol[k] = vizinho[k];
        }
    }

    return fitness_atual;
}