/**
 * @file cadastro.h
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Funções do cadastro de perguntas (inserir, listar, pesquisar, alterar, excluir e exportar).
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef CADASTRO_H
#define CADASTRO_H

#include "pergunta.h"

/**
 * Lê os dados de uma nova pergunta e a insere na lista.
 * @param lista endereço da lista
 */
void cadastrarPergunta(Lista *lista);

/**
 * Lista todas as perguntas (id, nível e enunciado).
 * @param lista endereço da lista
 */
void listarPerguntas(const Lista *lista);

/**
 * Pesquisa perguntas por palavra do enunciado ou por id.
 * @param lista endereço da lista
 */
void pesquisarPerguntas(const Lista *lista);

/**
 * Mostra as perguntas de um nível, uma de cada vez, permitindo
 * navegar para a próxima ou para a anterior.
 * @param lista endereço da lista
 */
void navegarPorNivel(const Lista *lista);

/**
 * Altera os dados de uma pergunta escolhida pelo id.
 * @param lista endereço da lista
 */
void alterarPergunta(Lista *lista);

/**
 * Exclui uma pergunta escolhida pelo id (pede confirmação).
 * @param lista endereço da lista
 */
void excluirPergunta(Lista *lista);

/**
 * Exporta todas as perguntas para um arquivo CSV.
 * @param lista endereço da lista
 */
void exportarPerguntas(const Lista *lista);

#endif /* CADASTRO_H */
