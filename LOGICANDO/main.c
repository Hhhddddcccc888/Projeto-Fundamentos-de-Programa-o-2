/**
 * @file main.c
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Programa principal: LOGICANDO, um jogo de perguntas no estilo do Show do Milhão (tema: Puzzles e Raciocínio Lógico).
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 * Equipe Rockstar Fundamentos - Fundamentos de Programação 2 - UTFPR Apucarana.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pergunta.h"
#include "arquivo.h"
#include "cadastro.h"
#include "jogo.h"
#include "ranking.h"
#include "tela.h"
#include "util.h"
#include "visual.h"

#define LARGURA_MENU 45

/**
 * Mostra o menu principal.
 * @param lista endereço da lista (para mostrar quantas perguntas existem)
 */
static void mostrarMenu(const Lista *lista){
    char texto[LARGURA_MENU];

    limparTela();
    printf("\n");
    mostrarLetreiro(0);
    printf("\n");

    caixaBorda(LARGURA_MENU, COR_AMARELO);
    caixaLinha("   PUZZLES E RACIOCINIO LOGICO", COR_AMARELO, LARGURA_MENU, COR_AMARELO);
    sprintf(texto, "   %d perguntas cadastradas", lista->quantidade);
    caixaLinha(texto, COR_CINZA, LARGURA_MENU, COR_AMARELO);
    caixaBorda(LARGURA_MENU, COR_AMARELO);
    caixaLinha(" 1 - JOGAR", COR_VERDE, LARGURA_MENU, COR_AMARELO);
    caixaLinha(" 2 - Cadastrar pergunta", COR_BRANCO, LARGURA_MENU, COR_AMARELO);
    caixaLinha(" 3 - Listar perguntas", COR_BRANCO, LARGURA_MENU, COR_AMARELO);
    caixaLinha(" 4 - Pesquisar perguntas", COR_BRANCO, LARGURA_MENU, COR_AMARELO);
    caixaLinha(" 5 - Ver perguntas por nivel", COR_BRANCO, LARGURA_MENU, COR_AMARELO);
    caixaLinha(" 6 - Alterar pergunta", COR_BRANCO, LARGURA_MENU, COR_AMARELO);
    caixaLinha(" 7 - Excluir pergunta", COR_BRANCO, LARGURA_MENU, COR_AMARELO);
    caixaLinha(" 8 - Exportar perguntas para CSV", COR_BRANCO, LARGURA_MENU, COR_AMARELO);
    caixaLinha(" 9 - Ranking e recordes", COR_CIANO, LARGURA_MENU, COR_AMARELO);
    caixaLinha(" 0 - Salvar e sair", COR_VERMELHO, LARGURA_MENU, COR_AMARELO);
    caixaBorda(LARGURA_MENU, COR_AMARELO);
}//mostrarMenu

/**
 * Carrega as perguntas: do arquivo binário se ele existir (a partir da
 * segunda execução) ou do CSV (primeira execução).
 * @param lista endereço da lista
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
        mensagemAviso("o arquivo binario esta com problema. Lendo o CSV inicial...");
    }//if

    lidas = carregarCSV(lista, ARQUIVO_CSV, &ignoradas);
    if (lidas < 0){
        mensagemErro("nao foi possivel abrir o arquivo %s. Iniciando sem perguntas.", ARQUIVO_CSV);
        return;
    }//if

    printf("%d perguntas lidas do arquivo %s.\n", lidas, ARQUIVO_CSV);
    if (ignoradas > 0){
        mensagemAviso("%d linha(s) invalida(s) foram ignoradas.", ignoradas);
    }//if
}//carregarDados

/**
 * Submenu do ranking: top 10 ou recorde pessoal de um jogador.
 * @param ranking endereço do ranking
 */
static void menuRanking(const Ranking *ranking){
    char nome[TAM_NOME];
    int opcao;

    mostrarTitulo("RANKING E RECORDES", COR_AMARELO);
    printf("1 - Ver o TOP 10\n");
    printf("2 - Ver o recorde pessoal de um jogador\n");
    printf("0 - Voltar\n");
    opcao = lerInteiro("Escolha: ", 0, 2);

    if (opcao == 1){
        mostrarRanking(ranking);
    } else if (opcao == 2){
        if (lerTextoObrigatorio("Nome do jogador: ", nome, TAM_NOME)){
            mostrarHistoricoJogador(ranking, nome);
        }//if
    }//if
}//menuRanking

int main(void){
    Lista lista;
    Ranking ranking;
    int opcao;

    srand((unsigned int)time(NULL));
    iniciarTela();

    if (!iniciarLista(&lista) || !iniciarRanking(&ranking)){
        printf("Erro: memoria insuficiente!\n");
        return 1;
    }//if

    telaAbertura();
    carregarDados(&lista);
    carregarRanking(&ranking, ARQUIVO_RANKING);
    pausar();

    do {
        mostrarMenu(&lista);
        opcao = lerInteiro("Escolha uma opcao: ", 0, 9);

        switch (opcao){
            case 1:
                jogar(&lista, &ranking);
                //salva o ranking
                if (!salvarRanking(&ranking, ARQUIVO_RANKING)){
                    mensagemErro("nao foi possivel salvar o ranking!");
                }//if
                break;
            case 2: cadastrarPergunta(&lista);  break;
            case 3: listarPerguntas(&lista);    break;
            case 4: pesquisarPerguntas(&lista); break;
            case 5: navegarPorNivel(&lista);    break;
            case 6: alterarPergunta(&lista);    break;
            case 7: excluirPergunta(&lista);    break;
            case 8: exportarPerguntas(&lista);  break;
            case 9: menuRanking(&ranking);      break;
            default: break;
        }//switch

        if (opcao != 0){
            pausar();
        }//if
    } while (opcao != 0);

    //salva tudo antes de sair
    if (salvarBinario(&lista, ARQUIVO_BINARIO) && salvarRanking(&ranking, ARQUIVO_RANKING)){
        mensagemSucesso("Dados salvos. Ate a proxima!");
    } else {
        mensagemErro("nao foi possivel salvar os arquivos binarios!");
    }//if

    liberarLista(&lista);
    liberarRanking(&ranking);
    return 0;
}//main
