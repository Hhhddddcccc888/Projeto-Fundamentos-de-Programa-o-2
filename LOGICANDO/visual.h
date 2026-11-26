/**
 * @file visual.h
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Efeitos visuais do jogo: letreiro, abertura animada, suspense antes da resposta, comemoração e o cachorro Lógica.
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef VISUAL_H
#define VISUAL_H

#include "pergunta.h"
#include "jogo.h"

/**
 * Desenha o letreiro "LOGICANDO" em letras grandes, cada letra de uma cor.
 * @param deslocamento muda qual cor cada letra recebe (usado na animação)
 */
void mostrarLetreiro(int deslocamento);

/**
 * Tela de abertura: o letreiro pisca trocando de cores, toca uma
 * musiquinha e o subtítulo aparece letra por letra.
 */
void telaAbertura(void);

/**
 * Suspense antes de revelar a resposta: "Vamos ver..." com pontinhos
 * aparecendo devagar e beeps de tensão.
 */
void suspense(void);

/**
 * Mostra as alternativas com a correta em verde e, se o jogador errou,
 * a escolhida em vermelho.
 * @param p pergunta respondida
 * @param escolhida letra escolhida pelo jogador
 * @param largura largura da caixa
 */
void revelarResposta(Pergunta p, char escolhida, int largura);

/**
 * Animação de quem acerta as 15 perguntas: chuva de símbolos coloridos.
 */
void comemorarVitoria(void);

/**
 * Desenha o cachorro Lógica (de óculos) em ASCII, com 7 linhas de altura
 * e no máximo 19 colunas de largura. A pose muda com o humor: deitado
 * dormindo, deitado acordado, sentado, em pé, em festa ou triste.
 * @param humor humor do cachorro
 * @param linha linha da tela onde o desenho começa; 0 desenha onde o
 *        cursor estiver, uma linha embaixo da outra
 * @param coluna coluna da tela (usada só quando linha > 0)
 */
void desenharLogica(Humor humor, int linha, int coluna);

#endif /* VISUAL_H */
