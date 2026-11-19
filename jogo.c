/**
 * \file jogo.c
 * \brief Implementação da partida do Jogo do Milhão.
 */

#include <stdio.h>
#include <stdlib.h>
#include "jogo.h"
#include "util.h"

#define TOTAL_ETAPAS 15

/**
 * Descobre o nível da pergunta de cada etapa.
 * \param etapa número da etapa (1 a 15)
 * \return nível da etapa
 */
static int nivelDaEtapa(int etapa){
    if (etapa <= 2){
        return MUITO_FACIL;
    } else if (etapa <= 4){
        return FACIL;
    } else if (etapa <= 8){
        return MEDIO;
    } else if (etapa <= 12){
        return DIFICIL;
    }//if
    return MUITO_DIFICIL;
}//nivelDaEtapa

/**
 * Retorna o prêmio de uma etapa.
 * \param etapa número da etapa (0 a 15), a etapa 0 vale zero
 * \return valor em reais
 */
static long premioDaEtapa(int etapa){
    long premios[TOTAL_ETAPAS + 1] = {
        0,
        1000, 2000, 3000, 4000, 5000,
        10000, 20000, 30000, 40000, 50000,
        100000, 200000, 300000, 500000, 1000000
    };

    return premios[etapa];
}//premioDaEtapa

/**
 * Retorna o prêmio garantido pelos marcos de segurança (perguntas 5 e 10).
 * \param acertos quantidade de perguntas já acertadas
 * \return valor em reais que o jogador leva se errar
 */
static long premioGarantido(int acertos){
    if (acertos >= 10){
        return premioDaEtapa(10);
    } else if (acertos >= 5){
        return premioDaEtapa(5);
    }//if
    return 0;
}//premioGarantido

/**
 * Confere se existem perguntas suficientes em cada nível para uma partida.
 * \param lista endereço da lista
 * \return 1 se dá para jogar, 0 caso contrário
 */
static int temPerguntasSuficientes(const Lista *lista){
    int precisa[6] = {0, 0, 0, 0, 0, 0};
    int etapa;
    int nivel;
    int ok = 1;

    for (etapa = 1; etapa <= TOTAL_ETAPAS; etapa++){
        precisa[nivelDaEtapa(etapa)]++;
    }//for

    for (nivel = MUITO_FACIL; nivel <= MUITO_DIFICIL; nivel++){
        if (contarNivel(lista, nivel) < precisa[nivel]){
            printf("Erro: o nivel %s precisa de %d pergunta(s), mas tem so %d.\n",
                   nomeNivel(nivel), precisa[nivel], contarNivel(lista, nivel));
            ok = 0;
        }//if
    }//for

    if (!ok){
        printf("Cadastre mais perguntas para poder jogar.\n");
    }//if
    return ok;
}//temPerguntasSuficientes

/**
 * Verifica se uma pergunta já saiu na partida.
 * \param indice posição da pergunta no vetor
 * \param usadas vetor com as posições das perguntas que já saíram
 * \param quantidade quantas posições de usadas estão preenchidas
 * \return 1 se já foi usada, 0 caso contrário
 */
static int jaUsada(int indice, const int usadas[], int quantidade){
    int i;

    for (i = 0; i < quantidade; i++){
        if (usadas[i] == indice){
            return 1;
        }//if
    }//for
    return 0;
}//jaUsada

/**
 * Sorteia uma pergunta de um nível que ainda não saiu na partida.
 * \param lista endereço da lista
 * \param nivel nível da pergunta
 * \param usadas vetor com as posições das perguntas que já saíram
 * \param quantidade quantas posições de usadas estão preenchidas
 * \return posição da pergunta sorteada, ou -1 se não há mais perguntas
 */
static int sortearPergunta(const Lista *lista, int nivel, const int usadas[], int quantidade){
    int i;
    int candidatas = 0;
    int sorteada;
    int contador = 0;

    for (i = 0; i < lista->quantidade; i++){
        if (lista->itens[i].nivel == nivel && !jaUsada(i, usadas, quantidade)){
            candidatas++;
        }//if
    }//for

    if (candidatas == 0){
        return -1;
    }//if

    sorteada = rand() % candidatas;
    for (i = 0; i < lista->quantidade; i++){
        if (lista->itens[i].nivel == nivel && !jaUsada(i, usadas, quantidade)){
            if (contador == sorteada){
                return i;
            }//if
            contador++;
        }//if
    }//for
    return -1;
}//sortearPergunta

void jogar(const Lista *lista){
    int usadas[TOTAL_ETAPAS];
    int etapa = 1;
    int acertos = 0;
    int terminou = 0;
    int indice;
    long premio = 0;
    char resposta;
    Pergunta p;
    int i;

    printf("\n--- JOGO DO MILHAO ---\n");
    if (!temPerguntasSuficientes(lista)){
        return;
    }//if

    printf("Responda 15 perguntas e ganhe R$ 1.000.000!\n");
    printf("Digite A, B, C ou D para responder, ou P para parar e levar o premio.\n");

    while (etapa <= TOTAL_ETAPAS && !terminou){
        indice = sortearPergunta(lista, nivelDaEtapa(etapa), usadas, etapa - 1);
        if (indice < 0){
            printf("Erro: nao ha perguntas suficientes para continuar!\n");
            return;
        }//if
        usadas[etapa - 1] = indice;
        p = lista->itens[indice];

        printf("\n=== Pergunta %d de %d (%s) ===\n", etapa, TOTAL_ETAPAS, nomeNivel(p.nivel));
        printf("Acertar: R$ %ld | Parar: R$ %ld | Errar: R$ %ld\n",
               premioDaEtapa(etapa), premioDaEtapa(etapa - 1), premioGarantido(etapa - 1));
        if (etapa == 5 || etapa == 10){
            printf("*** MARCO DE SEGURANCA: se acertar, este valor fica garantido! ***\n");
        }//if

        printf("%s\n", p.enunciado);
        for (i = 0; i < 4; i++){
            printf("  %c) %s\n", 'A' + i, p.alternativas[i]);
        }//for

        resposta = lerLetra("Sua resposta (A, B, C, D ou P para parar): ", "PABCD");

        if (resposta == 'P'){
            premio = premioDaEtapa(acertos);
            printf("\nVoce parou!\n");
            terminou = 1;
        } else if (resposta == p.correta){
            printf("Resposta certa!\n");
            acertos++;
            etapa++;
        } else {
            printf("Resposta errada! A correta era a letra %c.\n", p.correta);
            premio = premioGarantido(acertos);
            terminou = 1;
        }//if
    }//while

    if (!terminou){
        premio = premioDaEtapa(TOTAL_ETAPAS);
        printf("\nPARABENS! VOCE GANHOU O MILHAO!\n");
    }//if

    printf("Perguntas acertadas: %d\n", acertos);
    printf("Premio final: R$ %ld\n", premio);
}//jogar
