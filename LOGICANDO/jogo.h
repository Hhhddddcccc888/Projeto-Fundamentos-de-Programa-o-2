/**
 * @file jogo.h
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Partida do LOGICANDO contra o computador (regras inspiradas no Show do Milhão, do SBT) e o humor do cachorro Lógica.
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef JOGO_H
#define JOGO_H

#include "pergunta.h"
#include "ranking.h"

/**
 * Joga uma partida de 15 perguntas:
 *  - cada acerto vale 100 pontos;
 *  - ajudas: pular (3x) e cartas (não valem na última pergunta);
 *  - parar: leva os pontos que já fez;
 *  - errar: leva metade dos pontos, mas nunca menos que o marco de
 *    segurança (perguntas 5 e 10); errar a última pergunta zera os pontos.
 * O cachorro Lógica vai ficando mais feliz a cada acerto.
 * Também mede o tempo, dá bônus por sequência de acertos rápidos e grava
 * o resultado no ranking, avisando se o jogador bateu o recorde pessoal.
 * @param lista endereço da lista de perguntas
 * @param ranking endereço do ranking, onde o resultado será adicionado
 */
void jogar(const Lista *lista, Ranking *ranking);

/* Regras da partida. Também são usadas pela versão gráfica (grafico.c). */

#define TOTAL_ETAPAS 15
#define PONTOS_POR_ACERTO 100  /**< pontos de cada pergunta acertada */
#define SEQUENCIA_BONUS 3      /**< acertos seguidos para ganhar o bônus */
#define LIMITE_RAPIDO 60       /**< segundos para o acerto contar como rápido */
#define PONTOS_BONUS 50        /**< pontos ganhos a cada sequência */

/** Humor do cachorro Lógica, que muda conforme o jogador acerta */
typedef enum {
    LOGICA_DORMINDO,   /**< 0 acertos: deitado, dormindo */
    LOGICA_ACORDADO,   /**< 1 a 4 acertos: deitado, acordado */
    LOGICA_ANIMADO,    /**< 5 a 9 acertos: sentado, sorrindo */
    LOGICA_FELIZ,      /**< 10 a 14 acertos: em pé, com a língua de fora */
    LOGICA_EM_FESTA,   /**< 15 acertos: pulando de alegria */
    LOGICA_TRISTE      /**< o jogador errou */
} Humor;//enum

/**
 * Descobre o nível da pergunta de cada etapa.
 * @param etapa número da etapa (1 a 15)
 * @return nível da etapa
 */
int nivelDaEtapa(int etapa);

/**
 * Pontos acumulados depois de acertar uma certa quantidade de perguntas.
 * @param acertos perguntas acertadas (0 a 15)
 * @return pontos (100 por acerto)
 */
long pontosDosAcertos(int acertos);

/**
 * Pontos de quem erra (regra do SBT): metade do acumulado, mas nunca
 * menos que o marco de segurança. Na última pergunta, perde tudo.
 * @param etapa etapa em que o jogador errou
 * @param acertos perguntas acertadas até agora
 * @return pontos que o jogador leva
 */
long pontosAoErrar(int etapa, int acertos);

/**
 * Sorteia uma pergunta de um nível que ainda não saiu na partida.
 * @param lista endereço da lista
 * @param nivel nível da pergunta
 * @param usadas vetor com as posições das perguntas que já saíram
 * @param quantidade quantas posições de usadas estão preenchidas
 * @return posição da pergunta sorteada, ou -1 se não há mais perguntas
 */
int sortearPergunta(const Lista *lista, int nivel, const int usadas[], int quantidade);

/**
 * Humor do Lógica de acordo com a quantidade de acertos.
 * @param acertos perguntas acertadas (0 a 15)
 * @return humor do cachorro
 */
Humor humorDaLogica(int acertos);

/**
 * Texto curto que descreve o humor do Lógica (ex: "ANIMADO").
 * @param humor humor do cachorro
 * @return texto em maiúsculas, sem acentos
 */
const char *textoDoHumor(Humor humor);

#endif /* JOGO_H */
