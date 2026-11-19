/**
 * \file pergunta.c
 * \brief Implementação das funções da Pergunta e da Lista.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pergunta.h"

#define CAPACIDADE_INICIAL 10

int iniciarLista(Lista *lista){
    lista->quantidade = 0;
    lista->proximoId = 1;
    lista->itens = malloc(CAPACIDADE_INICIAL * sizeof(Pergunta));

    if (lista->itens == NULL){
        lista->capacidade = 0;
        return 0;
    }//if

    lista->capacidade = CAPACIDADE_INICIAL;
    return 1;
}//iniciarLista

void liberarLista(Lista *lista){
    free(lista->itens);
    lista->itens = NULL;
    lista->quantidade = 0;
    lista->capacidade = 0;
}//liberarLista

int inserirPergunta(Lista *lista, Pergunta p){
    Pergunta *novo;

    //se o vetor estiver cheio, dobra o tamanho
    if (lista->quantidade == lista->capacidade){
        novo = realloc(lista->itens, lista->capacidade * 2 * sizeof(Pergunta));
        if (novo == NULL){
            return -1;
        }//if
        lista->itens = novo;
        lista->capacidade = lista->capacidade * 2;
    }//if

    p.id = lista->proximoId;
    lista->proximoId++;
    lista->itens[lista->quantidade] = p;
    lista->quantidade++;
    return p.id;
}//inserirPergunta

void removerPergunta(Lista *lista, int indice){
    int i;
    Pergunta *novo;

    //move as perguntas seguintes uma posição para trás
    for (i = indice; i < lista->quantidade - 1; i++){
        lista->itens[i] = lista->itens[i + 1];
    }//for
    lista->quantidade--;

    //se sobrou pouca coisa, diminui o vetor pela metade
    if (lista->capacidade > CAPACIDADE_INICIAL && lista->quantidade <= lista->capacidade / 4){
        novo = realloc(lista->itens, (lista->capacidade / 2) * sizeof(Pergunta));
        if (novo != NULL){
            lista->itens = novo;
            lista->capacidade = lista->capacidade / 2;
        }//if
    }//if
}//removerPergunta

int buscarPorId(const Lista *lista, int id){
    int i;

    for (i = 0; i < lista->quantidade; i++){
        if (lista->itens[i].id == id){
            return i;
        }//if
    }//for
    return -1;
}//buscarPorId

int contarNivel(const Lista *lista, int nivel){
    int i;
    int total = 0;

    for (i = 0; i < lista->quantidade; i++){
        if (lista->itens[i].nivel == nivel){
            total++;
        }//if
    }//for
    return total;
}//contarNivel

const char *nomeNivel(int nivel){
    switch (nivel){
        case MUITO_FACIL:   return "Muito facil";
        case FACIL:         return "Facil";
        case MEDIO:         return "Medio";
        case DIFICIL:       return "Dificil";
        case MUITO_DIFICIL: return "Muito dificil";
        default:            return "Desconhecido";
    }//switch
}//nomeNivel

void mostrarPergunta(Pergunta p, int mostrarResposta){
    int i;

    printf("[ID %d] Nivel: %s\n", p.id, nomeNivel(p.nivel));
    printf("%s\n", p.enunciado);
    for (i = 0; i < 4; i++){
        printf("  %c) %s", 'A' + i, p.alternativas[i]);
        if (mostrarResposta && p.correta == 'A' + i){
            printf("   <-- correta");
        }//if
        printf("\n");
    }//for
}//mostrarPergunta
