/**
 * @file visual.c
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Implementação dos efeitos visuais do jogo.
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "visual.h"
#include "tela.h"

#define LETRAS_LETREIRO 9
#define LINHAS_LETRA 5
#define QUADROS_ABERTURA 6
#define QUADROS_CHUVA 60

/**
 * "Fonte" do letreiro: cada letra de LOGICANDO é desenhada com 5 linhas de '#'.
 * @param letra índice da letra (0 a 8)
 * @param linha linha da letra (0 a 4)
 * @return texto da linha
 */
static const char *linhaDaLetra(int letra, int linha){
    static const char *fonte[LETRAS_LETREIRO][LINHAS_LETRA] = {
        {"#    ", "#    ", "#    ", "#    ", "#####"},  //L
        {" ### ", "#   #", "#   #", "#   #", " ### "},  //O
        {" ####", "#    ", "#  ##", "#   #", " ### "},  //G
        {"#####", "  #  ", "  #  ", "  #  ", "#####"},  //I
        {" ####", "#    ", "#    ", "#    ", " ####"},  //C
        {" ### ", "#   #", "#####", "#   #", "#   #"},  //A
        {"#   #", "##  #", "# # #", "#  ##", "#   #"},  //N
        {"#### ", "#   #", "#   #", "#   #", "#### "},  //D
        {" ### ", "#   #", "#   #", "#   #", " ### "}   //O
    };

    return fonte[letra][linha];
}//linhaDaLetra

void mostrarLetreiro(int deslocamento){
    const char *cores[5] = {COR_AMARELO, COR_VERDE, COR_CIANO, COR_ROXO, COR_VERMELHO};
    int linha;
    int letra;

    for (linha = 0; linha < LINHAS_LETRA; linha++){
        printf("   ");
        for (letra = 0; letra < LETRAS_LETREIRO; letra++){
            printf("%s%s  ", cores[(letra + deslocamento) % 5], linhaDaLetra(letra, linha));
        }//for
        printf("%s\n", COR_RESET);
    }//for
}//mostrarLetreiro

void telaAbertura(void){
    int quadro;

    //redesenha o letreiro trocando as cores
    for (quadro = 0; quadro < QUADROS_ABERTURA; quadro++){
        limparTela();
        printf("\n");
        mostrarLetreiro(quadro);
        esperar(250);
    }//for
    somVitoria();

    printf("\n%s       ", COR_AMARELO);
    escreverDevagar("*** PUZZLES E RACIOCINIO LOGICO ***", 35);
    printf("%s\n\n", COR_RESET);
    printf("%s         Equipe Rockstar Fundamentos - UTFPR%s\n\n", COR_CINZA, COR_RESET);
    esperar(600);
}//telaAbertura

void suspense(void){
    int i;

    printf("\n%sVamos ver", COR_AMARELO);
    for (i = 0; i < 3; i++){
        esperar(500);
        printf(" .");
        somAviso();
    }//for
    printf("%s\n", COR_RESET);
    esperar(400);
}//suspense

void revelarResposta(Pergunta p, char escolhida, int largura){
    char texto[TAM_ALTERNATIVA + 20];
    int i;
    char letra;

    caixaBorda(largura, COR_AZUL);
    for (i = 0; i < 4; i++){
        letra = (char)('A' + i);
        if (letra == p.correta){
            sprintf(texto, "%c) %s  <<< CERTA", letra, p.alternativas[i]);
            caixaLinha(texto, COR_VERDE, largura, COR_AZUL);
        } else if (letra == escolhida){
            sprintf(texto, "%c) %s  <<< SUA RESPOSTA", letra, p.alternativas[i]);
            caixaLinha(texto, COR_VERMELHO, largura, COR_AZUL);
        } else {
            sprintf(texto, "%c) %s", letra, p.alternativas[i]);
            caixaLinha(texto, COR_CINZA, largura, COR_AZUL);
        }//if
    }//for
    caixaBorda(largura, COR_AZUL);
}//revelarResposta

void comemorarVitoria(void){
    const char *cores[4] = {COR_AMARELO, COR_VERDE, COR_CIANO, COR_ROXO};
    const char simbolos[3] = {'*', '+', 'o'};
    int i;

    //simbolos em posições aleatorias
    limparTela();
    for (i = 0; i < QUADROS_CHUVA; i++){
        moverCursor(1 + rand() % 20, 1 + rand() % 70);
        printf("%s%c%s", cores[rand() % 4], simbolos[rand() % 3], COR_RESET);
        esperar(30);
    }//for

    moverCursor(9, 18);
    printf("%s+----------------------------------+%s", COR_AMARELO, COR_RESET);
    moverCursor(10, 18);
    printf("%s|  VOCE ACERTOU AS 15 PERGUNTAS!   |%s", COR_AMARELO, COR_RESET);
    moverCursor(11, 18);
    printf("%s+----------------------------------+%s", COR_AMARELO, COR_RESET);
    desenharLogica(LOGICA_EM_FESTA, 13, 26);
    moverCursor(22, 1);
    somVitoria();
    somVitoria();
    printf("\n");
}//comemorarVitoria

/**
 * Desenhos do Lógica em ASCII. Cada pose tem 7 linhas.
 * (A barra invertida aparece como \\ dentro das aspas.)
 * @param humor humor do cachorro
 * @param linha linha do desenho (0 a 6)
 * @return texto da linha
 */
static const char *linhaDaLogica(Humor humor, int linha){
    static const char *poses[6][7] = {
        {   //dormindo
            "             z Z",
            "   /^---^\\    z",
            "  ( [-]-[-] )",
            "   \\  (_)  /",
            "  __\\ --- /______",
            " (__|_____|______)~",
            ""
        },
        {   //acordado
            "",
            "   /^---^\\",
            "  ( [o]-[o] )",
            "   \\  (_)  /",
            "  __\\ --- /______",
            " (__|_____|______)~",
            ""
        },
        {   //animado (sentado)
            "   /^---^\\",
            "  ( [o]-[o] )",
            "   \\  (_)  /",
            "    \\ \\_/ /",
            "    /     \\   ~",
            "   |  | |  |_/",
            "   (__) (__)"
        },
        {   //feliz (em pe)
            "   /^---^\\     \\\\",
            "  ( [^]-[^] )   ||",
            "   \\  (_)  /____//",
            "    \\ \\U/ /     |",
            "     \\_________/",
            "      ||  ||  ||",
            "      ^^  ^^  ^^"
        },
        {   //em festa
            " *   /^---^\\   *",
            "    ( [^]-[^] )",
            " +   \\  (_)  /  +",
            "      \\ \\U/ /",
            "  *   \\|   |/  *",
            "       |   |",
            "      /     \\"
        },
        {   //triste
            "",
            "   /v---v\\",
            "  ( [;]-[;] )",
            "   \\  (_)  /",
            "  __\\ /^\\ /______",
            " (__|_____|______)",
            ""
        }
    };

    return poses[humor][linha];
}//linhaDaLogica

void desenharLogica(Humor humor, int linha, int coluna){
    const char *cores[6] = {COR_CINZA, COR_BRANCO, COR_CIANO, COR_VERDE, COR_AMARELO, COR_AZUL};
    int i;

    for (i = 0; i < 7; i++){
        if (linha > 0){
            moverCursor(linha + i, coluna);
        }//if
        printf("%s%s%s", cores[humor], linhaDaLogica(humor, i), COR_RESET);
        if (linha <= 0){
            printf("\n");
        }//if
    }//for
}//desenharLogica
