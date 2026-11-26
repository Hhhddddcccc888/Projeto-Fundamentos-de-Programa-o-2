/**
 * @file cadastro.c
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Implementação das funções do cadastro de perguntas.
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "cadastro.h"
#include "arquivo.h"
#include "util.h"
#include "tela.h"

#define TEXTO_NIVEIS "Nivel (1-Muito facil, 2-Facil, 3-Medio, 4-Dificil, 5-Muito dificil): "

void cadastrarPergunta(Lista *lista){
    Pergunta nova;
    char mensagem[30];
    int i;
    int id;

    memset(&nova, 0, sizeof(Pergunta));
    mostrarTitulo("CADASTRAR PERGUNTA", COR_CIANO);

    if (!lerTextoObrigatorio("Enunciado: ", nova.enunciado, TAM_ENUNCIADO)){
        return;
    }//if

    for (i = 0; i < 4; i++){
        sprintf(mensagem, "Alternativa %c: ", 'A' + i);
        if (!lerTextoObrigatorio(mensagem, nova.alternativas[i], TAM_ALTERNATIVA)){
            return;
        }//if
    }//for

    nova.correta = lerLetra("Alternativa correta (A, B, C ou D): ", "ABCD");
    nova.nivel = lerInteiro(TEXTO_NIVEIS, 1, 5);

    id = inserirPergunta(lista, nova);
    if (id < 0){
        mensagemErro("memoria insuficiente para cadastrar a pergunta!");
    } else {
        mensagemSucesso("Pergunta cadastrada com sucesso! ID: %d", id);
    }//if
}//cadastrarPergunta

void listarPerguntas(const Lista *lista){
    int i;
    Pergunta p;

    mostrarTitulo("LISTA DE PERGUNTAS", COR_CIANO);
    if (lista->quantidade == 0){
        mensagemAviso("Nenhuma pergunta cadastrada.");
        return;
    }//if

    for (i = 0; i < lista->quantidade; i++){
        p = lista->itens[i];
        printf("%s[ID %3d] %-13s%s %s\n", corDoNivel(p.nivel), p.id,
               nomeNivel(p.nivel), COR_RESET, p.enunciado);

        //pausa a cada 15 perguntas
        if ((i + 1) % 15 == 0 && i + 1 < lista->quantidade){
            pausar();
        }//if
    }//for
    printf("Total: %d pergunta(s).\n", lista->quantidade);
}//listarPerguntas

void pesquisarPerguntas(const Lista *lista){
    char palavra[40];
    int opcao;
    int i;
    int id;
    int indice;
    int achou = 0;

    mostrarTitulo("PESQUISAR PERGUNTAS", COR_CIANO);
    printf("1 - Por palavra do enunciado\n");
    printf("2 - Por ID\n");
    printf("0 - Voltar\n");
    opcao = lerInteiro("Escolha: ", 0, 2);

    if (opcao == 1){
        if (!lerTextoObrigatorio("Palavra: ", palavra, sizeof(palavra))){
            return;
        }//if
        for (i = 0; i < lista->quantidade; i++){
            if (contemTexto(lista->itens[i].enunciado, palavra)){
                printf("\n");
                mostrarPergunta(lista->itens[i], 1);
                achou++;
            }//if
        }//for
        if (achou == 0){
            mensagemAviso("Nenhuma pergunta encontrada!");
        } else {
            printf("\n%d pergunta(s) encontrada(s).\n", achou);
        }//if
    } else if (opcao == 2){
        id = lerInteiro("ID: ", 1, INT_MAX);
        indice = buscarPorId(lista, id);
        if (indice < 0){
            mensagemErro("nao existe pergunta com esse ID!");
        } else {
            printf("\n");
            mostrarPergunta(lista->itens[indice], 1);
        }//if
    }//if
}//pesquisarPerguntas

/**
 * Procura a próxima pergunta de um nível, a partir de uma posição.
 * @param lista endereço da lista
 * @param nivel nível procurado
 * @param inicio posição onde a busca começa
 * @param passo 1 para procurar para frente, -1 para procurar para trás
 * @return posição encontrada, ou -1 se não existe
 */
static int procurarNivel(const Lista *lista, int nivel, int inicio, int passo){
    int i;

    for (i = inicio; i >= 0 && i < lista->quantidade; i = i + passo){
        if (lista->itens[i].nivel == nivel){
            return i;
        }//if
    }//for
    return -1;
}//procurarNivel

/**
 * Mostra quantas perguntas existem em cada nível, cada um com sua cor.
 * @param lista endereço da lista
 */
static void mostrarResumoNiveis(const Lista *lista){
    int nivel;

    for (nivel = MUITO_FACIL; nivel <= MUITO_DIFICIL; nivel++){
        printf("  %s%d - %-13s (%d perguntas)%s\n", corDoNivel(nivel), nivel,
               nomeNivel(nivel), contarNivel(lista, nivel), COR_RESET);
    }//for
}//mostrarResumoNiveis

void navegarPorNivel(const Lista *lista){
    int nivel;
    int atual;
    int outra;
    int posicao;
    int i;
    char opcao;

    mostrarTitulo("NAVEGAR POR NIVEL", COR_CIANO);
    mostrarResumoNiveis(lista);
    nivel = lerInteiro("Escolha o nivel (1 a 5): ", 1, 5);

    atual = procurarNivel(lista, nivel, 0, 1);
    if (atual < 0){
        mensagemAviso("Nenhuma pergunta neste nivel!");
        return;
    }//if

    do {
        //posição da pergunta no nível
        posicao = 0;
        for (i = 0; i <= atual; i++){
            if (lista->itens[i].nivel == nivel){
                posicao++;
            }//if
        }//for

        limparTela();
        mostrarTitulo(nomeNivel(nivel), corDoNivel(nivel));
        printf("Pergunta %d de %d\n\n", posicao, contarNivel(lista, nivel));
        mostrarPergunta(lista->itens[atual], 1);

        opcao = lerLetra("\n[P] Proxima  [A] Anterior  [S] Sair: ", "SPA");
        if (opcao == 'P'){
            outra = procurarNivel(lista, nivel, atual + 1, 1);
            if (outra < 0){
                mensagemAviso("Esta e a ultima pergunta do nivel!");
                pausar();
            } else {
                atual = outra;
            }//if
        } else if (opcao == 'A'){
            outra = procurarNivel(lista, nivel, atual - 1, -1);
            if (outra < 0){
                mensagemAviso("Esta e a primeira pergunta do nivel!");
                pausar();
            } else {
                atual = outra;
            }//if
        }//if
    } while (opcao != 'S');
}//navegarPorNivel

void alterarPergunta(Lista *lista){
    Pergunta nova;
    char mensagem[40];
    int id;
    int indice;
    int i;
    int nivel;
    char letra;

    mostrarTitulo("ALTERAR PERGUNTA", COR_CIANO);
    if (lista->quantidade == 0){
        mensagemAviso("Nenhuma pergunta cadastrada.");
        return;
    }//if

    id = lerInteiro("ID da pergunta: ", 1, INT_MAX);
    indice = buscarPorId(lista, id);
    if (indice < 0){
        mensagemErro("nao existe pergunta com esse ID!");
        return;
    }//if

    nova = lista->itens[indice];
    printf("\n");
    mostrarPergunta(nova, 1);
    printf("\nSe quiser manter um valor, apenas aperte ENTER.\n");

    if (!lerTextoOpcional("Novo enunciado: ", nova.enunciado, TAM_ENUNCIADO)){
        return;
    }//if
    for (i = 0; i < 4; i++){
        sprintf(mensagem, "Nova alternativa %c: ", 'A' + i);
        if (!lerTextoOpcional(mensagem, nova.alternativas[i], TAM_ALTERNATIVA)){
            return;
        }//if
    }//for

    //M ou 0 mantem o valor antigo
    letra = lerLetra("Nova alternativa correta (A, B, C, D) ou M para manter: ", "MABCD");
    if (letra != 'M'){
        nova.correta = letra;
    }//if
    nivel = lerInteiro("Novo nivel (1 a 5) ou 0 para manter: ", 0, 5);
    if (nivel != 0){
        nova.nivel = nivel;
    }//if

    printf("\nComo vai ficar:\n");
    mostrarPergunta(nova, 1);

    if (lerLetra("Salvar as alteracoes? (S/N): ", "NS") == 'S'){
        lista->itens[indice] = nova;
        mensagemSucesso("Pergunta alterada com sucesso!");
    } else {
        mensagemAviso("Alteracao cancelada.");
    }//if
}//alterarPergunta

void excluirPergunta(Lista *lista){
    int id;
    int indice;

    mostrarTitulo("EXCLUIR PERGUNTA", COR_VERMELHO);
    if (lista->quantidade == 0){
        mensagemAviso("Nenhuma pergunta cadastrada.");
        return;
    }//if

    id = lerInteiro("ID da pergunta: ", 1, INT_MAX);
    indice = buscarPorId(lista, id);
    if (indice < 0){
        mensagemErro("nao existe pergunta com esse ID!");
        return;
    }//if

    printf("\n");
    mostrarPergunta(lista->itens[indice], 1);

    if (lerLetra("\nTem certeza que deseja excluir? (S/N): ", "NS") == 'S'){
        removerPergunta(lista, indice);
        mensagemSucesso("Pergunta excluida com sucesso!");
    } else {
        mensagemAviso("Exclusao cancelada.");
    }//if
}//excluirPergunta

void exportarPerguntas(const Lista *lista){
    char nome[60];
    FILE *teste;

    mostrarTitulo("EXPORTAR PARA CSV", COR_CIANO);
    if (lista->quantidade == 0){
        mensagemAviso("Nao ha perguntas para exportar.");
        return;
    }//if

    if (!lerTextoObrigatorio("Nome do arquivo (exemplo: saida.csv): ", nome, sizeof(nome))){
        return;
    }//if

    //pergunta antes de substituir o arquivo
    teste = fopen(nome, "r");
    if (teste != NULL){
        fclose(teste);
        if (lerLetra("O arquivo ja existe. Deseja substituir? (S/N): ", "NS") != 'S'){
            mensagemAviso("Exportacao cancelada.");
            return;
        }//if
    }//if

    if (exportarCSV(lista, nome)){
        mensagemSucesso("%d perguntas exportadas para %s.", lista->quantidade, nome);
    } else {
        mensagemErro("nao foi possivel gravar o arquivo!");
    }//if
}//exportarPerguntas
