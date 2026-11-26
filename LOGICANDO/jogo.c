/**
 * @file jogo.c
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Implementação da partida do LOGICANDO.
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "jogo.h"
#include "ajuda.h"
#include "tela.h"
#include "util.h"
#include "visual.h"

#define LARGURA_CAIXA 58       /**< largura das caixas da tela da pergunta */
#define COLUNA_LOGICA 61       /**< coluna onde o Lógica é desenhado */
#define LARGURA_FIM 50         /**< largura da caixa do resumo final */
#define MAX_USADAS (TOTAL_ETAPAS + MAX_PULOS)

int nivelDaEtapa(int etapa){
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

long pontosDosAcertos(int acertos){
    return (long)acertos * PONTOS_POR_ACERTO;
}//pontosDosAcertos

/**
 * Pontos garantidos pelos marcos de segurança (perguntas 5 e 10).
 * @param acertos quantidade de perguntas já acertadas
 * @return pontos que ficam garantidos
 */
static long pontosGarantidos(int acertos){
    if (acertos >= 10){
        return pontosDosAcertos(10);
    } else if (acertos >= 5){
        return pontosDosAcertos(5);
    }//if
    return 0;
}//pontosGarantidos

long pontosAoErrar(int etapa, int acertos){
    long metade = pontosDosAcertos(acertos) / 2;
    long garantido = pontosGarantidos(acertos);

    if (etapa == TOTAL_ETAPAS){
        return 0;
    }//if
    if (metade > garantido){
        return metade;
    }//if
    return garantido;
}//pontosAoErrar

Humor humorDaLogica(int acertos){
    if (acertos == 0){
        return LOGICA_DORMINDO;
    } else if (acertos < 5){
        return LOGICA_ACORDADO;
    } else if (acertos < 10){
        return LOGICA_ANIMADO;
    } else if (acertos < TOTAL_ETAPAS){
        return LOGICA_FELIZ;
    }//if
    return LOGICA_EM_FESTA;
}//humorDaLogica

const char *textoDoHumor(Humor humor){
    switch (humor){
        case LOGICA_DORMINDO: return "DORMINDO";
        case LOGICA_ACORDADO: return "ACORDOU";
        case LOGICA_ANIMADO:  return "ANIMADO";
        case LOGICA_FELIZ:    return "MUITO FELIZ";
        case LOGICA_EM_FESTA: return "EM FESTA!";
        default:              return "TRISTE";
    }//switch
}//textoDoHumor

/**
 * Confere se existem perguntas suficientes em cada nível para uma partida.
 * @param lista endereço da lista
 * @return 1 se dá para jogar, 0 caso contrário
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
            mensagemErro("o nivel %s precisa de %d pergunta(s), mas tem so %d.",
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
 * @param indice posição da pergunta no vetor
 * @param usadas vetor com as posições das perguntas que já saíram
 * @param quantidade quantas posições de usadas estão preenchidas
 * @return 1 se já foi usada, 0 caso contrário
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

int sortearPergunta(const Lista *lista, int nivel, const int usadas[], int quantidade){
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

/**
 * Desenha, no lado direito da tela, o cachorro Lógica e o progresso da
 * partida. A barra mostra um '#' por acerto, '>' na pergunta atual e
 * '|' nos marcos de segurança (perguntas 5 e 10).
 * No final, o cursor volta ao topo.
 * @param etapa etapa atual (1 a 15)
 * @param acertos perguntas acertadas até agora
 */
static void desenharPainelLogica(int etapa, int acertos){
    Humor humor = humorDaLogica(acertos);
    char barra[TOTAL_ETAPAS + 5];
    int n = 0;
    int i;

    moverCursor(1, COLUNA_LOGICA);
    printf("%s LOGICA%s", COR_CIANO, COR_RESET);
    desenharLogica(humor, 3, COLUNA_LOGICA);
    moverCursor(11, COLUNA_LOGICA);
    printf("%s %s%s", COR_AMARELO, textoDoHumor(humor), COR_RESET);

    barra[n++] = '[';
    for (i = 1; i <= TOTAL_ETAPAS; i++){
        if (i <= acertos){
            barra[n++] = '#';
        } else if (i == etapa){
            barra[n++] = '>';
        } else {
            barra[n++] = '.';
        }//if
        if (i == 5 || i == 10){
            barra[n++] = '|';
        }//if
    }//for
    barra[n++] = ']';
    barra[n] = '\0';

    moverCursor(13, COLUNA_LOGICA);
    printf("ACERTOS %d/%d", acertos, TOTAL_ETAPAS);
    moverCursor(14, COLUNA_LOGICA);
    printf("%s%s%s", COR_VERDE, barra, COR_RESET);
    moverCursor(15, COLUNA_LOGICA);
    printf("PONTOS  %ld", pontosDosAcertos(acertos));
    moverCursor(17, COLUNA_LOGICA);
    printf("%s | marco de seguranca%s", COR_CINZA, COR_RESET);
    moverCursor(1, 1);
}//desenharPainelLogica

/**
 * Mostra as ajudas disponíveis dentro da caixa e monta o texto com as
 * opções válidas (que será usado no lerLetra).
 * @param ajudas ajudas restantes
 * @param eliminadas alternativas eliminadas (não podem ser escolhidas)
 * @param podeAjudar 0 na última pergunta (sem ajudas), 1 nas outras
 * @param validas vetor (mínimo 8 posições) que recebe as opções válidas
 */
static void montarOpcoes(const Ajudas *ajudas, const int eliminadas[4],
                         int podeAjudar, char validas[]){
    char linha[LARGURA_CAIXA];
    char texto[30];
    int n = 0;
    int i;

    validas[n++] = 'P';  //opção padrao
    for (i = 0; i < 4; i++){
        if (!eliminadas[i]){
            validas[n++] = (char)('A' + i);
        }//if
    }//for

    strcpy(linha, "AJUDAS: ");
    if (podeAjudar){
        if (ajudas->pulos > 0){
            sprintf(texto, "[1] Pular (%d)   ", ajudas->pulos);
            strcat(linha, texto);
            validas[n++] = '1';
        }//if
        if (ajudas->cartas > 0){
            strcat(linha, "[2] Cartas");
            validas[n++] = '2';
        }//if
    }//if
    validas[n] = '\0';

    if (!podeAjudar){
        caixaLinha("AJUDAS: nao valem na ultima pergunta!", COR_VERMELHO, LARGURA_CAIXA, COR_AZUL);
    } else if (strchr(validas, '1') == NULL && strchr(validas, '2') == NULL){
        caixaLinha("AJUDAS: todas ja foram usadas", COR_CINZA, LARGURA_CAIXA, COR_AZUL);
    } else {
        caixaLinha(linha, COR_ROXO, LARGURA_CAIXA, COR_AZUL);
    }//if
    caixaLinha("[A-D] Responder      [P] Parar e levar os pontos", COR_BRANCO, LARGURA_CAIXA, COR_AZUL);
}//montarOpcoes

/**
 * Desenha a tela da pergunta: cabeçalho, enunciado, alternativas,
 * pontos e ajudas, tudo dentro de caixas.
 * @param etapa etapa atual (1 a 15)
 * @param nome nome do jogador
 * @param acertos perguntas acertadas até agora
 * @param p pergunta atual
 * @param eliminadas alternativas eliminadas pelas cartas
 * @param ajudas ajudas restantes
 * @param validas vetor que recebe as opções válidas para o lerLetra
 */
static void desenharPergunta(int etapa, const char *nome, int acertos, Pergunta p,
                             const int eliminadas[4], const Ajudas *ajudas, char validas[]){
    char texto[LARGURA_CAIXA + TAM_ALTERNATIVA];
    int i;

    caixaBorda(LARGURA_CAIXA, COR_AZUL);
    sprintf(texto, "PERGUNTA %d DE %d  -  %s", etapa, TOTAL_ETAPAS, nomeNivel(p.nivel));
    caixaLinha(texto, corDoNivel(p.nivel), LARGURA_CAIXA, COR_AZUL);
    sprintf(texto, "Jogador: %s", nome);
    caixaLinha(texto, COR_CINZA, LARGURA_CAIXA, COR_AZUL);
    caixaBorda(LARGURA_CAIXA, COR_AZUL);

    caixaTexto(p.enunciado, COR_BRANCO, LARGURA_CAIXA, COR_AZUL);
    caixaBorda(LARGURA_CAIXA, COR_AZUL);

    for (i = 0; i < 4; i++){
        if (eliminadas[i]){
            sprintf(texto, "%c) ----------", 'A' + i);
            caixaLinha(texto, COR_CINZA, LARGURA_CAIXA, COR_AZUL);
        } else {
            sprintf(texto, "%c) %s", 'A' + i, p.alternativas[i]);
            caixaLinha(texto, COR_CIANO, LARGURA_CAIXA, COR_AZUL);
        }//if
    }//for
    caixaBorda(LARGURA_CAIXA, COR_AZUL);

    sprintf(texto, "PONTOS  Parar: %ld  |  Errar: %ld  |  Acertar: %ld",
            pontosDosAcertos(acertos), pontosAoErrar(etapa, acertos), pontosDosAcertos(acertos + 1));
    caixaLinha(texto, COR_BRANCO, LARGURA_CAIXA, COR_AZUL);
    if (etapa == 5 || etapa == 10){
        caixaLinha("MARCO DE SEGURANCA: acertando, os pontos ficam garantidos!", COR_VERDE, LARGURA_CAIXA, COR_AZUL);
    } else if (etapa == TOTAL_ETAPAS){
        caixaLinha("ULTIMA PERGUNTA! Se errar, voce perde tudo!", COR_VERMELHO, LARGURA_CAIXA, COR_AZUL);
    }//if
    montarOpcoes(ajudas, eliminadas, etapa < TOTAL_ETAPAS, validas);
    caixaBorda(LARGURA_CAIXA, COR_AZUL);
}//desenharPergunta

void jogar(const Lista *lista, Ranking *ranking){
    int usadas[MAX_USADAS];
    int quantidadeUsadas = 0;
    int eliminadas[4];
    int etapa = 1;
    int acertos = 0;
    int terminou = 0;
    int errou = 0;
    int respondeu;
    int indice;
    int novo;
    int recorde;
    int sequencia = 0;
    int bonus = 0;
    int segundos;
    int telaNova;
    long base = 0;
    char resposta;
    char validas[8];
    char texto[LARGURA_FIM];
    time_t inicioPartida;
    time_t inicioPergunta;
    Ajudas ajudas;
    Resultado resultado;
    Pergunta p;

    mostrarTitulo("LOGICANDO - PUZZLES E RACIOCINIO LOGICO", COR_AMARELO);
    if (!temPerguntasSuficientes(lista)){
        return;
    }//if

    memset(&resultado, 0, sizeof(Resultado));
    if (!lerTextoObrigatorio("Qual e o seu nome? ", resultado.nome, TAM_NOME)){
        return;
    }//if

    recorde = recordePessoal(ranking, resultado.nome);
    if (recorde >= 0){
        printf("Bem-vindo de volta, %s! Seu recorde e %s%d pontos%s. Tente superar!\n",
               resultado.nome, COR_AMARELO, recorde, COR_RESET);
    } else {
        printf("Bem-vindo, %s! Esta e a sua primeira partida.\n", resultado.nome);
    }//if

    printf("\nResponda 15 perguntas. Cada acerto vale %d pontos.\n", PONTOS_POR_ACERTO);
    printf("O Logica, o cachorro de oculos, esta dormindo. Acerte para deixa-lo feliz!\n");
    printf("- Digite A, B, C ou D para responder, ou P para parar e levar os pontos.\n");
    printf("- Errar: voce leva metade dos pontos (ou o marco de seguranca).\n");
    printf("- Acerte %d seguidas em menos de %d s cada e ganhe +%d pontos de bonus!\n",
           SEQUENCIA_BONUS, LIMITE_RAPIDO, PONTOS_BONUS);
    pausar();

    iniciarAjudas(&ajudas);
    inicioPartida = time(NULL);

    while (etapa <= TOTAL_ETAPAS && !terminou){
        indice = sortearPergunta(lista, nivelDaEtapa(etapa), usadas, quantidadeUsadas);
        if (indice < 0){
            mensagemErro("nao ha perguntas suficientes para continuar!");
            return;
        }//if
        usadas[quantidadeUsadas++] = indice;
        memset(eliminadas, 0, sizeof(eliminadas));
        inicioPergunta = time(NULL);

        respondeu = 0;
        telaNova = 1;
        while (!respondeu){
            //limpa a tela e desenha o logica
            if (telaNova){
                limparTela();
                desenharPainelLogica(etapa, acertos);
                telaNova = 0;
            }//if
            p = lista->itens[indice];
            desenharPergunta(etapa, resultado.nome, acertos, p, eliminadas, &ajudas, validas);
            resposta = lerLetra("Sua resposta: ", validas);

            if (resposta == '1'){
                novo = sortearPergunta(lista, nivelDaEtapa(etapa), usadas, quantidadeUsadas);
                if (novo < 0){
                    mensagemAviso("Nao ha outra pergunta deste nivel para trocar.");
                } else {
                    ajudas.pulos--;
                    indice = novo;
                    usadas[quantidadeUsadas++] = indice;
                    memset(eliminadas, 0, sizeof(eliminadas));
                    mensagemAviso("Pergunta pulada! Restam %d pulo(s).", ajudas.pulos);
                    esperar(900);
                    telaNova = 1;
                }//if
            } else if (resposta == '2'){
                ajudas.cartas--;
                usarCartas(p, eliminadas);
            } else if (resposta == 'P'){
                if (lerLetra("Quer mesmo parar? (S/N): ", "SN") == 'S'){
                    respondeu = 1;
                    terminou = 1;
                    base = pontosDosAcertos(acertos);
                    printf("\nVoce parou!\n");
                }//if
            } else if (lerLetra("Voce esta certo disso? (S/N): ", "SN") == 'S'){
                respondeu = 1;
                segundos = (int)difftime(time(NULL), inicioPergunta);

                suspense();
                revelarResposta(p, resposta, LARGURA_CAIXA);

                if (resposta == p.correta){
                    acertos++;
                    mensagemSucesso("CERTA RESPOSTA! (%d s)", segundos);

                    //bonus de sequencia
                    if (segundos < LIMITE_RAPIDO){
                        sequencia++;
                    } else {
                        sequencia = 0;
                    }//if
                    if (sequencia == SEQUENCIA_BONUS){
                        bonus += PONTOS_BONUS;
                        sequencia = 0;
                        printf("%s*** BONUS! %d acertos rapidos seguidos: +%d pontos ***%s\n",
                               COR_AMARELO, SEQUENCIA_BONUS, PONTOS_BONUS, COR_RESET);
                        somVitoria();
                    }//if

                    //logica mudou de humor
                    if (humorDaLogica(acertos) != humorDaLogica(acertos - 1)){
                        printf("%sO Logica esta %s%s\n", COR_CIANO,
                               textoDoHumor(humorDaLogica(acertos)), COR_RESET);
                    }//if

                    etapa++;
                    if (etapa <= TOTAL_ETAPAS){
                        pausar();
                    }//if
                } else {
                    base = pontosAoErrar(etapa, acertos);
                    terminou = 1;
                    errou = 1;
                    mensagemErro("que pena, resposta errada! A correta era a letra %c.", p.correta);
                }//if
            }//if
        }//while
    }//while

    if (!terminou){
        base = pontosDosAcertos(TOTAL_ETAPAS);
        comemorarVitoria();
    }//if

    //resultado da partida
    resultado.segundos = (int)difftime(time(NULL), inicioPartida);
    resultado.acertos = acertos;
    resultado.base = base;
    resultado.bonus = bonus;
    resultado.pontos = calcularPontos(base, bonus, resultado.segundos);

    //desenha o logica no final
    printf("\n");
    desenharLogica(errou ? LOGICA_TRISTE : humorDaLogica(acertos), 0, 0);

    //resumo da partida
    printf("\n");
    caixaBorda(LARGURA_FIM, COR_AMARELO);
    caixaLinha("FIM DE JOGO", COR_AMARELO, LARGURA_FIM, COR_AMARELO);
    caixaBorda(LARGURA_FIM, COR_AMARELO);
    sprintf(texto, "Perguntas acertadas: %d", acertos);
    caixaLinha(texto, COR_BRANCO, LARGURA_FIM, COR_AMARELO);
    sprintf(texto, "Pontos dos acertos:  %ld", base);
    caixaLinha(texto, COR_VERDE, LARGURA_FIM, COR_AMARELO);
    sprintf(texto, "Bonus de sequencia:  +%d", bonus);
    caixaLinha(texto, COR_BRANCO, LARGURA_FIM, COR_AMARELO);
    sprintf(texto, "Tempo total:         %dm%02ds (-%d)", resultado.segundos / 60,
            resultado.segundos % 60, resultado.segundos / 10);
    caixaLinha(texto, COR_BRANCO, LARGURA_FIM, COR_AMARELO);
    sprintf(texto, "PONTUACAO:           %d pontos", resultado.pontos);
    caixaLinha(texto, COR_AMARELO, LARGURA_FIM, COR_AMARELO);
    sprintf(texto, "Logica:              %s", textoDoHumor(errou ? LOGICA_TRISTE : humorDaLogica(acertos)));
    caixaLinha(texto, COR_CIANO, LARGURA_FIM, COR_AMARELO);
    if (recorde >= 0 && resultado.pontos > recorde){
        sprintf(texto, "*** NOVO RECORDE PESSOAL! (antes: %d) ***", recorde);
        caixaLinha(texto, COR_VERDE, LARGURA_FIM, COR_AMARELO);
    }//if
    caixaBorda(LARGURA_FIM, COR_AMARELO);

    if (recorde >= 0 && resultado.pontos > recorde){
        somVitoria();
    }//if

    if (!adicionarResultado(ranking, resultado)){
        mensagemErro("memoria insuficiente para guardar o resultado no ranking!");
    }//if
}//jogar
