#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"

double distancias[500][500];

int obter_id(char *str) {
    return atoi(&str[1]) - 1;
}

void init_rand() {
    srand((unsigned)time(NULL));
}

int random_l_h(int min, int max) {
    return min + rand() % (max - min + 1);
}

float rand_01() {
    return ((float)rand()) / RAND_MAX;
}

int carregar_instancia(char *path, int *C, int *m){
    FILE *f = fopen(path, "r");
    if(!f) return 0;
    
    if (fscanf(f, "%d %d", C, m) != 2) {
        fclose(f);
        return 0;
    }

    for(int i=0; i<*C; i++)
        for(int j=0; j<*C; j++)
            distancias[i][j] = 0.0;

    char buf1[20], buf2[20];
    double dist;
    while(fscanf(f, "%s %s %lf", buf1, buf2, &dist) == 3) {
        int id1 = obter_id(buf1);
        int id2 = obter_id(buf2);
        if(id1 >= 0 && id1 < *C && id2 >= 0 && id2 < *C) {
            distancias[id1][id2] = dist;
            distancias[id2][id1] = dist;
        }
    }
    fclose(f);
    return 1;
}

void gera_sol_inicial(int *sol, int m, int C) {
    int escolhidos = 0;
    while(escolhidos < m) {
        int candidato = random_l_h(0, C - 1);
        
        int existe = 0;
        for(int i=0; i<escolhidos; i++) {
            if(sol[i] == candidato) { existe = 1; break; }
        }
        
        if(!existe) {
            sol[escolhidos] = candidato;
            escolhidos++;
        }
    }
}

void escreve_sol(int *sol, int m) {
    printf("{");
    for(int i=0; i<m; i++) {
        printf("e%d", sol[i]+1);
        if(i < m-1) printf(", ");
    }
    printf("}");
}