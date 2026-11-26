/**
 * @file util.c
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Implementação das funções auxiliares de leitura.
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util.h"
#include "tela.h"

int lerTexto(char *destino, int tamanho){
    int c;

    if (fgets(destino, tamanho, stdin) == NULL){
        destino[0] = '\0';
        return 0;
    }//if

    //texto maior que o vetor
    if (strchr(destino, '\n') == NULL && strlen(destino) == (size_t)(tamanho - 1)){
        while ((c = getchar()) != '\n' && c != EOF){
            //limpa o resto da linha
        }//while
        return -1;
    }//if

    destino[strcspn(destino, "\r\n")] = '\0';
    return 1;
}//lerTexto

/**
 * Verifica se um texto está vazio ou só tem espaços.
 * @param texto texto verificado
 * @return 1 se estiver vazio, 0 caso contrário
 */
static int textoVazio(const char *texto){
    int i;

    for (i = 0; texto[i] != '\0'; i++){
        if (!isspace((unsigned char)texto[i])){
            return 0;
        }//if
    }//for
    return 1;
}//textoVazio

int lerTextoObrigatorio(const char *mensagem, char *destino, int tamanho){
    int resultado;

    while (1){
        printf("%s", mensagem);
        fflush(stdout);
        resultado = lerTexto(destino, tamanho);

        if (resultado == 0){
            return 0;
        } else if (resultado == -1){
            mensagemErro("texto muito grande! O maximo e %d caracteres.", tamanho - 1);
        } else if (textoVazio(destino)){
            mensagemErro("o campo nao pode ficar vazio!");
        } else if (strchr(destino, ',') != NULL){
            mensagemErro("nao use virgula no texto (o CSV usa a virgula para separar os campos)!");
        } else {
            return 1;
        }//if
    }//while
}//lerTextoObrigatorio

int lerTextoOpcional(const char *mensagem, char *destino, int tamanho){
    char temp[256];
    int resultado;

    while (1){
        printf("%s", mensagem);
        fflush(stdout);
        resultado = lerTexto(temp, tamanho);

        if (resultado == 0){
            return 0;
        } else if (resultado == -1){
            mensagemErro("texto muito grande! O maximo e %d caracteres.", tamanho - 1);
        } else if (strchr(temp, ',') != NULL){
            mensagemErro("nao use virgula no texto!");
        } else {
            if (!textoVazio(temp)){
                strcpy(destino, temp);
            }//if
            return 1;
        }//if
    }//while
}//lerTextoOpcional

int lerInteiro(const char *mensagem, int minimo, int maximo){
    char linha[20];
    char *fim;
    long numero;
    int resultado;

    while (1){
        printf("%s", mensagem);
        fflush(stdout);
        resultado = lerTexto(linha, sizeof(linha));

        if (resultado == 0){
            return minimo;
        }//if

        numero = strtol(linha, &fim, 10);
        if (resultado == -1 || linha[0] == '\0' || *fim != '\0'){
            mensagemErro("digite apenas numeros!");
        } else if (numero < minimo || numero > maximo){
            mensagemErro("digite um valor entre %d e %d!", minimo, maximo);
        } else {
            return (int)numero;
        }//if
    }//while
}//lerInteiro

char lerLetra(const char *mensagem, const char *validas){
    char linha[10];
    char letra;
    int resultado;

    while (1){
        printf("%s", mensagem);
        fflush(stdout);
        resultado = lerTexto(linha, sizeof(linha));

        if (resultado == 0){
            return validas[0];
        }//if

        letra = (char)toupper((unsigned char)linha[0]);
        if (resultado == 1 && strlen(linha) == 1 && strchr(validas, letra) != NULL){
            return letra;
        }//if
        mensagemErro("opcao invalida! Digite uma das opcoes: %s", validas);
    }//while
}//lerLetra

void pausar(void){
    int c;

    printf("Pressione ENTER para continuar...");
    fflush(stdout);
    while ((c = getchar()) != '\n' && c != EOF){
        //limpa o teclado
    }//while
}//pausar

int contemTexto(const char *texto, const char *busca){
    int i, j;

    for (i = 0; texto[i] != '\0'; i++){
        j = 0;
        while (busca[j] != '\0' && texto[i + j] != '\0' &&
               tolower((unsigned char)texto[i + j]) == tolower((unsigned char)busca[j])){
            j++;
        }//while
        if (busca[j] == '\0'){
            return 1;
        }//if
    }//for
    return 0;
}//contemTexto

int textosIguais(const char *a, const char *b){
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0'){
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])){
            return 0;
        }//if
        i++;
    }//while
    return a[i] == b[i];
}//textosIguais
