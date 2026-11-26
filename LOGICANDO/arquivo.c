/**
 * @file arquivo.c
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Implementação da leitura e gravação de arquivos.
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
#include "arquivo.h"

#define TAM_LINHA 1024
#define LIMITE_PERGUNTAS 100000

/**
 * Verifica se os dados de uma pergunta estão corretos.
 * @param p pergunta verificada
 * @return 1 se está correta, 0 caso contrário
 */
static int perguntaValida(Pergunta p){
    int i;

    if (p.enunciado[0] == '\0'){
        return 0;
    }//if
    for (i = 0; i < 4; i++){
        if (p.alternativas[i][0] == '\0'){
            return 0;
        }//if
    }//for
    if (p.correta < 'A' || p.correta > 'D'){
        return 0;
    }//if
    if (p.nivel < MUITO_FACIL || p.nivel > MUITO_DIFICIL){
        return 0;
    }//if
    return 1;
}//perguntaValida

/**
 * Transforma uma linha do CSV em uma pergunta.
 * A linha é alterada (as vírgulas viram '\0').
 * @param linha linha lida do arquivo
 * @param p endereço da pergunta que será preenchida
 * @return 1 se a linha era válida, 0 caso contrário
 */
static int linhaParaPergunta(char *linha, Pergunta *p){
    char *campos[7];
    int qtd = 1;
    int i;
    char *c;

    campos[0] = linha;
    for (c = linha; *c != '\0'; c++){
        if (*c == ','){
            *c = '\0';
            if (qtd < 7){
                campos[qtd] = c + 1;
            }//if
            qtd++;
        }//if
    }//for

    if (qtd != 7){
        return 0;
    }//if

    //confere o tamanho dos campos
    if (strlen(campos[1]) >= TAM_ENUNCIADO || strlen(campos[6]) != 1){
        return 0;
    }//if
    for (i = 0; i < 4; i++){
        if (strlen(campos[2 + i]) >= TAM_ALTERNATIVA){
            return 0;
        }//if
    }//for

    memset(p, 0, sizeof(Pergunta));
    p->nivel = atoi(campos[0]);
    strcpy(p->enunciado, campos[1]);
    for (i = 0; i < 4; i++){
        strcpy(p->alternativas[i], campos[2 + i]);
    }//for
    p->correta = (char)toupper((unsigned char)campos[6][0]);

    return perguntaValida(*p);
}//linhaParaPergunta

int carregarCSV(Lista *lista, const char *nome, int *ignoradas){
    FILE *arq = fopen(nome, "r");
    char linha[TAM_LINHA];
    Pergunta p;
    int numeroLinha = 0;
    int lidas = 0;
    int c;

    *ignoradas = 0;
    if (arq == NULL){
        return -1;
    }//if

    while (fgets(linha, TAM_LINHA, arq) != NULL){
        numeroLinha++;

        //linha grande demais
        if (strchr(linha, '\n') == NULL && !feof(arq)){
            while ((c = fgetc(arq)) != '\n' && c != EOF){
                //limpa o resto da linha
            }//while
            (*ignoradas)++;
            continue;
        }//if

        linha[strcspn(linha, "\r\n")] = '\0';

        if (linha[0] == '\0'){
            continue; //linha em branco
        }//if

        //pula o cabeçalho do csv
        if (numeroLinha == 1 && !isdigit((unsigned char)linha[0])){
            continue;
        }//if

        if (linhaParaPergunta(linha, &p) && inserirPergunta(lista, p) > 0){
            lidas++;
        } else {
            (*ignoradas)++;
        }//if
    }//while

    fclose(arq);
    return lidas;
}//carregarCSV

int exportarCSV(const Lista *lista, const char *nome){
    FILE *arq = fopen(nome, "w");
    int i;
    int ok;
    Pergunta p;

    if (arq == NULL){
        return 0;
    }//if

    fprintf(arq, "nivel,enunciado,alternativa_a,alternativa_b,alternativa_c,alternativa_d,correta\n");
    for (i = 0; i < lista->quantidade; i++){
        p = lista->itens[i];
        fprintf(arq, "%d,%s,%s,%s,%s,%s,%c\n", p.nivel, p.enunciado,
                p.alternativas[0], p.alternativas[1], p.alternativas[2],
                p.alternativas[3], p.correta);
    }//for

    ok = (ferror(arq) == 0);
    if (fclose(arq) != 0){
        ok = 0;
    }//if
    return ok;
}//exportarCSV

int salvarBinario(const Lista *lista, const char *nome){
    FILE *arq = fopen(nome, "wb");
    int ok;

    if (arq == NULL){
        return 0;
    }//if

    //grava os dados no arquivo
    ok = (fwrite(&lista->quantidade, sizeof(int), 1, arq) == 1);
    if (ok){
        ok = (fwrite(&lista->proximoId, sizeof(int), 1, arq) == 1);
    }//if
    if (ok && lista->quantidade > 0){
        ok = (fwrite(lista->itens, sizeof(Pergunta), lista->quantidade, arq)
              == (size_t)lista->quantidade);
    }//if

    if (fclose(arq) != 0){
        ok = 0;
    }//if
    return ok;
}//salvarBinario

int carregarBinario(Lista *lista, const char *nome){
    FILE *arq = fopen(nome, "rb");
    Pergunta *itens;
    int quantidade;
    int proximoId;
    int capacidade;
    int i;
    int j;

    if (arq == NULL){
        return 0;
    }//if

    if (fread(&quantidade, sizeof(int), 1, arq) != 1 ||
        fread(&proximoId, sizeof(int), 1, arq) != 1 ||
        quantidade < 0 || quantidade > LIMITE_PERGUNTAS || proximoId < 1){
        fclose(arq);
        return 0;
    }//if

    capacidade = quantidade;
    if (capacidade < 10){
        capacidade = 10;
    }//if

    itens = malloc(capacidade * sizeof(Pergunta));
    if (itens == NULL){
        fclose(arq);
        return 0;
    }//if

    if (quantidade > 0 &&
        fread(itens, sizeof(Pergunta), quantidade, arq) != (size_t)quantidade){
        free(itens);
        fclose(arq);
        return 0;
    }//if
    fclose(arq);

    //confere as perguntas lidas
    for (i = 0; i < quantidade; i++){
        itens[i].enunciado[TAM_ENUNCIADO - 1] = '\0';
        for (j = 0; j < 4; j++){
            itens[i].alternativas[j][TAM_ALTERNATIVA - 1] = '\0';
        }//for
        if (!perguntaValida(itens[i]) || itens[i].id < 1 || itens[i].id >= proximoId){
            free(itens);
            return 0;
        }//if
    }//for

    //troca a lista antiga pela nova
    free(lista->itens);
    lista->itens = itens;
    lista->quantidade = quantidade;
    lista->capacidade = capacidade;
    lista->proximoId = proximoId;
    return 1;
}//carregarBinario
