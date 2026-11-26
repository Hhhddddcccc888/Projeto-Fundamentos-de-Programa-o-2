/**
 * @file ranking.c
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Implementação do ranking local e do recorde pessoal.
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ranking.h"
#include "tela.h"
#include "util.h"

#define CAPACIDADE_INICIAL_RANKING 10
#define LIMITE_RESULTADOS 100000

int iniciarRanking(Ranking *ranking){
    ranking->quantidade = 0;
    ranking->itens = malloc(CAPACIDADE_INICIAL_RANKING * sizeof(Resultado));

    if (ranking->itens == NULL){
        ranking->capacidade = 0;
        return 0;
    }//if

    ranking->capacidade = CAPACIDADE_INICIAL_RANKING;
    return 1;
}//iniciarRanking

void liberarRanking(Ranking *ranking){
    free(ranking->itens);
    ranking->itens = NULL;
    ranking->quantidade = 0;
    ranking->capacidade = 0;
}//liberarRanking

/**
 * Procura o resultado de um jogador (sem diferenciar maiúsculas de minúsculas).
 * @param ranking endereço do ranking
 * @param nome nome do jogador
 * @return posição do resultado no vetor, ou -1 se o jogador não está no ranking
 */
static int buscarJogador(const Ranking *ranking, const char *nome){
    int i;

    for (i = 0; i < ranking->quantidade; i++){
        if (textosIguais(ranking->itens[i].nome, nome)){
            return i;
        }//if
    }//for
    return -1;
}//buscarJogador

int adicionarResultado(Ranking *ranking, Resultado r){
    Resultado *novo;
    int posicao = buscarJogador(ranking, r.nome);

    //jogador repetido: fica a maior pontuação
    if (posicao >= 0){
        if (r.pontos > ranking->itens[posicao].pontos){
            ranking->itens[posicao] = r;
        }//if
        return 1;
    }//if

    //jogador novo vai pro final do vetor
    if (ranking->quantidade == ranking->capacidade){
        novo = realloc(ranking->itens, ranking->capacidade * 2 * sizeof(Resultado));
        if (novo == NULL){
            return 0;
        }//if
        ranking->itens = novo;
        ranking->capacidade = ranking->capacidade * 2;
    }//if

    ranking->itens[ranking->quantidade] = r;
    ranking->quantidade++;
    return 1;
}//adicionarResultado

int calcularPontos(long base, int bonus, int segundos){
    int pontos = (int)base + bonus - segundos / 10;

    if (pontos < 0){
        pontos = 0;
    }//if
    return pontos;
}//calcularPontos

int recordePessoal(const Ranking *ranking, const char *nome){
    int i;
    int recorde = -1;

    for (i = 0; i < ranking->quantidade; i++){
        if (textosIguais(ranking->itens[i].nome, nome) && ranking->itens[i].pontos > recorde){
            recorde = ranking->itens[i].pontos;
        }//if
    }//for
    return recorde;
}//recordePessoal

/**
 * Compara dois resultados para o qsort (maior pontuação primeiro;
 * em caso de empate, quem jogou mais rápido fica na frente).
 * @param a endereço do primeiro resultado
 * @param b endereço do segundo resultado
 * @return negativo se a vem antes de b, positivo se vem depois, 0 se empatar
 */
static int compararResultados(const void *a, const void *b){
    const Resultado *ra = (const Resultado *)a;
    const Resultado *rb = (const Resultado *)b;

    if (ra->pontos != rb->pontos){
        return rb->pontos - ra->pontos;
    }//if
    return ra->segundos - rb->segundos;
}//compararResultados

/**
 * Mostra uma linha da tabela do ranking.
 * @param posicao posição no ranking (0 para não mostrar)
 * @param r resultado
 */
static void mostrarLinha(int posicao, Resultado r){
    const char *cor = COR_RESET;

    //ouro, prata e bronze
    if (posicao == 1){
        cor = COR_AMARELO;
    } else if (posicao == 2){
        cor = COR_BRANCO;
    } else if (posicao == 3){
        cor = COR_ROXO;
    }//if

    if (posicao > 0){
        printf("%s%3d  ", cor, posicao);
    } else {
        printf("%s     ", cor);
    }//if
    printf("%-20s %6d %7d %4dm%02ds %5d%s\n", r.nome,
           r.pontos, r.acertos, r.segundos / 60, r.segundos % 60,
           r.bonus, COR_RESET);
}//mostrarLinha

/**
 * Mostra o cabeçalho da tabela do ranking.
 */
static void mostrarCabecalho(void){
    printf("%s  #  %-20s %6s %7s %8s %5s%s\n", COR_CIANO, "Jogador",
           "Pontos", "Acertos", "Tempo", "Bonus", COR_RESET);
}//mostrarCabecalho

void mostrarRanking(const Ranking *ranking){
    Resultado *copia;
    int i;
    int limite;

    mostrarTitulo("RANKING - TOP 10", COR_AMARELO);
    if (ranking->quantidade == 0){
        mensagemAviso("Nenhuma partida jogada ainda. Seja o primeiro!");
        return;
    }//if

    //ordena uma copia do vetor
    copia = malloc(ranking->quantidade * sizeof(Resultado));
    if (copia == NULL){
        mensagemErro("memoria insuficiente para montar o ranking!");
        return;
    }//if
    memcpy(copia, ranking->itens, ranking->quantidade * sizeof(Resultado));
    qsort(copia, ranking->quantidade, sizeof(Resultado), compararResultados);

    limite = ranking->quantidade;
    if (limite > TAMANHO_TOP){
        limite = TAMANHO_TOP;
    }//if

    mostrarCabecalho();
    for (i = 0; i < limite; i++){
        mostrarLinha(i + 1, copia[i]);
    }//for
    printf("Total de jogadores no ranking: %d\n", ranking->quantidade);

    free(copia);
}//mostrarRanking

void mostrarHistoricoJogador(const Ranking *ranking, const char *nome){
    int posicao = buscarJogador(ranking, nome);

    mostrarTitulo("RECORDE PESSOAL", COR_AMARELO);
    if (posicao < 0){
        mensagemAviso("O jogador %s ainda nao jogou nenhuma partida.", nome);
        return;
    }//if

    printf("Recorde de %s: %s%d pontos%s\n\n", nome, COR_AMARELO,
           ranking->itens[posicao].pontos, COR_RESET);
    printf("Melhor partida:\n");
    mostrarCabecalho();
    mostrarLinha(0, ranking->itens[posicao]);
}//mostrarHistoricoJogador

int salvarRanking(const Ranking *ranking, const char *nome){
    FILE *arq = fopen(nome, "wb");
    int ok;

    if (arq == NULL){
        return 0;
    }//if

    //grava os dados no arquivo
    ok = (fwrite(&ranking->quantidade, sizeof(int), 1, arq) == 1);
    if (ok && ranking->quantidade > 0){
        ok = (fwrite(ranking->itens, sizeof(Resultado), ranking->quantidade, arq)
              == (size_t)ranking->quantidade);
    }//if

    if (fclose(arq) != 0){
        ok = 0;
    }//if
    return ok;
}//salvarRanking

/**
 * Deixa só um resultado por jogador, o de maior pontuação. Serve para
 * arquivos antigos, gravados quando o ranking guardava todas as partidas.
 * @param ranking endereço do ranking
 */
static void juntarRepetidos(Ranking *ranking){
    int i;
    int j;
    int ficam = 0;
    int repetido;

    for (i = 0; i < ranking->quantidade; i++){
        repetido = 0;
        //procura o nome repetido
        for (j = 0; j < ficam && !repetido; j++){
            if (textosIguais(ranking->itens[j].nome, ranking->itens[i].nome)){
                repetido = 1;
                if (ranking->itens[i].pontos > ranking->itens[j].pontos){
                    ranking->itens[j] = ranking->itens[i];
                }//if
            }//if
        }//for
        if (!repetido){
            ranking->itens[ficam] = ranking->itens[i];
            ficam++;
        }//if
    }//for
    ranking->quantidade = ficam;
}//juntarRepetidos

int carregarRanking(Ranking *ranking, const char *nome){
    FILE *arq = fopen(nome, "rb");
    Resultado *itens;
    int quantidade;
    int capacidade;
    int i;

    if (arq == NULL){
        return 0;
    }//if

    if (fread(&quantidade, sizeof(int), 1, arq) != 1 ||
        quantidade < 0 || quantidade > LIMITE_RESULTADOS){
        fclose(arq);
        return 0;
    }//if

    capacidade = quantidade;
    if (capacidade < CAPACIDADE_INICIAL_RANKING){
        capacidade = CAPACIDADE_INICIAL_RANKING;
    }//if

    itens = malloc(capacidade * sizeof(Resultado));
    if (itens == NULL){
        fclose(arq);
        return 0;
    }//if

    if (quantidade > 0 &&
        fread(itens, sizeof(Resultado), quantidade, arq) != (size_t)quantidade){
        free(itens);
        fclose(arq);
        return 0;
    }//if
    fclose(arq);

    //coloca o '\0' no fim dos nomes
    for (i = 0; i < quantidade; i++){
        itens[i].nome[TAM_NOME - 1] = '\0';
    }//for

    free(ranking->itens);
    ranking->itens = itens;
    ranking->quantidade = quantidade;
    ranking->capacidade = capacidade;
    juntarRepetidos(ranking);
    return 1;
}//carregarRanking
