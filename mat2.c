/*Ler uma matriz 6 x 6, contar e escrever quantos valores maiores que 10 ela possui.*/

#include <stdio.h>

int main() {
    int matriz[6][6];
    int i, j;
    int maior_que_10;

    for(i = 0; i < 6; i++) {
        for(j = 0; j < 6; j++) {
            scanf("%d", &matriz[i][j]);
        }
    }
    
    maior_que_10 = 0;
    for(i = 0; i < 6; i++) {
        for(j = 0; j < 6; j++) {
            if(matriz[i][j] > 10) {
                maior_que_10++;
            }
        }
    }
    printf("A matriz possui %d numeros maiores que dez", maior_que_10);

    return 0;
}
