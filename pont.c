#include <stdio.h>
int main() {
    int x=1,y=4 ;
    int *px = &y, *py = &y;
    *px = *px + *py; /* ao fazer isso o *px que esta apontando para o endereco de y eh modificado, modificando y */
    printf("%d, %d\n", x, y); /*o conteudo de x e o conteudo de y (modificado) */
    printf("%d, %d\n", *px, *py); /*espaco alocado que corresponde ao conteudo de y (modificaoo)*/
    printf("%p, %p\n", px, py); /*endereco de memoria de px e py, mostrando que px e py estao apontados para y*/
    printf("%p, %p\n", &x, &y); /*endereco de memoria de x e y*/
    return 0;
}
