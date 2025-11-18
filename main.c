#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_C 500

double distantcias [MAX_C][MAX_C];

typedef struct{
    int locais[MAX_C];
    int num_locais;
    double fitness;
}Solucao;

int C, m;

int carregar_instancia(const char *path){
    FILE *f = fopen(path, "r");
    if(!f){
        perror("Erro ao abrir o arquivo");
        return 0;
    }
    
    if (fscanf(f, "%d %d", &C, &m) != 2){
        fprintf(stderr, "Formato inválido na priemira linha\n");
        fclose(f);
        return 0;
    }

    if(C<=0 || C > MAX_C){
        fprintf(stderr,"Numero de candidaturas (%d) fora dos limites [1, %d].\n", C, MAX_C);
        fclose(f);
        return 0;
    }

    if(m<=0 || m > C){
        fprintf(stderr, "Numero de locais a construir (%d) invalido (tem de estar entre 1 e %d).\n", m, C);
        fclose(f);
        return 0;
    }

    for (int i = 0; i < C; i++) {
        for (int j = 0; j < C; j++) {
            if(fscanf(f, "%lf", &distancias[i][j]) != 1){
                fprintf(stderr, "Dados insuficientes ou inválidos ao ler distâncias (%d,%d).\n", i, j);
                fclose(f);
                return 0;
            }
        }
    }
    fclose(f);
    return 1;
}

double avaliar_solucao(const Solucao *s){
    if(s->num_locais != m){
        return -INFINITY;
}

    double soma = 0.0;
    int pares = 0;

    for(int i = 0; i < s->num_locais; i++){
        int idx_i = s->locais[i];
        if(idx_i < 0 || idx_i>= C){
            return -INFINITY;
        }
        for(int j = i+1; j < s->num_locais; j++){
            int idx_j = s->locais[j];
            if(idx_j < 0 || idx_j>= C){
                return -INFINITY;
            }
            soma += distancias[idx_i][idx_j];
            pares++;
        }
    }

    if (pares == 0){
        return -INFINITY;
    }
    return (2.0 * soma) / (m *(m - 1));

}

int main(int agrc, char **argv){
    if(argc < 2){
        fprintf(stderr, "Uso: %s <ficheiro_instancia>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (!carregar_instancia(argv[1])){
        return EXIT_FAILURE;
    }

    printf("Instancia carregada com sucesso: C=%d, m=%d\n", C, m)
    return EXIT_SUCCESS;
}

