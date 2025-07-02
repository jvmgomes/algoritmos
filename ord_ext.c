//João Vitor Moreira Gomes - RA:176536
//Daniel Monteiro Ribeiro - RA: 176231
//Gabriel Delgado Panovich Barros - RA:176313


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#ifndef LLONG_MAX
#define LLONG_MAX 9223372036854775807LL
#endif

typedef long long int TipoRegistro;

typedef struct {
    TipoRegistro valor;
    int idFita;
} RegistroIntercalacao;


void heapify(TipoRegistro vetor[], int tamanho, int indice) {
    int menor = indice;
    int esquerda = 2 * indice + 1;
    int direita = 2 * indice + 2;

    if (esquerda < tamanho && vetor[esquerda] < vetor[menor]) {
        menor = esquerda;
    }
    if (direita < tamanho && vetor[direita] < vetor[menor]) {
        menor = direita;
    }

    if (menor != indice) {
        TipoRegistro temporario = vetor[indice];
        vetor[indice] = vetor[menor];
        vetor[menor] = temporario;
        heapify(vetor, tamanho, menor);
    }
}

void construirHeap(TipoRegistro vetor[], int tamanho) {
    for (int i = tamanho / 2 - 1; i >= 0; i--) {
        heapify(vetor, tamanho, i);
    }
}

int lerRegistro(FILE *arquivo, TipoRegistro *valor) {
    if (fread(valor, sizeof(TipoRegistro), 1, arquivo) == 1) {
        return 1;
    }
    return 0;
}

void escreverRegistro(FILE *arquivo, TipoRegistro valor) {
    fwrite(&valor, sizeof(TipoRegistro), 1, arquivo);
}

int faseDistribuicao(FILE *arquivoEntrada, FILE *fita1, FILE *fita2, int capacidadeMemoria) {
    TipoRegistro *heapPrincipal = (TipoRegistro*) malloc(capacidadeMemoria * sizeof(TipoRegistro));
    TipoRegistro *registrosCongelados = (TipoRegistro*) malloc(capacidadeMemoria * sizeof(TipoRegistro));
    int tamanhoHeapPrincipal = 0;
    int tamanhoRegistrosCongelados = 0;
    int contadorRuns = 0;
    int indiceFitaAtual = 0;
    FILE *fitasSaida[] = {fita1, fita2};
    int entradaEsgotada = 0;

    for (int i = 0; i < capacidadeMemoria; i++) {
        TipoRegistro valor;
        if (lerRegistro(arquivoEntrada, &valor)) {
            heapPrincipal[tamanhoHeapPrincipal++] = valor;
        } else {
            entradaEsgotada = 1;
            break;
        }
    }

    if (tamanhoHeapPrincipal == 0) {
        free(heapPrincipal);
        free(registrosCongelados);
        return 0;
    }

    while (tamanhoHeapPrincipal > 0 || tamanhoRegistrosCongelados > 0) {
        if (tamanhoHeapPrincipal == 0) {
            memcpy(heapPrincipal, registrosCongelados, tamanhoRegistrosCongelados * sizeof(TipoRegistro));
            tamanhoHeapPrincipal = tamanhoRegistrosCongelados;
            tamanhoRegistrosCongelados = 0;
        }

        contadorRuns++;
        indiceFitaAtual = (indiceFitaAtual + 1) % 2;
        construirHeap(heapPrincipal, tamanhoHeapPrincipal);
        
        TipoRegistro ultimoValorEscrito = -LLONG_MAX;

        while (tamanhoHeapPrincipal > 0) {
            TipoRegistro valorMinimo = heapPrincipal[0];
            escreverRegistro(fitasSaida[indiceFitaAtual], valorMinimo);
            ultimoValorEscrito = valorMinimo;

            TipoRegistro novoRegistro;
            if (!entradaEsgotada && lerRegistro(arquivoEntrada, &novoRegistro)) {
                if (novoRegistro >= ultimoValorEscrito) {
                    heapPrincipal[0] = novoRegistro;
                } else {
                    registrosCongelados[tamanhoRegistrosCongelados++] = novoRegistro;
                    heapPrincipal[0] = heapPrincipal[--tamanhoHeapPrincipal];
                }
            } else {
                entradaEsgotada = 1;
                heapPrincipal[0] = heapPrincipal[--tamanhoHeapPrincipal];
            }
            
            if (tamanhoHeapPrincipal > 0) {
                heapify(heapPrincipal, tamanhoHeapPrincipal, 0);
            }
        }
    }

    free(heapPrincipal);
    free(registrosCongelados);
    return contadorRuns;
}

void passoIntercalacao(FILE* fitaEntrada1, FILE* fitaEntrada2, FILE* fitaSaida) {
    TipoRegistro valor1, valor2;
    TipoRegistro ultimo1 = -LLONG_MAX, ultimo2 = -LLONG_MAX;
    int status1 = lerRegistro(fitaEntrada1, &valor1);
    int status2 = lerRegistro(fitaEntrada2, &valor2);

    while(status1 || status2) {
        int run1Ativo = status1;
        int run2Ativo = status2;

        while(run1Ativo || run2Ativo) {
            int pegarDaFita1 = 0;
            if (run1Ativo && run2Ativo) {
                if (valor1 <= valor2) pegarDaFita1 = 1;
            } else if (run1Ativo) {
                pegarDaFita1 = 1;
            }

            if (pegarDaFita1) {
                escreverRegistro(fitaSaida, valor1);
                ultimo1 = valor1;
                status1 = lerRegistro(fitaEntrada1, &valor1);
                if (!status1 || valor1 < ultimo1) {
                    run1Ativo = 0;
                }
            } else {
                escreverRegistro(fitaSaida, valor2);
                ultimo2 = valor2;
                status2 = lerRegistro(fitaEntrada2, &valor2);
                if (!status2 || valor2 < ultimo2) {
                    run2Ativo = 0;
                }
            }
        }
    }
}

void redistribuirRuns(FILE* fitaEntrada, FILE* fitaSaida1, FILE* fitaSaida2) {
    TipoRegistro valor, ultimoValor = -LLONG_MAX;
    int saidaAtual = 0;
    FILE* fitasSaida[] = {fitaSaida1, fitaSaida2};

    rewind(fitaEntrada);
    
    while(lerRegistro(fitaEntrada, &valor)) {
        if(valor < ultimoValor) {
            saidaAtual = (saidaAtual + 1) % 2;
        }
        escreverRegistro(fitasSaida[saidaAtual], valor);
        ultimoValor = valor;
    }
}

int main() {
    const char *arquivosEntrada[] = {"arq15M.txt", "arq40M.txt"};
    long int capacidadesMemoria[] = {1000000, 10000000};
    const char *nomesFitas[] = {"fita1.bin", "fita2.bin", "fita_merge.bin"};

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            printf("\n------------------------------------------------------\n");
            printf("Arquivo: %s | Memória: %ld registros\n", arquivosEntrada[i], capacidadesMemoria[j]);

            FILE *arquivoEntrada = fopen(arquivosEntrada[i], "rb");
            if (!arquivoEntrada) {
                perror("Erro ao abrir arquivo de entrada");
                continue;
            }

            FILE *fita1 = fopen(nomesFitas[0], "wb+");
            FILE *fita2 = fopen(nomesFitas[1], "wb+");
            if (!fita1 || !fita2) {
                perror("Erro ao criar fitas temporárias");
                return 1;
            }

            clock_t t1 = clock();
            int numRuns = faseDistribuicao(arquivoEntrada, fita1, fita2, capacidadesMemoria[j]);
            clock_t t2 = clock();
            printf("Distribuição: %.2f s (%d runs criados)\n", (double)(t2 - t1) / CLOCKS_PER_SEC, numRuns);

            fclose(arquivoEntrada);
            fclose(fita1);
            fclose(fita2);

            clock_t t3 = clock();
            const char *nomeEntrada1 = nomesFitas[0];
            const char *nomeEntrada2 = nomesFitas[1];
            const char *nomeSaida = nomesFitas[2];
            const char *nomeArquivoFinal = nomeEntrada1;

            if (numRuns > 1) {
                 while (numRuns > 1) {
                    FILE *arqEntrada1 = fopen(nomeEntrada1, "rb");
                    FILE *arqEntrada2 = fopen(nomeEntrada2, "rb");
                    FILE *arqSaida = fopen(nomeSaida, "wb");
                    
                    passoIntercalacao(arqEntrada1, arqEntrada2, arqSaida);

                    fclose(arqEntrada1);
                    fclose(arqEntrada2);
                    fclose(arqSaida);

                    numRuns = (numRuns + 1) / 2;

                    if (numRuns > 1) {
                        FILE *arqParaDistribuir = fopen(nomeSaida, "rb");
                        arqEntrada1 = fopen(nomeEntrada1, "wb");
                        arqEntrada2 = fopen(nomeEntrada2, "wb");

                        redistribuirRuns(arqParaDistribuir, arqEntrada1, arqEntrada2);

                        fclose(arqParaDistribuir);
                        fclose(arqEntrada1);
                        fclose(arqEntrada2);
                    } else {
                        nomeArquivoFinal = nomeSaida;
                    }
                }
            }
           
            clock_t t4 = clock();
            printf("Intercalação: %.2f s\n", (double)(t4 - t3) / CLOCKS_PER_SEC);

            FILE *arquivoSaidaLeitura = fopen(nomeArquivoFinal, "rb");
            if (arquivoSaidaLeitura) {
                fseek(arquivoSaidaLeitura, 0, SEEK_END);
                long tamanhoArquivo = ftell(arquivoSaidaLeitura);
                int tamanhoRegistro = sizeof(TipoRegistro);
                long numRegistros = tamanhoArquivo / tamanhoRegistro;
                int qtdParaImprimir = (numRegistros < 10) ? numRegistros : 10;
                
                if (qtdParaImprimir > 0) {
                    fseek(arquivoSaidaLeitura, -qtdParaImprimir * tamanhoRegistro, SEEK_END);
                    TipoRegistro *ultimosRegistros = (TipoRegistro*) malloc(qtdParaImprimir * sizeof(TipoRegistro));
                    fread(ultimosRegistros, sizeof(TipoRegistro), qtdParaImprimir, arquivoSaidaLeitura);

                    printf("\nÚltimos %d números ordenados:\n", qtdParaImprimir);
                    for (int k = 0; k < qtdParaImprimir; k++) {
                        printf("%lld\n", ultimosRegistros[k]);
                    }
                    free(ultimosRegistros);
                } else {
                    printf("\nArquivo de saída está vazio.\n");
                }
                fclose(arquivoSaidaLeitura);
            }
            
            remove(nomesFitas[0]);
            remove(nomesFitas[1]);
            if (strcmp(nomeArquivoFinal, nomesFitas[2]) == 0) {
                 remove(nomesFitas[2]);
            }
        }
    }
    printf("\n------------------------------------------------------\n");
    return 0;
}
