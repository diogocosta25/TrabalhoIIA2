#include <stdio.h>
#include <stdlib.h>
#include "algoritmo3.h"
#include "algoritmo2.h"
#include "algoritmo.h"
#include "utils.h"
#include "funcao.h"

// =============================================================================
// FUNÇÕES AUXILIARES LOCAIS (STATIC)
// =============================================================================

// Verifica se um valor existe no gene (para evitar duplicados)
static int existe_local(int *genes, int tam, int valor) {
    for (int i = 0; i < tam; i++) {
        if (genes[i] == valor) return 1;
    }
    return 0;
}

// Repara soluções inválidas após crossover
static void reparar_local(Individuo *ind, int m, int C) {
    int *contagem = calloc(C, sizeof(int));

    // Contar ocorrências
    for(int i=0; i<m; i++) {
        if(ind->genes[i] >= 0 && ind->genes[i] < C)
            contagem[ind->genes[i]]++;
    }

    // Substituir inválidos ou duplicados
    for(int i=0; i<m; i++) {
        int gene_atual = ind->genes[i];
        if(gene_atual < 0 || gene_atual >= C || contagem[gene_atual] > 1) {

            if(gene_atual >= 0 && gene_atual < C) contagem[gene_atual]--;

            int novo_gene = -1;
            int tentativas = 0;

            // Tenta encontrar aleatoriamente
            while(tentativas < 50) {
                int r = random_l_h(0, C-1);
                if(contagem[r] == 0 && !existe_local(ind->genes, m, r)) {
                    novo_gene = r;
                    break;
                }
                tentativas++;
            }

            // Se falhar aleatoriamente, procura sequencialmente
            if(novo_gene == -1) {
                for(int k=0; k<C; k++) {
                    if(contagem[k] == 0 && !existe_local(ind->genes, m, k)) {
                        novo_gene = k;
                        break;
                    }
                }
            }

            ind->genes[i] = novo_gene;
            contagem[novo_gene]++;
        }
    }
    free(contagem);
}

// Seleção por Torneio
static void torneio_local(Individuo *pop, int pop_size, Individuo *pai) {
    int i1 = random_l_h(0, pop_size - 1);
    int i2 = random_l_h(0, pop_size - 1);

    if(pop[i1].fitness > pop[i2].fitness) {
        *pai = pop[i1];
    } else {
        *pai = pop[i2];
    }
}

// Crossover de Um Ponto
static void crossover_local(Individuo *p1, Individuo *p2, Individuo *f1, Individuo *f2, int m, int C) {
    int ponto = random_l_h(1, m - 2);

    for(int i=0; i<m; i++) {
        if(i < ponto) {
            f1->genes[i] = p1->genes[i];
            f2->genes[i] = p2->genes[i];
        } else {
            f1->genes[i] = p2->genes[i];
            f2->genes[i] = p1->genes[i];
        }
    }
    reparar_local(f1, m, C);
    reparar_local(f2, m, C);
}

// Mutação por Substituição (CORRIGIDA)
// Agora recebe 'C' para poder escolher um gene novo válido
static void mutacao_local(Individuo *ind, int m, int C) {
    if (rand_01() < 0.05) { // Probabilidade interna (pode ser ajustada)
        int ponto_a_remover = random_l_h(0, m - 1);
        int novo_valor;

        // Procura um valor que NÃO esteja na solução atual
        int tentativas = 0;
        do {
            novo_valor = random_l_h(0, C - 1);
            tentativas++;
            if (tentativas > 100) break; // Segurança
        } while (existe_local(ind->genes, m, novo_valor));

        if (!existe_local(ind->genes, m, novo_valor)) {
            ind->genes[ponto_a_remover] = novo_valor;
        }
    }
}

// =============================================================================
// HÍBRIDO 1: REFINAMENTO FINAL
// =============================================================================
double hibrido_refinamento_final(int *sol, int m, int C, int pop_size, int num_geracoes, int num_iter_tc) {

    // 1. Evolutivo
    // Assume-se que o teu algoritmo_evolutivo original já está otimizado.
    // Se não estiver, usa a lógica do memético abaixo mas com abordagem=0
    algoritmo_evolutivo(sol, m, C, pop_size, num_geracoes, 0.7, 0.05);

    // 2. Pesquisa Local
    // Criamos o auxiliar aqui para passar ao trepa_colinas
    int *aux = malloc(sizeof(int) * m);
    if (!aux) return -1.0;

    double fitness_final = trepa_colinas(sol, m, C, num_iter_tc, aux);

    free(aux);
    return fitness_final;
}

// =============================================================================
// HÍBRIDO 2: MEMÉTICO (OTIMIZADO)
// =============================================================================
double hibrido_memetico(int *sol, int m, int C, int pop_size, int num_geracoes) {

    // --- OTIMIZAÇÃO 1: Alocar auxiliar para Trepa Colinas ---
    int *vizinho_aux = malloc(sizeof(int) * m);

    Individuo *populacao = malloc(sizeof(Individuo) * pop_size);
    Individuo *nova_geracao = malloc(sizeof(Individuo) * pop_size);

    // --- OTIMIZAÇÃO 2: Pré-alocar filhos fora do ciclo ---
    Individuo p1, p2, f1, f2;
    f1.genes = malloc(sizeof(int) * m);
    f2.genes = malloc(sizeof(int) * m);

    // Inicialização
    for(int i=0; i<pop_size; i++) {
        populacao[i].genes = malloc(sizeof(int) * m);
        nova_geracao[i].genes = malloc(sizeof(int) * m);
        gera_sol_inicial(populacao[i].genes, m, C);
        populacao[i].fitness = avaliar_solucao(populacao[i].genes, m);
    }

    // Ciclo Principal
    for (int g = 0; g < num_geracoes; g++) {

        // --- COMPONENTE MEMÉTICA ---
        // Aplica Trepa Colinas a alguns indivíduos de 10 em 10 gerações
        if (g % 10 == 0) {
            for(int i=0; i < pop_size; i++) {
                if(rand_01() < 0.1) { // 10% da população
                    // Passamos 'vizinho_aux' para ser rápido
                    populacao[i].fitness = trepa_colinas(populacao[i].genes, m, C, 50, vizinho_aux);
                }
            }
        }

        // Elitismo
        int melhor_desta_geracao = 0;
        for(int i=1; i<pop_size; i++) {
            if(populacao[i].fitness > populacao[melhor_desta_geracao].fitness)
                melhor_desta_geracao = i;
        }

        for(int k=0; k<m; k++) nova_geracao[0].genes[k] = populacao[melhor_desta_geracao].genes[k];
        nova_geracao[0].fitness = populacao[melhor_desta_geracao].fitness;

        // Loop Evolutivo
        for (int i = 1; i < pop_size; i += 2) {
            // JÁ NÃO FAZEMOS MALLOC AQUI! Usamos f1 e f2 que criámos lá em cima.

            torneio_local(populacao, pop_size, &p1);
            torneio_local(populacao, pop_size, &p2);

            if(rand_01() < 0.7) {
                crossover_local(&p1, &p2, &f1, &f2, m, C);
            } else {
                for(int k=0;k<m;k++) { f1.genes[k] = p1.genes[k]; f2.genes[k] = p2.genes[k]; }
            }

            if(rand_01() < 0.05) mutacao_local(&f1, m, C);
            if(rand_01() < 0.05) mutacao_local(&f2, m, C);

            f1.fitness = avaliar_solucao(f1.genes, m);
            f2.fitness = avaliar_solucao(f2.genes, m);

            // Copia para a nova geração
            for(int k=0; k<m; k++) nova_geracao[i].genes[k] = f1.genes[k];
            nova_geracao[i].fitness = f1.fitness;

            if(i+1 < pop_size) {
                for(int k=0; k<m; k++) nova_geracao[i+1].genes[k] = f2.genes[k];
                nova_geracao[i+1].fitness = f2.fitness;
            }

            // JÁ NÃO FAZEMOS FREE AQUI!
        }

        Individuo *temp = populacao;
        populacao = nova_geracao;
        nova_geracao = temp;
    }

    // Melhor Global
    int melhor_index = 0;
    for(int i=1; i<pop_size; i++) {
        if(populacao[i].fitness > populacao[melhor_index].fitness)
            melhor_index = i;
    }
    for(int k=0; k<m; k++) sol[k] = populacao[melhor_index].genes[k];
    double fit_final = populacao[melhor_index].fitness;

    // --- LIMPEZA FINAL ---
    free(vizinho_aux); // Limpa o auxiliar do trepa colinas
    free(f1.genes);    // Limpa os auxiliares dos filhos
    free(f2.genes);

    for(int i=0; i<pop_size; i++) {
        free(populacao[i].genes);
        free(nova_geracao[i].genes);
    }
    free(populacao);
    free(nova_geracao);

    return fit_final;
}