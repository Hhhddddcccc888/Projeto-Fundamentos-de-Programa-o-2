/**
 * \file arquivo.h
 * \brief Leitura e gravação das perguntas em arquivo CSV e arquivo binário.
 */

#ifndef ARQUIVO_H
#define ARQUIVO_H

#include "pergunta.h"

#define ARQUIVO_CSV "perguntas.csv"
#define ARQUIVO_BINARIO "perguntas.bin"

/**
 * Lê as perguntas de um arquivo CSV (nivel,enunciado,altA,altB,altC,altD,correta)
 * e coloca todas na lista. Linhas inválidas são ignoradas.
 * \param lista endereço da lista
 * \param nome nome do arquivo CSV
 * \param ignoradas endereço da variável que recebe quantas linhas foram ignoradas
 * \return quantidade de perguntas lidas, ou -1 se não conseguiu abrir o arquivo
 */
int carregarCSV(Lista *lista, const char *nome, int *ignoradas);

/**
 * Grava todas as perguntas da lista em um arquivo CSV.
 * \param lista endereço da lista
 * \param nome nome do arquivo que será criado
 * \return 1 se gravou, 0 se deu erro
 */
int exportarCSV(const Lista *lista, const char *nome);

/**
 * Grava a lista em um arquivo binário.
 * \param lista endereço da lista
 * \param nome nome do arquivo binário
 * \return 1 se gravou, 0 se deu erro
 */
int salvarBinario(const Lista *lista, const char *nome);

/**
 * Lê a lista de um arquivo binário. Se o arquivo não existir ou estiver
 * com problema, a lista não é alterada.
 * \param lista endereço da lista (o conteúdo antigo é substituído)
 * \param nome nome do arquivo binário
 * \return 1 se leu, 0 se não conseguiu
 */
int carregarBinario(Lista *lista, const char *nome);

#endif
