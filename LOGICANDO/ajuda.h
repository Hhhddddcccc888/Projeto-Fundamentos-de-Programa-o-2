/**
 * @file ajuda.h
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Ajudas da partida, baseadas no programa do SBT: pular e cartas.
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef AJUDA_H
#define AJUDA_H

#include "pergunta.h"

#define MAX_PULOS 3

/** Quantas vezes o jogador ainda pode usar cada ajuda */
typedef struct {
    int pulos;   /**< pular a pergunta (até 3 vezes) */
    int cartas;  /**< cartas: elimina de 0 a 3 alternativas erradas */
} Ajudas;//struct

/**
 * Coloca a quantidade inicial de cada ajuda.
 * @param ajudas endereço da estrutura de ajudas
 */
void iniciarAjudas(Ajudas *ajudas);

/**
 * Ajuda das cartas: as 4 cartas são embaralhadas, o jogador escolhe uma e ela diz quantas
 * alternativas erradas serão eliminadas (Rei = 0, Ás = 1, 2 = 2, 3 = 3).
 * @param p pergunta atual
 * @param eliminadas vetor de 4 posições; recebe 1 nas alternativas eliminadas
 * @return quantas alternativas foram eliminadas
 */
int usarCartas(Pergunta p, int eliminadas[4]);

/**
 * Sorteia uma alternativa errada que ainda não foi eliminada.
 * (Também usada pela versão gráfica, grafico.c.)
 * @param p pergunta atual
 * @param eliminadas alternativas eliminadas
 * @return índice (0 a 3) da alternativa errada, ou -1 se não houver nenhuma
 */
int sortearErrada(Pergunta p, const int eliminadas[4]);

#endif /* AJUDA_H */
