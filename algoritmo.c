#include <stdio.h>
#include <stdlib.h>
#include "algoritmo.h"
#include "funcao.h"
#include "utils.h"

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

void gera_vizinho2(int *sol, int *vizinho, int m, int C) {
    for (int i = 0; i < m; i++) vizinho[i] = sol[i];

    for (int k = 0; k < 2; k++) {
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
}

double trepa_colinas(int *sol, int m, int C, int num_iter) {
    int *vizinho = malloc(sizeof(int) * m);
    if (!vizinho) {
        fprintf(stderr, "Erro de memoria.\n");
        return -1.0;
    }


    double fitness_atual = avaliar_solucao(sol, m);
    double fitness_vizinho;

    for (int i = 0; i < num_iter; i++) {
        // VIZINHANÇA 1
        //gera_vizinho(sol, vizinho, m, C);
        //------------------------------------------------

        // VIZINHANÇA 2
        gera_vizinho2(sol, vizinho, m, C);
        //------------------------------------------------

        fitness_vizinho = avaliar_solucao(vizinho, m);


        // CUSTO IGUAL NÃO É ACEITE
        /*
        if(fitness_vizinho > fitness_atual || rand_01() < 0.01) {
            fitness_atual = fitness_vizinho;
            for(int k = 0; k < m; k++) sol[k] = vizinho[k];
        }
        */
        //------------------------------------------------
        // CUSTO IGUAL É ACEITE (Maior ou Igual >=)
        if (fitness_vizinho >= fitness_atual || rand_01() < 0.01) {
            fitness_atual = fitness_vizinho;
            for (int k = 0; k < m; k++) sol[k] = vizinho[k];
        }
    }

    free(vizinho);
    return fitness_atual;
}
