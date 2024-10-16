/*Ler uma matriz 20 x 20 e um inteiro X. Fazer uma busca de X na matriz e, ao final,
escrever a localização de todas as suas ocorrências (linha e coluna) ou a mensagem
“não encontrado”.
*/

#include <stdio.h>

int main() {
   int mat[20][20];
   int i, j;
   int x;

   printf("Escreva a matriz 20x20: \n");
   for(i = 0; i < 20; i++){
       for(j = 0; j < 20; j++) {
           scanf("%d", &mat[i][j]);
       }
   }
    for(i = 0; i < 20; i++) {
        for(j = 0; j < 20; j++) {
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
    scanf("%d", &x);
    for(i = 0; i < 20; i++) {
        for(j = 0; j < 20; j++) {
            if(mat[i][j] == x) {
                printf("x na posicao %d,%d\n", i, j);
            }
        }
    }
    return 0;
}
