/**
 * @file ajuda.c
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Implementação das ajudas da partida.
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "ajuda.h"
#include "tela.h"
#include "util.h"

void iniciarAjudas(Ajudas *ajudas){
    ajudas->pulos = MAX_PULOS;
    ajudas->cartas = 1;
}//iniciarAjudas

int sortearErrada(Pergunta p, const int eliminadas[4]){
    int candidatas[4];
    int quantidade = 0;
    int i;

    for (i = 0; i < 4; i++){
        if ('A' + i != p.correta && !eliminadas[i]){
            candidatas[quantidade] = i;
            quantidade++;
        }//if
    }//for

    if (quantidade == 0){
        return -1;
    }//if
    return candidatas[rand() % quantidade];
}//sortearErrada

int usarCartas(Pergunta p, int eliminadas[4]){
    const char *nomes[4] = {"REI", "AS", "2", "3"};
    int baralho[4] = {0, 1, 2, 3};  //rei, as, dois e tres
    int carta;
    int quantidade;
    int i;
    int sorteio;
    int temp;
    int errada;

    //embaralha as cartas
    for (i = 3; i > 0; i--){
        sorteio = rand() % (i + 1);
        temp = baralho[i];
        baralho[i] = baralho[sorteio];
        baralho[sorteio] = temp;
    }//for

    printf("\n%sAs cartas foram embaralhadas e estao viradas para baixo:%s\n", COR_AMARELO, COR_RESET);
    for (i = 0; i < 4; i++){
        printf("  %s[ %d ]%s", COR_AZUL, i + 1, COR_RESET);
    }//for
    printf("\n");

    carta = baralho[lerInteiro("Escolha uma carta (1 a 4): ", 1, 4) - 1];
    quantidade = carta;
    somAviso();
    printf("Voce tirou a carta %s%s%s! ", COR_AMARELO, nomes[carta], COR_RESET);

    if (quantidade == 0){
        printf("Que azar, nenhuma alternativa sera eliminada.\n");
        return 0;
    }//if

    printf("%d alternativa(s) errada(s) sera(o) eliminada(s).\n", quantidade);
    for (i = 0; i < quantidade; i++){
        errada = sortearErrada(p, eliminadas);
        if (errada >= 0){
            eliminadas[errada] = 1;
        }//if
    }//for
    return quantidade;
}//usarCartas
