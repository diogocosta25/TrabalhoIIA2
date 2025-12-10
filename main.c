#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "algoritmo.h"
#include "utils.h"
#include "funcao.h"
#include "algoritmo2.h"
#include "algoritmo3.h"

#define DEFAULT_RUNS 10

int main(int argc, char* argv[]) {
    char nome_fich[100];   
    int runs, alg_choice;
    int *sol, *best;       
    double custo, best_custo; 
    double mbf = 0.0;      
    int C, m; 

    if (argc == 3) {
        strcpy(nome_fich, argv[1]);
        runs = atoi(argv[2]);
        printf("Escolha o algoritmo (1-Trepa Colinas, 2-Evolutivo, 3-Hibrido): ");
        scanf("%d", &alg_choice);
    } else {
        runs = DEFAULT_RUNS;
        printf("Nome do Ficheiro (ex: tourism_5.txt): ");
        scanf("%s", nome_fich); 
        printf("Numero de execucoes (min 10): ");
        scanf("%d", &runs);
        printf("Escolha o algoritmo:\n");
        printf("1 - Trepa-Colinas (Pesquisa Local)\n");
        printf("2 - Algoritmo Evolutivo\n");
        printf("3 - Hibrido\n");
        printf("Opcao: ");
        scanf("%d", &alg_choice);
    }

    
    init_rand(); 
    
    if (carregar_instancia(nome_fich, &C, &m) == 0) {
        fprintf(stderr, "Erro ao carregar o ficheiro.\n");
        return 1;
    }

    sol = malloc(sizeof(int) * m);
    best = malloc(sizeof(int) * m);
    
    if (!sol || !best) {
        fprintf(stderr, "Erro de memoria.\n");
        return 1;
    }

    best_custo = -1.0; 

    for (int k = 0; k < runs; k++) {
        gera_sol_inicial(sol, m, C);

        switch (alg_choice) {
            case 1:
                custo = trepa_colinas(sol, m, C, 500);
                break;
            
            case 2:
                custo = algoritmo_evolutivo(sol, m, C, 50, 100, 0.5, 0.7);
                break;
                
            case 3:
            printf(">>> HIBRIDO 1: Evolutivo + Refinamento Local <<<\n");
            custo = hibrido_refinamento_final(sol, m, C, 50, 100, 1000);
            break;
                
            default:
                printf("Opcao invalida.\n");
                return 1;
        }

        printf("\nRepeticao %d:", k + 1);
        escreve_sol(sol, m);
        printf(" Fitness Final: %.4f\n", custo);

        mbf += custo;

        if (custo > best_custo) {
            best_custo = custo;
            for(int i=0; i<m; i++) best[i] = sol[i];
        }
    }

    printf("\n========================================\n");
    printf("RESULTADOS FINAIS (%d execucoes)\n", runs);
    printf("========================================\n");
    printf("Algoritmo: %d\n", alg_choice);
    printf("Melhor Fitness Encontrado (Best): %.4f\n", best_custo);
    printf("Media dos Fitness (MBF): %.4f\n", mbf / runs);
    printf("Melhor Solucao: ");
    escreve_sol(best, m);
    printf("\n");

    free(sol);
    free(best);

    return 0;
}