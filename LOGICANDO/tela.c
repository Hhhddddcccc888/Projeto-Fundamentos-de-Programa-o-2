/**
 * @file tela.c
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Implementação das cores, sons e mensagens do terminal.
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/* necessário no Linux para usar o nanosleep com -std=c99 */
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "tela.h"
#include "pergunta.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

/* alguns compiladores antigos (MinGW) não definem essa constante */
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#define LARGURA_TITULO 46

void iniciarTela(void){
#ifdef _WIN32
    HANDLE saida = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo = 0;

    if (saida != INVALID_HANDLE_VALUE && GetConsoleMode(saida, &modo)){
        SetConsoleMode(saida, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }//if
#endif
}//iniciarTela

void limparTela(void){
    printf("\033[2J\033[H");
    fflush(stdout);
}//limparTela

/**
 * Toca um beep com uma frequência e duração.
 * No Windows usa a função Beep; nos outros sistemas usa o caractere '\a'.
 * @param frequencia frequência em Hz
 * @param duracao duração em milissegundos
 */
static void tocar(int frequencia, int duracao){
#ifdef _WIN32
    Beep((DWORD)frequencia, (DWORD)duracao);
#else
    (void)frequencia;
    (void)duracao;
    printf("\a");
    fflush(stdout);
#endif
}//tocar

void somAcerto(void){
    tocar(880, 90);
    tocar(1320, 140);
}//somAcerto

void somErro(void){
    tocar(300, 250);
}//somErro

void somAviso(void){
    tocar(660, 120);
}//somAviso

void somVitoria(void){
    tocar(523, 150);
    tocar(659, 150);
    tocar(784, 150);
    tocar(1047, 400);
}//somVitoria

/**
 * Mostra uma mensagem colorida (parte comum das funções mensagemXxx).
 * @param cor código ANSI da cor
 * @param prefixo texto mostrado antes da mensagem (ex: "Erro: ")
 * @param formato texto no formato do printf
 * @param argumentos argumentos do formato
 */
static void mensagemColorida(const char *cor, const char *prefixo,
                             const char *formato, va_list argumentos){
    printf("%s%s", cor, prefixo);
    vprintf(formato, argumentos);
    printf("%s\n", COR_RESET);
    fflush(stdout);
}//mensagemColorida

void mensagemErro(const char *formato, ...){
    va_list argumentos;

    va_start(argumentos, formato);
    mensagemColorida(COR_VERMELHO, "[X] Erro: ", formato, argumentos);
    va_end(argumentos);
    somErro();
}//mensagemErro

void mensagemSucesso(const char *formato, ...){
    va_list argumentos;

    va_start(argumentos, formato);
    mensagemColorida(COR_VERDE, "[OK] ", formato, argumentos);
    va_end(argumentos);
    somAcerto();
}//mensagemSucesso

void mensagemAviso(const char *formato, ...){
    va_list argumentos;

    va_start(argumentos, formato);
    mensagemColorida(COR_AMARELO, "[!] ", formato, argumentos);
    va_end(argumentos);
    somAviso();
}//mensagemAviso

const char *corDoNivel(int nivel){
    switch (nivel){
        case MUITO_FACIL:   return COR_VERDE;
        case FACIL:         return COR_CIANO;
        case MEDIO:         return COR_AMARELO;
        case DIFICIL:       return COR_ROXO;
        case MUITO_DIFICIL: return COR_VERMELHO;
        default:            return COR_BRANCO;
    }//switch
}//corDoNivel

void mostrarTitulo(const char *titulo, const char *cor){
    int tamanho = (int)strlen(titulo);
    int esquerda = (LARGURA_TITULO - tamanho) / 2;
    int i;

    if (esquerda < 0){
        esquerda = 0;
    }//if

    printf("\n%s", cor);
    for (i = 0; i < LARGURA_TITULO; i++){
        printf("=");
    }//for
    printf("\n%*s%s\n", esquerda, "", titulo);
    for (i = 0; i < LARGURA_TITULO; i++){
        printf("=");
    }//for
    printf("%s\n", COR_RESET);
}//mostrarTitulo

void esperar(int milissegundos){
    fflush(stdout);  //mostra o texto antes de esperar
#ifdef _WIN32
    Sleep((DWORD)milissegundos);
#else
    struct timespec tempo;

    tempo.tv_sec = milissegundos / 1000;
    tempo.tv_nsec = (milissegundos % 1000) * 1000000L;
    nanosleep(&tempo, NULL);
#endif
}//esperar

void moverCursor(int linha, int coluna){
    printf("\033[%d;%dH", linha, coluna);
}//moverCursor

void caixaBorda(int largura, const char *corBorda){
    int i;

    printf("%s+", corBorda);
    for (i = 0; i < largura - 2; i++){
        printf("-");
    }//for
    printf("+%s\n", COR_RESET);
}//caixaBorda

/**
 * Desenha uma linha da caixa com um pedaço do texto.
 * @param texto início do pedaço
 * @param tamanho quantos caracteres do texto serão mostrados
 * @param corTexto cor do texto
 * @param largura largura total da caixa
 * @param corBorda cor da borda
 */
static void caixaPedaco(const char *texto, int tamanho, const char *corTexto,
                        int largura, const char *corBorda){
    int interno = largura - 4;  //tamanho sem as bordas

    if (tamanho > interno){
        tamanho = interno;
    }//if
    //escreve o texto e completa com espaços
    printf("%s|%s %s%-*.*s%s %s|%s\n", corBorda, COR_RESET, corTexto,
           interno, tamanho, texto, COR_RESET, corBorda, COR_RESET);
}//caixaPedaco

void caixaLinha(const char *texto, const char *corTexto, int largura, const char *corBorda){
    caixaPedaco(texto, (int)strlen(texto), corTexto, largura, corBorda);
}//caixaLinha

void caixaTexto(const char *texto, const char *corTexto, int largura, const char *corBorda){
    int interno = largura - 4;
    int tamanho = (int)strlen(texto);
    int inicio = 0;
    int fim;
    int corte;

    while (inicio < tamanho){
        fim = inicio + interno;
        if (fim >= tamanho){
            fim = tamanho;
        } else {
            //nao corta a palavra no meio
            corte = fim;
            while (corte > inicio && texto[corte] != ' '){
                corte--;
            }//while
            if (corte > inicio){
                fim = corte;
            }//if
        }//if

        caixaPedaco(texto + inicio, fim - inicio, corTexto, largura, corBorda);

        inicio = fim;
        while (texto[inicio] == ' '){
            inicio++;
        }//while
    }//while
}//caixaTexto

void escreverDevagar(const char *texto, int atraso){
    int i;

    for (i = 0; texto[i] != '\0'; i++){
        putchar(texto[i]);
        esperar(atraso);
    }//for
}//escreverDevagar
