/**
 * \file main.c
 * \brief Programa principal: Jogo do Milhão (tema: Computação e Programação).
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pergunta.h"
#include "arquivo.h"
#include "cadastro.h"
#include "jogo.h"
#include "util.h"

/**
 * Mostra o menu principal.
 */
static void mostrarMenu(void){
    printf("\n==============================================\n");
    printf("   JOGO DO MILHAO - COMPUTACAO E PROGRAMACAO\n");
    printf("==============================================\n");
    printf("1 - Jogar\n");
    printf("2 - Cadastrar pergunta\n");
    printf("3 - Listar perguntas\n");
    printf("4 - Pesquisar perguntas\n");
    printf("5 - Ver perguntas por nivel\n");
    printf("6 - Alterar pergunta\n");
    printf("7 - Excluir pergunta\n");
    printf("8 - Exportar perguntas para CSV\n");
    printf("0 - Salvar e sair\n");
}//mostrarMenu

/**
 * Carrega as perguntas: do arquivo binário se ele existir (a partir da
 * segunda execução) ou do CSV (primeira execução).
 * \param lista endereço da lista
 */
static void carregarDados(Lista *lista){
    FILE *teste = fopen(ARQUIVO_BINARIO, "rb");
    int lidas;
    int ignoradas;

    if (teste != NULL){
        fclose(teste);
        if (carregarBinario(lista, ARQUIVO_BINARIO)){
            printf("%d perguntas carregadas do arquivo binario.\n", lista->quantidade);
            return;
        }//if
        printf("Aviso: o arquivo binario esta com problema. Lendo o CSV inicial...\n");
    }//if

    lidas = carregarCSV(lista, ARQUIVO_CSV, &ignoradas);
    if (lidas < 0){
        printf("Erro: nao foi possivel abrir o arquivo %s. Iniciando sem perguntas.\n", ARQUIVO_CSV);
        return;
    }//if

    printf("%d perguntas lidas do arquivo %s.\n", lidas, ARQUIVO_CSV);
    if (ignoradas > 0){
        printf("Aviso: %d linha(s) invalida(s) foram ignoradas.\n", ignoradas);
    }//if
}//carregarDados

int main(void){
    Lista lista;
    int opcao;

    srand((unsigned int)time(NULL));

    if (!iniciarLista(&lista)){
        printf("Erro: memoria insuficiente!\n");
        return 1;
    }//if

    carregarDados(&lista);

    do {
        mostrarMenu();
        opcao = lerInteiro("Escolha uma opcao: ", 0, 8);

        switch (opcao){
            case 1: jogar(&lista);              break;
            case 2: cadastrarPergunta(&lista);  break;
            case 3: listarPerguntas(&lista);    break;
            case 4: pesquisarPerguntas(&lista); break;
            case 5: navegarPorNivel(&lista);    break;
            case 6: alterarPergunta(&lista);    break;
            case 7: excluirPergunta(&lista);    break;
            case 8: exportarPerguntas(&lista);  break;
            default: break;
        }//switch
    } while (opcao != 0);

    //ao sair, salva tudo no arquivo binário
    if (salvarBinario(&lista, ARQUIVO_BINARIO)){
        printf("Dados salvos. Ate a proxima!\n");
    } else {
        printf("Erro: nao foi possivel salvar o arquivo binario!\n");
    }//if

    liberarLista(&lista);
    return 0;
}//main
