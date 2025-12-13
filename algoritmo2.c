#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "algoritmo2.h"
#include "utils.h"
#include "funcao.h"

int existe(int *genes, int tam, int valor) {
    for (int i = 0; i < tam; i++) {
        if (genes[i] == valor) return 1;
    }
    return 0;
}

void reparar_individuo(Individuo *ind, int m, int C) {
    int *contagem = calloc(C, sizeof(int));

    for (int i = 0; i < m; i++) {
        if (ind->genes[i] >= 0 && ind->genes[i] < C)
            contagem[ind->genes[i]]++;
    }
    for (int i = 0; i < m; i++) {
        int gene_atual = ind->genes[i];
        if (gene_atual < 0 || gene_atual >= C || contagem[gene_atual] > 1) {
            if (gene_atual >= 0 && gene_atual < C) contagem[gene_atual]--;

            int novo_gene = -1;
            int tentativas = 0;
            while (tentativas < 50) {
                int r = random_l_h(0, C - 1);
                if (contagem[r] == 0 && !existe(ind->genes, m, r)) {
                    novo_gene = r;
                    break;
                }
                tentativas++;
            }

            if (novo_gene == -1) {
                for (int k = 0; k < C; k++) {
                    if (contagem[k] == 0 && !existe(ind->genes, m, k)) {
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

//--------------------Selecao-----------------------
void torneio(Individuo *pop, int pop_size, Individuo *pai) {
    int i1 = random_l_h(0, pop_size - 1);
    int i2 = random_l_h(0, pop_size - 1);

    if (pop[i1].fitness > pop[i2].fitness) {
        *pai = pop[i1];
    } else {
        *pai = pop[i2];
    }
}


void roleta(Individuo *pop, int pop_size, Individuo *pai) {
    double soma_fitness = 0.0;
    for (int i = 0; i < pop_size; i++) {
        soma_fitness += pop[i].fitness;
    }


    double ponto_sorteado = rand_01() * soma_fitness;

    double soma_parcial = 0.0;
    for (int i = 0; i < pop_size; i++) {
        soma_parcial += pop[i].fitness;

        if (soma_parcial >= ponto_sorteado) {
            *pai = pop[i]; // Selecionado!
            return;
        }
    }

    *pai = pop[pop_size - 1];
}

//-----------Recombinaçao----------------------------
void crossover_uniponto(Individuo *p1, Individuo *p2, Individuo *f1, Individuo *f2, int m, int C) {
    int ponto = random_l_h(1, m - 2);

    for (int i = 0; i < m; i++) {
        if (i < ponto) {
            f1->genes[i] = p1->genes[i];
            f2->genes[i] = p2->genes[i];
        } else {
            f1->genes[i] = p2->genes[i];
            f2->genes[i] = p1->genes[i];
        }
    }
    reparar_individuo(f1, m, C);
    reparar_individuo(f2, m, C);
}

void crossover_uniforme(Individuo *p1, Individuo *p2, Individuo *f1, Individuo *f2, int m, int C) {
    for (int i = 0; i < m; i++) {
        if (rand_01() < 0.5) {
            f1->genes[i] = p1->genes[i];
            f2->genes[i] = p2->genes[i];
        } else {
            f1->genes[i] = p2->genes[i];
            f2->genes[i] = p1->genes[i];
        }
    }


    reparar_individuo(f1, m, C);
    reparar_individuo(f2, m, C);
}

//----------Mutaçai-------------------

void mutacao_substituicao(Individuo *ind, int m, int C) {
    int ponto_a_remover = random_l_h(0, m - 1);
    int novo_valor;

    do {
        novo_valor = random_l_h(0, C - 1);
    } while (existe(ind->genes, m, novo_valor));

    ind->genes[ponto_a_remover] = novo_valor;
}

void mutacao_multipla(Individuo *ind, int m, int C) {
    int num_alteracoes = 2;

    for (int k = 0; k < num_alteracoes; k++) {
        int ponto_a_remover = random_l_h(0, m - 1);
        int novo_valor;
        int tentativas = 0;

        do {
            novo_valor = random_l_h(0, C - 1);
            tentativas++;
            if (tentativas > 100) break;
        } while (existe(ind->genes, m, novo_valor));

        if (!existe(ind->genes, m, novo_valor)) {
            ind->genes[ponto_a_remover] = novo_valor;
        }
    }
}

double algoritmo_evolutivo(int *sol, int m, int C, int pop_size, int num_geracoes, float prob_recombinacao,
                           float prob_mutacao) {
    Individuo *populacao = malloc(sizeof(Individuo) * pop_size);
    Individuo *nova_geracao = malloc(sizeof(Individuo) * pop_size);

    for (int i = 0; i < pop_size; i++) {
        populacao[i].genes = malloc(sizeof(int) * m);
        nova_geracao[i].genes = malloc(sizeof(int) * m);
        gera_sol_inicial(populacao[i].genes, m, C);
        populacao[i].fitness = avaliar_solucao(populacao[i].genes, m);
    }

    for (int g = 0; g < num_geracoes; g++) {
        int melhor_desta_geracao = 0;
        for (int i = 1; i < pop_size; i++) {
            if (populacao[i].fitness > populacao[melhor_desta_geracao].fitness)
                melhor_desta_geracao = i;
        }
        for (int k = 0; k < m; k++) nova_geracao[0].genes[k] = populacao[melhor_desta_geracao].genes[k];
        nova_geracao[0].fitness = populacao[melhor_desta_geracao].fitness;

        for (int i = 1; i < pop_size; i += 2) {
            Individuo p1, p2, f1, f2;
            f1.genes = malloc(sizeof(int) * m);
            f2.genes = malloc(sizeof(int) * m);



            //  Torneio
            torneio(populacao, pop_size, &p1);
            torneio(populacao, pop_size, &p2);

            //  Roleta
            /*
            roleta(populacao, pop_size, &p1);
            roleta(populacao, pop_size, &p2);
            */


            // CROSSOVER

            if (rand_01() < prob_recombinacao) {
                //  Uniponto
                /*
                crossover_uniponto(&p1, &p2, &f1, &f2, m, C);
                */

                //  Uniforme
                crossover_uniforme(&p1, &p2, &f1, &f2, m, C);
            } else {
                for (int k = 0; k < m; k++) {
                    f1.genes[k] = p1.genes[k];
                    f2.genes[k] = p2.genes[k];
                }
            }

            // MUTAÇÃO

            if (rand_01() < prob_mutacao) {
                //  Substituição Simples (1 gene)
                mutacao_substituicao(&f1, m, C);

                //  Múltipla (2 genes)
                /*
                mutacao_multipla(&f1, m, C);
                */
            }
            if (rand_01() < prob_mutacao) {
                // Substituição Simples (1 gene)
                mutacao_substituicao(&f2, m, C);

                // Múltipla (2 genes)
                /*
                mutacao_multipla(&f2, m, C);
                */
            }

            // ==========================================

            f1.fitness = avaliar_solucao(f1.genes, m);
            f2.fitness = avaliar_solucao(f2.genes, m);

            for (int k = 0; k < m; k++) nova_geracao[i].genes[k] = f1.genes[k];
            nova_geracao[i].fitness = f1.fitness;

            if (i + 1 < pop_size) {
                for (int k = 0; k < m; k++) nova_geracao[i + 1].genes[k] = f2.genes[k];
                nova_geracao[i + 1].fitness = f2.fitness;
            }
            free(f1.genes);
            free(f2.genes);
        }

        Individuo *temp = populacao;
        populacao = nova_geracao;
        nova_geracao = temp;
    }

    int melhor_index_global = 0;
    for (int i = 1; i < pop_size; i++) {
        if (populacao[i].fitness > populacao[melhor_index_global].fitness)
            melhor_index_global = i;
    }
    for (int k = 0; k < m; k++) sol[k] = populacao[melhor_index_global].genes[k];
    double fit_final = populacao[melhor_index_global].fitness;

    for (int i = 0; i < pop_size; i++) {
        free(populacao[i].genes);
        free(nova_geracao[i].genes);
    }
    free(populacao);
    free(nova_geracao);

    return fit_final;
}
