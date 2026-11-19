/**
 * \file jogo.h
 * \brief Partida do Jogo do Milhão (15 perguntas).
 */

#ifndef JOGO_H
#define JOGO_H

#include "pergunta.h"

/**
 * Executa uma partida completa. As perguntas são sorteadas de acordo
 * com o nível de cada etapa: 1-2 muito fácil, 3-4 fácil, 5-8 médio,
 * 9-12 difícil e 13-15 muito difícil. Marcos de segurança: 5 e 10.
 * \param lista endereço da lista de perguntas
 */
void jogar(const Lista *lista);

#endif
