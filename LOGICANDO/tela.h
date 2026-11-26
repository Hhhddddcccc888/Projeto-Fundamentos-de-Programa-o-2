/**
 * @file tela.h
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Cores, sons (beeps) e mensagens coloridas do terminal.
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 * As cores usam os códigos ANSI ("\033[...m"). No Windows 10 o terminal
 * precisa ser avisado para entender esses códigos (ver iniciarTela).
 */

#ifndef TELA_H
#define TELA_H

/* Códigos de cor ANSI */
#define COR_RESET    "\033[0m"
#define COR_NEGRITO  "\033[1m"
#define COR_VERMELHO "\033[1;31m"
#define COR_VERDE    "\033[1;32m"
#define COR_AMARELO  "\033[1;33m"
#define COR_AZUL     "\033[1;34m"
#define COR_ROXO     "\033[1;35m"
#define COR_CIANO    "\033[1;36m"
#define COR_BRANCO   "\033[1;37m"
#define COR_CINZA    "\033[0;90m"

/**
 * Prepara o terminal para mostrar cores (no Windows ativa os códigos ANSI).
 * Deve ser chamada uma vez no início do programa.
 */
void iniciarTela(void);

/**
 * Limpa a tela e coloca o cursor no canto superior esquerdo.
 */
void limparTela(void);

/**
 * Som curto e agudo: resposta certa / operação concluída.
 */
void somAcerto(void);

/**
 * Som grave: resposta errada / erro do usuário.
 */
void somErro(void);

/**
 * Som de aviso (fim da lista, ajuda usada, etc.).
 */
void somAviso(void);

/**
 * Pequena "musiquinha" de vitória.
 */
void somVitoria(void);

/**
 * Mostra uma mensagem de erro em vermelho e toca o som de erro.
 * Funciona como o printf (aceita %d, %s...). Já pula linha no final.
 * @param formato texto no formato do printf
 */
void mensagemErro(const char *formato, ...);

/**
 * Mostra uma mensagem de sucesso em verde e toca o som de acerto.
 * @param formato texto no formato do printf
 */
void mensagemSucesso(const char *formato, ...);

/**
 * Mostra uma mensagem de aviso em amarelo e toca o som de aviso.
 * @param formato texto no formato do printf
 */
void mensagemAviso(const char *formato, ...);

/**
 * Retorna a cor usada para cada nível de dificuldade.
 * @param nivel nível (1 a 5)
 * @return código ANSI da cor
 */
const char *corDoNivel(int nivel);

/**
 * Mostra um título centralizado dentro de uma moldura colorida.
 * @param titulo texto do título
 * @param cor código ANSI da cor da moldura
 */
void mostrarTitulo(const char *titulo, const char *cor);

/**
 * Pausa o programa por um tempo (usado nas animações).
 * @param milissegundos tempo de espera
 */
void esperar(int milissegundos);

/**
 * Move o cursor para uma posição da tela (a primeira linha/coluna é 1).
 * @param linha linha de destino
 * @param coluna coluna de destino
 */
void moverCursor(int linha, int coluna);

/**
 * Desenha a borda de cima/baixo de uma caixa: +--------+
 * @param largura largura total da caixa
 * @param corBorda cor da borda
 */
void caixaBorda(int largura, const char *corBorda);

/**
 * Desenha uma linha dentro da caixa: | texto    |
 * Se o texto for maior que a caixa, ele é cortado.
 * @param texto texto da linha (sem códigos de cor)
 * @param corTexto cor do texto
 * @param largura largura total da caixa
 * @param corBorda cor da borda
 */
void caixaLinha(const char *texto, const char *corTexto, int largura, const char *corBorda);

/**
 * Desenha um texto longo dentro da caixa, quebrando as linhas
 * nos espaços para não cortar as palavras no meio.
 * @param texto texto (sem códigos de cor)
 * @param corTexto cor do texto
 * @param largura largura total da caixa
 * @param corBorda cor da borda
 */
void caixaTexto(const char *texto, const char *corTexto, int largura, const char *corBorda);

/**
 * Escreve um texto letra por letra, como uma máquina de escrever.
 * @param texto texto que será escrito
 * @param atraso tempo (em milissegundos) entre uma letra e outra
 */
void escreverDevagar(const char *texto, int atraso);

#endif /* TELA_H */
