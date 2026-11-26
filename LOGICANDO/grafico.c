/**
 * @file grafico.c
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Versão gráfica do LOGICANDO, feita com a biblioteca raylib.
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 * Visual minimalista em preto e branco, com a fonte pixelada da raylib,
 * linhas de "TV antiga" e sons de videogame (ondas quadradas).
 * No lugar da escada de prêmios fica o Lógica, um cachorro de óculos
 * desenhado em pixel art: ele começa deitado dormindo e vai ficando mais
 * feliz a cada acerto (acorda, senta, levanta e pula de alegria).
 *
 * Usa as mesmas regras (jogo.c e ajuda.c) e os mesmos arquivos da versão de
 * terminal: perguntas.bin / perguntas.csv e ranking.bin. Uma partida jogada
 * aqui aparece no ranking da versão de terminal, e vice-versa.
 * O cadastro de perguntas (inserir, alterar, excluir...) continua sendo
 * feito na versão de terminal (logicando.exe).
 *
 * Controles: mouse, ou o teclado (A-D responde, 1-2 ajudas, P para,
 * S/N confirma, Enter continua, Esc volta).
 *
 * Compilar e rodar: make grafico-run   (no Windows: mingw32-make grafico-run)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "raylib.h"
#include "pergunta.h"
#include "arquivo.h"
#include "ranking.h"
#include "ajuda.h"
#include "jogo.h"

#define LARGURA_JANELA 1280
#define ALTURA_JANELA 720
#define MAX_USADAS (TOTAL_ETAPAS + MAX_PULOS)
#define QTD_CONFETES 120
#define DURACAO_SUSPENSE 2.5    /**< segundos de suspense antes de revelar a resposta */
#define TAM_MENSAGEM 160
#define TAM_TEXTO 512
#define BORDA 4                 /**< espessura das bordas, em pixels */
#define SOMBRA 6                /**< deslocamento da "sombra" das caixas */
#define TAM_PIXEL 7             /**< tamanho de cada "pixel" do desenho do Lógica */

/* Só três tons: preto, branco e cinza */
#define PRETO        (Color){ 0, 0, 0, 255 }
#define BRANCO       (Color){ 245, 245, 245, 255 }
#define CINZA        (Color){ 120, 120, 120, 255 }
#define CINZA_ESCURO (Color){ 45, 45, 45, 255 }

/** Telas do programa */
typedef enum {
    TELA_MENU,
    TELA_NOME,
    TELA_JOGO,
    TELA_FIM,
    TELA_RANKING,
    TELA_SAIR
} Tela;//enum

/** Momentos de uma pergunta */
typedef enum {
    FASE_PERGUNTA,   /**< esperando a resposta ou uma ajuda */
    FASE_CARTAS,     /**< escolhendo uma das 4 cartas */
    FASE_CONFIRMAR,  /**< "Voce esta certo disso?" */
    FASE_PARAR,      /**< "Quer mesmo parar?" */
    FASE_SUSPENSE,   /**< alguns segundos antes de revelar */
    FASE_REVELADA    /**< mostra se acertou ou errou */
} Fase;//enum

/** Posições do corpo do Lógica */
typedef enum {
    POSE_DEITADO,
    POSE_SENTADO,
    POSE_EM_PE
} Pose;//enum

/** Sons do jogo, gerados pelo próprio programa (não precisa de arquivos de áudio) */
typedef struct {
    int ativo;      /**< 0 se o computador não tem saída de som */
    Sound acerto;
    Sound erro;
    Sound aviso;
    Sound vitoria;
} Sons;//struct

/** Recursos carregados uma vez só: fonte e sons */
typedef struct {
    Font fonte;
    Sons sons;
} Recursos;//struct

/** Um "pixel" que cai na comemoração */
typedef struct {
    Vector2 posicao;
    float velocidade;
    int tamanho;
} Confete;//struct

/** Tudo o que acontece em uma partida */
typedef struct {
    char nome[TAM_NOME];
    int recorde;                 /**< recorde do jogador antes da partida, -1 se nunca jogou */
    int usadas[MAX_USADAS];
    int quantidadeUsadas;
    int indice;                  /**< posição da pergunta atual na lista */
    int eliminadas[4];
    int etapa;
    int acertos;
    int sequencia;
    int bonus;
    long base;                   /**< pontos dos acertos que o jogador leva */
    Ajudas ajudas;
    Fase fase;
    char escolhida;
    double inicioPartida;
    double inicioPergunta;
    double inicioFase;
    int segundosResposta;
    int acertou;
    int errou;                   /**< 1 quando o jogador errou (o Lógica fica triste) */
    int baralho[4];              /**< 0 = Rei, 1 = Ás, 2 = dois, 3 = três */
    int cartaVirada;             /**< carta escolhida, -1 se ainda não escolheu */
    char mensagem[TAM_MENSAGEM];
    Resultado resultado;
    int acertouTudo;
    int novoRecorde;
    Confete confetes[QTD_CONFETES];
} Partida;//struct

/* ------------------------------------------------------------------ */
/* Sons                                                                 */
/* ------------------------------------------------------------------ */

/**
 * Cria um som de videogame antigo: uma sequência de notas em onda quadrada.
 * @param notas frequências das notas, em Hz
 * @param quantidade quantas notas
 * @param duracaoNota duração de cada nota, em segundos
 * @return o som pronto para tocar
 */
static Sound criarMelodia(const float notas[], int quantidade, float duracaoNota){
    const int taxa = 44100;
    int porNota = (int)(taxa * duracaoNota);
    int total = porNota * quantidade;
    short *dados = (short *)malloc(total * sizeof(short));
    Wave onda;
    Sound som;
    float envelope;
    float seno;
    int i;

    memset(&som, 0, sizeof(Sound));
    if (dados == NULL){
        return som;
    }//if

    for (i = 0; i < total; i++){
        //volume diminuindo
        envelope = 1.0f - 0.7f * (float)(i % porNota) / porNota;
        //onda quadrada
        seno = sinf(2.0f * PI * notas[i / porNota] * i / taxa);
        dados[i] = (short)((seno >= 0 ? 1 : -1) * 3500.0f * envelope);
    }//for

    onda.frameCount = (unsigned int)total;
    onda.sampleRate = taxa;
    onda.sampleSize = 16;
    onda.channels = 1;
    onda.data = dados;
    som = LoadSoundFromWave(onda);  //cria o som
    free(dados);
    return som;
}//criarMelodia

/**
 * Cria todos os sons do jogo.
 * @param sons endereço da estrutura de sons
 */
static void carregarSons(Sons *sons){
    const float acerto[2] = {660.0f, 990.0f};
    const float erro[3] = {330.0f, 247.0f, 165.0f};
    const float aviso[1] = {880.0f};
    const float vitoria[4] = {523.0f, 659.0f, 784.0f, 1047.0f};

    sons->ativo = IsAudioDeviceReady();
    if (!sons->ativo){
        return;
    }//if
    sons->acerto = criarMelodia(acerto, 2, 0.10f);
    sons->erro = criarMelodia(erro, 3, 0.18f);
    sons->aviso = criarMelodia(aviso, 1, 0.05f);
    sons->vitoria = criarMelodia(vitoria, 4, 0.12f);
}//carregarSons

/**
 * Libera a memória dos sons.
 * @param sons endereço da estrutura de sons
 */
static void liberarSons(Sons *sons){
    if (sons->ativo){
        UnloadSound(sons->acerto);
        UnloadSound(sons->erro);
        UnloadSound(sons->aviso);
        UnloadSound(sons->vitoria);
    }//if
}//liberarSons

/**
 * Toca um som, se o computador tiver saída de áudio.
 * @param sons endereço da estrutura de sons
 * @param som som que será tocado
 */
static void tocar(const Sons *sons, Sound som){
    if (sons->ativo){
        PlaySound(som);
    }//if
}//tocar

/* ------------------------------------------------------------------ */
/* Texto, caixas e botões                                               */
/* ------------------------------------------------------------------ */

/**
 * Escreve um texto com a fonte pixelada. Para os pixels ficarem nítidos,
 * o tamanho deve ser múltiplo de 10 (10, 20, 30, 40...).
 */
static void escrever(const Recursos *r, const char *texto, float x, float y, float tamanho, Color cor){
    DrawTextEx(r->fonte, texto, (Vector2){ (float)(int)x, (float)(int)y }, tamanho, tamanho / 10.0f, cor);
}//escrever

/**
 * Mede a largura de um texto, em pixels.
 */
static float larguraTexto(const Recursos *r, const char *texto, float tamanho){
    return MeasureTextEx(r->fonte, texto, tamanho, tamanho / 10.0f).x;
}//larguraTexto

/**
 * Escreve um texto centralizado em torno da coordenada x.
 */
static void escreverCentro(const Recursos *r, const char *texto, float x, float y, float tamanho, Color cor){
    escrever(r, texto, x - larguraTexto(r, texto, tamanho) / 2.0f, y, tamanho, cor);
}//escreverCentro

/**
 * Escreve um texto quebrando as linhas para caber na largura.
 * @param desenhar 0 para só medir a altura, sem desenhar
 * @return altura ocupada pelo texto, em pixels
 */
static float escreverQuebrado(const Recursos *r, const char *texto, float x, float y,
                              float largura, float tamanho, Color cor, int desenhar){
    char linha[TAM_TEXTO];
    char teste[TAM_TEXTO];
    char palavra[TAM_TEXTO];
    float altura = 0.0f;
    int i = 0;
    int n;

    linha[0] = '\0';
    while (texto[i] != '\0'){
        //separa a próxima palavra
        while (texto[i] == ' '){
            i++;
        }//while
        n = 0;
        while (texto[i] != '\0' && texto[i] != ' ' && n < TAM_TEXTO - 1){
            palavra[n++] = texto[i++];
        }//while
        palavra[n] = '\0';
        if (n == 0){
            break;
        }//if

        //tenta colocar a palavra na linha
        strcpy(teste, linha);
        if (strlen(linha) + strlen(palavra) + 2 <= TAM_TEXTO){
            if (teste[0] != '\0'){
                strcat(teste, " ");
            }//if
            strcat(teste, palavra);
        }//if

        if (linha[0] != '\0' && larguraTexto(r, teste, tamanho) > largura){
            //nao coube: pula a linha
            if (desenhar){
                escrever(r, linha, x, y + altura, tamanho, cor);
            }//if
            altura += tamanho * 1.4f;
            strcpy(linha, palavra);
        } else {
            strcpy(linha, teste);
        }//if
    }//while

    if (linha[0] != '\0'){
        if (desenhar){
            escrever(r, linha, x, y + altura, tamanho, cor);
        }//if
        altura += tamanho * 1.4f;
    }//if
    return altura;
}//escreverQuebrado

/**
 * Desenha uma caixa quadrada com borda grossa e uma "sombra" cinza
 * deslocada, no estilo dos jogos antigos.
 * @param fundo cor de dentro da caixa
 * @param borda cor da borda
 */
static void caixa(Rectangle area, Color fundo, Color borda){
    DrawRectangle((int)area.x + SOMBRA, (int)area.y + SOMBRA, (int)area.width, (int)area.height, CINZA_ESCURO);
    DrawRectangleRec(area, fundo);
    DrawRectangleLinesEx(area, BORDA, borda);
}//caixa

/**
 * Desenha um botão e diz se ele foi clicado. Com o mouse em cima, as
 * cores se invertem (fundo branco, letra preta).
 * @param ativo 0 deixa o botão cinza e sem clique
 * @return 1 se foi clicado neste quadro
 */
static int botao(const Recursos *r, Rectangle area, const char *rotulo, float tamanho, int ativo){
    int emCima = ativo && CheckCollisionPointRec(GetMousePosition(), area);
    Color fundo = emCima ? BRANCO : PRETO;
    Color frente = emCima ? PRETO : BRANCO;

    if (!ativo){
        frente = CINZA_ESCURO;
    }//if
    caixa(area, fundo, ativo ? BRANCO : CINZA_ESCURO);
    escreverCentro(r, rotulo, area.x + area.width / 2.0f, area.y + (area.height - tamanho) / 2.0f + 2,
                   tamanho, frente);
    return emCima && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}//botao

/**
 * Escurece a tela inteira (usado atrás das janelas de confirmação).
 */
static void escurecerTela(void){
    DrawRectangle(0, 0, LARGURA_JANELA, ALTURA_JANELA, Fade(PRETO, 0.8f));
}//escurecerTela

/**
 * Linhas horizontais escuras por cima de tudo, imitando uma TV antiga.
 */
static void desenharLinhasTV(void){
    int y;

    for (y = 0; y < ALTURA_JANELA; y += 3){
        DrawRectangle(0, y, LARGURA_JANELA, 1, Fade(PRETO, 0.25f));
    }//for
}//desenharLinhasTV

/**
 * Mostra o nível da pergunta como 5 quadradinhos: os cheios indicam a dificuldade.
 */
static void desenharNivel(int nivel, float x, float y){
    int i;

    for (i = 0; i < 5; i++){
        if (i < nivel){
            DrawRectangle((int)x + i * 22, (int)y, 16, 16, BRANCO);
        } else {
            DrawRectangleLinesEx((Rectangle){ x + i * 22, y, 16, 16 }, 2, CINZA);
        }//if
    }//for
}//desenharNivel

/**
 * Cursor piscando no estilo de terminal antigo.
 */
static int cursorAceso(void){
    return ((int)(GetTime() * 2)) % 2 == 0;
}//cursorAceso

/* ------------------------------------------------------------------ */
/* O cachorro Lógica (pixel art)                                        */
/* ------------------------------------------------------------------ */

/**
 * Pinta um retângulo de "pixels grandes" do desenho do Lógica.
 * As posições são contadas em pixels do desenho, a partir do chão:
 * x cresce para a direita e y negativo fica acima do chão.
 * @param chao ponto do chão onde o desenho começa (canto esquerdo)
 */
static void bloco(Vector2 chao, int x, int y, int largura, int altura, Color cor){
    DrawRectangle((int)chao.x + x * TAM_PIXEL, (int)chao.y + y * TAM_PIXEL,
                  largura * TAM_PIXEL, altura * TAM_PIXEL, cor);
}//bloco

/**
 * Desenha um coraçãozinho de 5x4 pixels.
 */
static void coracao(Vector2 chao, int x, int y, Color cor){
    bloco(chao, x + 1, y, 1, 1, cor);
    bloco(chao, x + 3, y, 1, 1, cor);
    bloco(chao, x, y + 1, 5, 1, cor);
    bloco(chao, x + 1, y + 2, 3, 1, cor);
    bloco(chao, x + 2, y + 3, 1, 1, cor);
}//coracao

/**
 * Desenha a cabeça do Lógica, de frente e de óculos (16 x 14 pixels).
 * Cada letra do desenho é um pixel: W = branco, G = cinza, K = preto
 * e '.' = vazio. Os olhos e a boca mudam conforme o humor.
 * @param x coluna (em pixels do desenho) do canto esquerdo da cabeça
 * @param y linha (em pixels do desenho) do topo da cabeça
 */
static void desenharCabeca(Vector2 chao, int x, int y, Humor humor){
    char cabeca[14][17] = {
        ".GG..........GG.",   //orelhas
        "GGGG........GGGG",
        "GGGGWWWWWWWWGGGG",
        "GGGWWWWWWWWWWGGG",
        "GGWWWWWWWWWWWWGG",
        ".GKKKKKWWKKKKKG.",   //oculos
        ".WKWKWKKKKWKWKW.",   //olhos
        ".WKWKWKWWKWKWKW.",
        ".WKKKKKWWKKKKKW.",
        ".WWWWWWWWWWWWWW.",
        "..WWWWWKKWWWWW..",   //focinho
        "..WWWWWGGWWWWW..",   //boca
        "...WWWWWWWWWW...",
        "....WWWWWWWW...."
    };
    Color cor;
    int linha;
    int coluna;

    if (humor == LOGICA_DORMINDO){
        //olhos fechados
        strcpy(cabeca[6], ".WKWWWKKKKWWWKW.");
        strcpy(cabeca[7], ".WKGGGKWWKGGGKW.");
    } else if (humor == LOGICA_ANIMADO){
        //sorriso
        strcpy(cabeca[11], "..WWWKWGGWKWWW..");
        strcpy(cabeca[12], "...WWWKKKKWWW...");
    } else if (humor == LOGICA_FELIZ || humor == LOGICA_EM_FESTA){
        //sorriso com a língua de fora
        strcpy(cabeca[11], "..WWWKWGGWKWWW..");
        strcpy(cabeca[12], "...WWWKGGKWWW...");
        strcpy(cabeca[13], "....WWWGGWWW....");
    } else if (humor == LOGICA_TRISTE){
        //cara triste
        strcpy(cabeca[0], "................");
        strcpy(cabeca[1], "................");
        strcpy(cabeca[2], "....WWWWWWWW....");
        strcpy(cabeca[3], "..WWWWWWWWWWWW..");
        strcpy(cabeca[4], ".WWWWWWWWWWWWWW.");
        strcpy(cabeca[12], "...WWWKKKKWWW...");
        strcpy(cabeca[13], "....WWKWWKWW....");
        bloco(chao, x - 1, y + 3, 2, 8, CINZA);
        bloco(chao, x + 15, y + 3, 2, 8, CINZA);
    }//if

    for (linha = 0; linha < 14; linha++){
        for (coluna = 0; coluna < 16; coluna++){
            if (cabeca[linha][coluna] == '.'){
                continue;
            }//if
            cor = BRANCO;
            if (cabeca[linha][coluna] == 'G'){
                cor = CINZA;
            } else if (cabeca[linha][coluna] == 'K'){
                cor = PRETO;
            }//if
            bloco(chao, x + coluna, y + linha, 1, 1, cor);
        }//for
    }//for
}//desenharCabeca

/**
 * Desenha o Lógica inteiro. A pose depende do humor:
 * dormindo, acordado e triste = deitado; animado = sentado;
 * feliz = em pé; em festa = pulando, com corações.
 * O rabo abana mais rápido quanto mais feliz ele está.
 * @param chao ponto do chão onde o desenho começa (canto esquerdo);
 *        o desenho ocupa cerca de 37 x 35 pixels de TAM_PIXEL
 */
static void desenharLogicaPixel(const Recursos *r, Vector2 chao, Humor humor){
    double tempo = GetTime();
    Pose pose = POSE_DEITADO;
    float velocidade = 0.0f;
    int abana = 0;
    Vector2 corpo = chao;

    if (humor == LOGICA_ANIMADO){
        pose = POSE_SENTADO;
        velocidade = 4.0f;
    } else if (humor == LOGICA_FELIZ || humor == LOGICA_EM_FESTA){
        pose = POSE_EM_PE;
        velocidade = 9.0f;
    } else if (humor == LOGICA_ACORDADO){
        velocidade = 1.5f;
    }//if
    if (velocidade > 0){
        abana = ((int)(tempo * velocidade)) % 2;
    }//if

    //pulo
    if (humor == LOGICA_EM_FESTA){
        corpo.y -= (int)(fabs(sin(tempo * 6.0)) * 4) * TAM_PIXEL;
    }//if

    //chão
    bloco(chao, -2, 0, 40, 1, CINZA_ESCURO);

    if (pose == POSE_DEITADO){
        bloco(corpo, 0, -3 - abana, 4, 1, CINZA);   //rabo
        bloco(corpo, 3, -7, 21, 6, BRANCO);         //corpo
        bloco(corpo, 5, -8, 16, 1, BRANCO);         //costas
        bloco(corpo, 8, -7, 6, 3, CINZA);           //mancha
        bloco(corpo, 4, -2, 7, 1, CINZA);           //pata de trás
        bloco(corpo, 21, -2, 13, 2, BRANCO);        //patas da frente
        bloco(corpo, 33, -2, 1, 2, CINZA);          //dedos
        desenharCabeca(corpo, 19, -16, humor);      //cabeça
        if (humor == LOGICA_DORMINDO){
            //zZ
            escrever(r, "z", corpo.x + 31 * TAM_PIXEL, corpo.y - 19 * TAM_PIXEL - (int)(tempo * 10) % 10, 20, CINZA);
            escrever(r, "Z", corpo.x + 33 * TAM_PIXEL, corpo.y - 24 * TAM_PIXEL - (int)(tempo * 10) % 10, 30, BRANCO);
        }//if
    } else if (pose == POSE_SENTADO){
        bloco(corpo, 0, -2 - abana, 5, 1, CINZA);   //rabo no chão
        bloco(corpo, 4, -9, 11, 8, BRANCO);         //quadril
        bloco(corpo, 3, -2, 8, 2, BRANCO);          //pata de trás
        bloco(corpo, 5, -8, 5, 3, CINZA);           //mancha
        bloco(corpo, 12, -16, 10, 9, BRANCO);       //peito
        bloco(corpo, 13, -8, 3, 7, BRANCO);         //pernas da frente
        bloco(corpo, 18, -8, 3, 7, BRANCO);
        bloco(corpo, 12, -1, 5, 1, BRANCO);         //patas
        bloco(corpo, 17, -1, 5, 1, BRANCO);
        desenharCabeca(corpo, 9, -29, humor);
    } else {
        bloco(corpo, 2 + abana, -19, 2, 3, CINZA);  //rabo
        bloco(corpo, 3, -16, 2, 2, CINZA);
        bloco(corpo, 4, -14, 2, 1, CINZA);
        bloco(corpo, 5, -15, 19, 7, BRANCO);        //corpo
        bloco(corpo, 8, -15, 6, 3, CINZA);          //mancha
        bloco(corpo, 6, -8, 3, 8, BRANCO);          //pernas
        bloco(corpo, 10, -8, 3, 8, BRANCO);
        bloco(corpo, 17, -8, 3, 8, BRANCO);
        bloco(corpo, 21, -8, 3, 8, BRANCO);
        bloco(corpo, 19, -18, 5, 4, BRANCO);        //pescoço
        desenharCabeca(corpo, 15, -31, humor);
    }//if

    //coraçoes
    if (humor == LOGICA_EM_FESTA){
        if (((int)(tempo * 3)) % 2 == 0){
            coracao(chao, -1, -27, BRANCO);
            coracao(chao, 33, -33, CINZA);
        } else {
            coracao(chao, 32, -24, BRANCO);
            coracao(chao, 2, -34, CINZA);
        }//if
    }//if
}//desenharLogicaPixel

/* ------------------------------------------------------------------ */
/* Regras da partida                                                    */
/* ------------------------------------------------------------------ */

/**
 * Confere se existem perguntas suficientes em cada nível.
 * @param aviso recebe a explicação do problema, se houver
 * @return 1 se dá para jogar, 0 caso contrário
 */
static int perguntasSuficientes(const Lista *lista, char aviso[TAM_MENSAGEM]){
    int precisa[6] = {0, 0, 0, 0, 0, 0};
    int etapa;
    int nivel;

    for (etapa = 1; etapa <= TOTAL_ETAPAS; etapa++){
        precisa[nivelDaEtapa(etapa)]++;
    }//for
    for (nivel = MUITO_FACIL; nivel <= MUITO_DIFICIL; nivel++){
        if (contarNivel(lista, nivel) < precisa[nivel]){
            snprintf(aviso, TAM_MENSAGEM, "NIVEL %s PRECISA DE %d PERGUNTAS (TEM %d)",
                     nomeNivel(nivel), precisa[nivel], contarNivel(lista, nivel));
            return 0;
        }//if
    }//for
    return 1;
}//perguntasSuficientes

/**
 * Coloca uma mensagem na área de mensagens da partida.
 */
static void avisar(Partida *p, const char *texto){
    size_t tamanho = strlen(texto);

    if (tamanho >= TAM_MENSAGEM){
        tamanho = TAM_MENSAGEM - 1;  //corta mensagem grande
    }//if
    memcpy(p->mensagem, texto, tamanho);
    p->mensagem[tamanho] = '\0';
}//avisar

/**
 * Humor do Lógica neste momento da partida: triste se o jogador
 * acabou de errar, senão depende dos acertos.
 */
static Humor humorAtual(const Partida *p){
    if (p->errou){
        return LOGICA_TRISTE;
    }//if
    return humorDaLogica(p->acertos);
}//humorAtual

/**
 * Sorteia a próxima pergunta da etapa atual.
 * @return 1 se conseguiu, 0 se acabaram as perguntas do nível
 */
static int proximaPergunta(Partida *p, const Lista *lista){
    int indice = sortearPergunta(lista, nivelDaEtapa(p->etapa), p->usadas, p->quantidadeUsadas);

    if (indice < 0){
        return 0;
    }//if
    p->indice = indice;
    p->usadas[p->quantidadeUsadas++] = indice;
    memset(p->eliminadas, 0, sizeof(p->eliminadas));
    p->fase = FASE_PERGUNTA;
    p->inicioPergunta = GetTime();
    avisar(p, "");
    return 1;
}//proximaPergunta

/**
 * Começa uma partida nova para o jogador cujo nome já está em p->nome.
 */
static void iniciarPartida(Partida *p, const Lista *lista, const Ranking *ranking){
    char nome[TAM_NOME];

    strcpy(nome, p->nome);
    memset(p, 0, sizeof(Partida));
    strcpy(p->nome, nome);

    p->recorde = recordePessoal(ranking, p->nome);
    p->etapa = 1;
    p->cartaVirada = -1;
    iniciarAjudas(&p->ajudas);
    p->inicioPartida = GetTime();
    proximaPergunta(p, lista);
}//iniciarPartida

/**
 * Termina a partida: calcula os pontos, guarda no ranking e prepara a festa.
 * @param acertouTudo 1 se o jogador acertou as 15 perguntas
 */
static void terminarPartida(Partida *p, Ranking *ranking, const Sons *sons, int acertouTudo){
    int i;

    strcpy(p->resultado.nome, p->nome);
    p->resultado.base = p->base;
    p->resultado.acertos = p->acertos;
    p->resultado.bonus = p->bonus;
    p->resultado.segundos = (int)(GetTime() - p->inicioPartida);
    p->resultado.pontos = calcularPontos(p->base, p->bonus, p->resultado.segundos);

    p->acertouTudo = acertouTudo;
    p->novoRecorde = p->recorde >= 0 && p->resultado.pontos > p->recorde;

    adicionarResultado(ranking, p->resultado);
    salvarRanking(ranking, ARQUIVO_RANKING);  //salva o ranking

    for (i = 0; i < QTD_CONFETES; i++){
        p->confetes[i].posicao = (Vector2){ (float)GetRandomValue(0, LARGURA_JANELA / 8) * 8,
                                            (float)GetRandomValue(-ALTURA_JANELA, 0) };
        p->confetes[i].velocidade = (float)GetRandomValue(60, 240);
        p->confetes[i].tamanho = GetRandomValue(1, 3) * 4;
    }//for

    if (acertouTudo || p->novoRecorde){
        tocar(sons, sons->vitoria);
    }//if
}//terminarPartida

/**
 * Ajuda: pular a pergunta.
 */
static void ajudaPular(Partida *p, const Lista *lista, const Sons *sons){
    char texto[TAM_MENSAGEM];

    if (!proximaPergunta(p, lista)){
        avisar(p, "NAO HA OUTRA PERGUNTA DESTE NIVEL PARA TROCAR.");
        return;
    }//if
    p->ajudas.pulos--;
    snprintf(texto, TAM_MENSAGEM, "PERGUNTA PULADA. RESTAM %d PULO(S).", p->ajudas.pulos);
    avisar(p, texto);
    tocar(sons, sons->aviso);
}//ajudaPular

/**
 * Ajuda: embaralha as 4 cartas e abre a tela de escolha.
 */
static void ajudaCartas(Partida *p, const Sons *sons){
    int i;
    int sorteio;
    int temp;

    p->ajudas.cartas--;
    for (i = 0; i < 4; i++){
        p->baralho[i] = i;
    }//for
    for (i = 3; i > 0; i--){
        sorteio = rand() % (i + 1);
        temp = p->baralho[i];
        p->baralho[i] = p->baralho[sorteio];
        p->baralho[sorteio] = temp;
    }//for
    p->cartaVirada = -1;
    p->fase = FASE_CARTAS;
    tocar(sons, sons->aviso);
}//ajudaCartas

/**
 * Vira a carta escolhida e elimina as alternativas erradas.
 * @param carta posição da carta escolhida (0 a 3)
 */
static void virarCarta(Partida *p, const Lista *lista, const Sons *sons, int carta){
    const char *nomes[4] = {"REI", "AS", "2", "3"};
    Pergunta pergunta = lista->itens[p->indice];
    int quantidade = p->baralho[carta];
    char texto[TAM_MENSAGEM];
    int errada;
    int i;

    p->cartaVirada = carta;
    for (i = 0; i < quantidade; i++){
        errada = sortearErrada(pergunta, p->eliminadas);
        if (errada >= 0){
            p->eliminadas[errada] = 1;
        }//if
    }//for

    if (quantidade == 0){
        avisar(p, "CARTA REI: NENHUMA ALTERNATIVA ELIMINADA.");
        tocar(sons, sons->erro);
    } else {
        snprintf(texto, TAM_MENSAGEM, "CARTA %s: %d ALTERNATIVA(S) ERRADA(S) ELIMINADA(S).",
                 nomes[quantidade], quantidade);
        avisar(p, texto);
        tocar(sons, sons->acerto);
    }//if
}//virarCarta

/**
 * Revela a resposta depois do suspense: confere, conta o bônus e toca o som.
 */
static void revelar(Partida *p, const Lista *lista, const Sons *sons){
    Pergunta pergunta = lista->itens[p->indice];
    Humor antes = humorDaLogica(p->acertos);
    char texto[TAM_MENSAGEM];

    p->fase = FASE_REVELADA;
    p->acertou = (p->escolhida == pergunta.correta);

    if (!p->acertou){
        p->errou = 1;
        p->base = pontosAoErrar(p->etapa, p->acertos);
        snprintf(texto, TAM_MENSAGEM, "ERROU. A CORRETA ERA A LETRA %c. O LOGICA FICOU TRISTE.", pergunta.correta);
        avisar(p, texto);
        tocar(sons, sons->erro);
        return;
    }//if

    p->acertos++;
    //bonus de sequencia
    if (p->segundosResposta < LIMITE_RAPIDO){
        p->sequencia++;
    } else {
        p->sequencia = 0;
    }//if
    if (p->sequencia == SEQUENCIA_BONUS){
        p->bonus += PONTOS_BONUS;
        p->sequencia = 0;
        snprintf(texto, TAM_MENSAGEM, "CERTA RESPOSTA! (%d S)  BONUS +%d PONTOS!", p->segundosResposta, PONTOS_BONUS);
        tocar(sons, sons->vitoria);
    } else if (humorDaLogica(p->acertos) != antes){
        snprintf(texto, TAM_MENSAGEM, "CERTA RESPOSTA! O LOGICA ESTA %s", textoDoHumor(humorDaLogica(p->acertos)));
        tocar(sons, sons->vitoria);
    } else {
        snprintf(texto, TAM_MENSAGEM, "CERTA RESPOSTA! (%d S)", p->segundosResposta);
        tocar(sons, sons->acerto);
    }//if
    avisar(p, texto);
}//revelar

/* ------------------------------------------------------------------ */
/* Desenho da partida                                                   */
/* ------------------------------------------------------------------ */

/**
 * Painel da direita: o Lógica, o humor dele e o progresso da partida.
 */
static void desenharPainelLogica(const Recursos *r, const Partida *p){
    Rectangle area = { 935, 80, 315, 445 };
    Rectangle baixo = { 935, 545, 315, 160 };
    Humor humor = humorAtual(p);
    char texto[40];
    float x;
    int altura;
    int i;

    caixa(area, PRETO, BRANCO);
    escreverCentro(r, "LOGICA", area.x + area.width / 2, area.y + 14, 20, BRANCO);
    desenharLogicaPixel(r, (Vector2){ area.x + 30, area.y + 345 }, humor);
    snprintf(texto, sizeof(texto), "%s", textoDoHumor(humor));
    escreverCentro(r, texto, area.x + area.width / 2, area.y + 370, 20, BRANCO);
    escreverCentro(r, "ACERTE PARA ANIMAR O LOGICA", area.x + area.width / 2, area.y + 405, 10, CINZA);

    //barra de progresso
    caixa(baixo, PRETO, BRANCO);
    snprintf(texto, sizeof(texto), "ACERTOS %02d/%d", p->acertos, TOTAL_ETAPAS);
    escrever(r, texto, baixo.x + 20, baixo.y + 20, 20, BRANCO);
    for (i = 1; i <= TOTAL_ETAPAS; i++){
        x = baixo.x + 20 + (i - 1) * 18;
        altura = (i == 5 || i == 10) ? 30 : 20;
        if (i <= p->acertos){
            DrawRectangle((int)x, (int)baixo.y + 85 - altura, 14, altura, BRANCO);
        } else if (i == p->etapa && !p->errou && cursorAceso()){
            DrawRectangle((int)x, (int)baixo.y + 85 - altura, 14, altura, CINZA);
        } else {
            DrawRectangleLinesEx((Rectangle){ x, baixo.y + 85 - altura, 14, (float)altura }, 2, CINZA_ESCURO);
        }//if
    }//for
    escrever(r, "MARCOS: 5 E 10", baixo.x + 20, baixo.y + 95, 10, CINZA);
    snprintf(texto, sizeof(texto), "PONTOS %ld", pontosDosAcertos(p->acertos));
    escrever(r, texto, baixo.x + 20, baixo.y + 120, 20, BRANCO);
}//desenharPainelLogica

/**
 * Desenha a barra do topo: pergunta atual, nível, jogador e cronômetro.
 */
static void desenharTopo(const Recursos *r, const Partida *p, Pergunta pergunta){
    char texto[TAM_MENSAGEM];
    int segundos = (int)(GetTime() - p->inicioPergunta);

    if (p->fase == FASE_SUSPENSE || p->fase == FASE_REVELADA){
        segundos = p->segundosResposta;  //para o relogio
    }//if

    snprintf(texto, sizeof(texto), "PERGUNTA %02d/%d", p->etapa, TOTAL_ETAPAS);
    escrever(r, texto, 30, 22, 30, BRANCO);
    desenharNivel(pergunta.nivel, 340, 28);

    snprintf(texto, sizeof(texto), "JOGADOR: %s", p->nome);
    escrever(r, texto, 490, 28, 20, CINZA);

    snprintf(texto, sizeof(texto), "SEQ %d/%d  BONUS %d", p->sequencia, SEQUENCIA_BONUS, p->bonus);
    escrever(r, texto, 935, 28, 20, CINZA);

    //relogio pisca depois de 1 min
    snprintf(texto, sizeof(texto), "%02d:%02d", segundos / 60, segundos % 60);
    if (segundos < LIMITE_RAPIDO || cursorAceso() || p->fase != FASE_PERGUNTA){
        escrever(r, texto, 1250 - larguraTexto(r, texto, 30), 22, 30, BRANCO);
    }//if

    DrawRectangle(30, 64, 1220, BORDA, BRANCO);
}//desenharTopo

/**
 * Desenha o enunciado e as 4 alternativas. Na fase da pergunta, devolve
 * a alternativa clicada.
 * @return índice (0 a 3) da alternativa clicada, ou -1 se nenhuma
 */
static int desenharPerguntaTela(const Recursos *r, const Partida *p, Pergunta pergunta){
    Rectangle quadro = { 30, 80, 875, 160 };
    Rectangle alt;
    char texto[TAM_ALTERNATIVA + 10];
    float tamanho = 30.0f;
    float altura;
    int clicada = -1;
    int emCima;
    int invertida;
    Color fundo;
    Color frente;
    int i;

    caixa(quadro, PRETO, BRANCO);
    //diminui a letra se nao couber
    altura = escreverQuebrado(r, pergunta.enunciado, 0, 0, quadro.width - 50, tamanho, BRANCO, 0);
    if (altura > quadro.height - 30){
        tamanho = 20.0f;
        altura = escreverQuebrado(r, pergunta.enunciado, 0, 0, quadro.width - 50, tamanho, BRANCO, 0);
    }//if
    escreverQuebrado(r, pergunta.enunciado, quadro.x + 25, quadro.y + (quadro.height - altura) / 2 + 4,
                     quadro.width - 50, tamanho, BRANCO, 1);

    for (i = 0; i < 4; i++){
        alt = (Rectangle){ 30, 260 + i * 66.0f, 875, 52 };
        emCima = p->fase == FASE_PERGUNTA && !p->eliminadas[i]
                 && CheckCollisionPointRec(GetMousePosition(), alt);

        //inverte as cores da alternativa
        invertida = emCima;
        if (p->fase == FASE_CONFIRMAR && 'A' + i == p->escolhida){
            invertida = 1;
        } else if (p->fase == FASE_SUSPENSE && 'A' + i == p->escolhida){
            invertida = ((int)(GetTime() * 6)) % 2;  //pisca
        } else if (p->fase == FASE_REVELADA && 'A' + i == pergunta.correta){
            invertida = 1;
        }//if

        fundo = invertida ? BRANCO : PRETO;
        frente = invertida ? PRETO : BRANCO;
        if (p->eliminadas[i]){
            frente = CINZA_ESCURO;
        } else if (p->fase == FASE_REVELADA && 'A' + i == p->escolhida && !p->acertou){
            fundo = CINZA;  //errada fica cinza
            frente = PRETO;
        }//if

        caixa(alt, fundo, p->eliminadas[i] ? CINZA_ESCURO : BRANCO);
        snprintf(texto, sizeof(texto), "%c", 'A' + i);
        DrawRectangle((int)alt.x, (int)alt.y, 52, 52, p->eliminadas[i] ? CINZA_ESCURO : frente);
        escreverCentro(r, texto, alt.x + 27, alt.y + 12, 30, fundo);

        if (p->eliminadas[i]){
            escrever(r, "- - - - - - - -", alt.x + 75, alt.y + 16, 20, CINZA_ESCURO);
        } else {
            tamanho = 30.0f;
            if (larguraTexto(r, pergunta.alternativas[i], tamanho) > alt.width - 200){
                tamanho = 20.0f;  //letra menor
            }//if
            escrever(r, pergunta.alternativas[i], alt.x + 75, alt.y + (alt.height - tamanho) / 2 + 2, tamanho, frente);
        }//if

        //certa ou errada
        if (p->fase == FASE_REVELADA && 'A' + i == pergunta.correta){
            escrever(r, "CERTA", alt.x + alt.width - 120, alt.y + 16, 20, PRETO);
        } else if (p->fase == FASE_REVELADA && 'A' + i == p->escolhida){
            escrever(r, "ERRADA", alt.x + alt.width - 130, alt.y + 16, 20, PRETO);
        }//if

        if (emCima && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            clicada = i;
        }//if
    }//for
    return clicada;
}//desenharPerguntaTela

/**
 * Janela de confirmação com os botões Sim e Não.
 * @return 1 = sim, 0 = não, -1 = ainda não respondeu
 */
static int janelaSimNao(const Recursos *r, const char *titulo, const char *detalhe){
    Rectangle quadro = { 340, 230, 600, 250 };

    escurecerTela();
    caixa(quadro, PRETO, BRANCO);
    escreverCentro(r, titulo, quadro.x + quadro.width / 2, quadro.y + 35, 30, BRANCO);
    escreverQuebrado(r, detalhe, quadro.x + 35, quadro.y + 90, quadro.width - 70, 20, CINZA, 1);

    if (botao(r, (Rectangle){ quadro.x + 70, quadro.y + 165, 200, 50 }, "[S] SIM", 20, 1)
        || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_ENTER)){
        return 1;
    }//if
    if (botao(r, (Rectangle){ quadro.x + 330, quadro.y + 165, 200, 50 }, "[N] NAO", 20, 1)
        || IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)){
        return 0;
    }//if
    return -1;
}//janelaSimNao

/**
 * Desenha o verso de uma carta: um xadrez de pixels.
 */
static void versoDaCarta(Rectangle carta){
    int x;
    int y;

    for (y = 0; y < (int)carta.height - 32; y += 8){
        for (x = 0; x < (int)carta.width - 32; x += 8){
            if (((x + y) / 8) % 2 == 0){
                DrawRectangle((int)carta.x + 16 + x, (int)carta.y + 16 + y, 8, 8, CINZA_ESCURO);
            }//if
        }//for
    }//for
}//versoDaCarta

/**
 * Tela das cartas: 4 cartas viradas para baixo; o jogador escolhe uma.
 */
static void desenharCartas(const Recursos *r, Partida *p, const Lista *lista, const Sons *sons){
    const char *nomes[4] = {"REI", "AS", "2", "3"};
    const char *efeito[4] = {"ELIMINA 0", "ELIMINA 1", "ELIMINA 2", "ELIMINA 3"};
    Rectangle carta;
    char numero[4];
    int escolhida = -1;
    int emCima;
    int i;

    escurecerTela();
    DrawRectangle(0, 100, LARGURA_JANELA, 70, PRETO);  //fundo do titulo
    escreverCentro(r, p->cartaVirada < 0 ? "ESCOLHA UMA CARTA [1-4]" : p->mensagem,
                   LARGURA_JANELA / 2.0f, 120, 30, BRANCO);

    for (i = 0; i < 4; i++){
        carta = (Rectangle){ 250 + i * 200.0f, 200, 160, 230 };
        emCima = p->cartaVirada < 0 && CheckCollisionPointRec(GetMousePosition(), carta);
        if (emCima){
            carta.y -= 16;  //carta sobe com o mouse
        }//if

        if (p->cartaVirada < 0){
            caixa(carta, PRETO, BRANCO);
            versoDaCarta(carta);
            snprintf(numero, sizeof(numero), "%d", i + 1);
            DrawRectangle((int)(carta.x + carta.width / 2 - 30), (int)carta.y + 85, 60, 60, PRETO);
            escreverCentro(r, numero, carta.x + carta.width / 2, carta.y + 95, 40, BRANCO);
        } else if (i == p->cartaVirada){
            caixa(carta, BRANCO, BRANCO);
            escreverCentro(r, nomes[p->baralho[i]], carta.x + carta.width / 2, carta.y + 70, 60, PRETO);
            escreverCentro(r, efeito[p->baralho[i]], carta.x + carta.width / 2, carta.y + 170, 20, PRETO);
        } else {
            caixa(carta, PRETO, CINZA_ESCURO);
            escreverCentro(r, nomes[p->baralho[i]], carta.x + carta.width / 2, carta.y + 70, 60, CINZA_ESCURO);
            escreverCentro(r, efeito[p->baralho[i]], carta.x + carta.width / 2, carta.y + 170, 20, CINZA_ESCURO);
        }//if

        if (emCima && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            escolhida = i;
        }//if
        if (p->cartaVirada < 0 && IsKeyPressed(KEY_ONE + i)){
            escolhida = i;
        }//if
    }//for

    if (escolhida >= 0){
        virarCarta(p, lista, sons, escolhida);
    } else if (p->cartaVirada >= 0){
        if (botao(r, (Rectangle){ 520, 500, 240, 55 }, "[ENTER] OK", 20, 1) || IsKeyPressed(KEY_ENTER)){
            p->fase = FASE_PERGUNTA;
        }//if
    }//if
}//desenharCartas

/**
 * Tela da partida: desenha tudo e trata o mouse e o teclado.
 * @return a próxima tela
 */
static Tela telaJogo(const Recursos *r, Partida *p, const Lista *lista, Ranking *ranking){
    Pergunta pergunta = lista->itens[p->indice];
    const Sons *sons = &r->sons;
    int naPergunta = (p->fase == FASE_PERGUNTA);
    int podeAjudar = naPergunta && p->etapa < TOTAL_ETAPAS;
    int antes = p->etapa - 1;   //acertos ate agora
    int clicada;
    int resposta;
    int pontos;
    char texto[TAM_MENSAGEM];
    char rotulo[40];
    float y;
    int i;

    desenharTopo(r, p, pergunta);
    desenharPainelLogica(r, p);
    clicada = desenharPerguntaTela(r, p, pergunta);

    //pontos da pergunta
    snprintf(texto, sizeof(texto), "PONTOS   PARAR %ld   ERRAR %ld   ACERTAR %ld",
             pontosDosAcertos(antes), pontosAoErrar(p->etapa, antes), pontosDosAcertos(antes + 1));
    escrever(r, texto, 30, 535, 20, CINZA);

    //mensagem
    if (p->fase == FASE_SUSPENSE){
        pontos = ((int)(GetTime() * 3)) % 4;
        snprintf(texto, sizeof(texto), "VAMOS VER%.*s", pontos, "...");
        escrever(r, texto, 30, 570, 30, BRANCO);
    } else if (p->mensagem[0] != '\0'){
        escrever(r, p->mensagem, 30, 575, 20, BRANCO);
    } else if (p->etapa == 5 || p->etapa == 10){
        escrever(r, "MARCO DE SEGURANCA: ACERTANDO, OS PONTOS FICAM GARANTIDOS.", 30, 575, 20, BRANCO);
    } else if (p->etapa == TOTAL_ETAPAS){
        escrever(r, "ULTIMA PERGUNTA. SEM AJUDAS. QUEM ERRA PERDE TUDO.", 30, 575, 20, BRANCO);
    } else if (naPergunta && cursorAceso()){
        escrever(r, "> ESCOLHA A, B, C OU D_", 30, 575, 20, CINZA);
    }//if

    //botoes
    y = 630;
    snprintf(rotulo, sizeof(rotulo), "1 PULAR x%d", p->ajudas.pulos);
    if (botao(r, (Rectangle){ 30, y, 200, 55 }, rotulo, 20, podeAjudar && p->ajudas.pulos > 0)
        || (podeAjudar && p->ajudas.pulos > 0 && IsKeyPressed(KEY_ONE))){
        ajudaPular(p, lista, sons);
        return TELA_JOGO;
    }//if
    if (botao(r, (Rectangle){ 250, y, 200, 55 }, "2 CARTAS", 20, podeAjudar && p->ajudas.cartas > 0)
        || (podeAjudar && p->ajudas.cartas > 0 && IsKeyPressed(KEY_TWO))){
        ajudaCartas(p, sons);
        return TELA_JOGO;
    }//if
    if (botao(r, (Rectangle){ 705, y, 200, 55 }, "P PARAR", 20, naPergunta)
        || (naPergunta && (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)))){
        p->fase = FASE_PARAR;
        return TELA_JOGO;
    }//if

    //resposta pelo teclado
    if (naPergunta){
        for (i = 0; i < 4; i++){
            if (IsKeyPressed(KEY_A + i) && !p->eliminadas[i]){
                clicada = i;
            }//if
        }//for
        if (clicada >= 0){
            p->escolhida = (char)('A' + clicada);
            p->fase = FASE_CONFIRMAR;
            return TELA_JOGO;
        }//if
    }//if

    switch (p->fase){
        case FASE_CARTAS:
            desenharCartas(r, p, lista, sons);
            break;

        case FASE_CONFIRMAR:
            snprintf(texto, sizeof(texto), "SUA RESPOSTA: %c) %s",
                     p->escolhida, pergunta.alternativas[p->escolhida - 'A']);
            resposta = janelaSimNao(r, "ESTA CERTO DISSO?", texto);
            if (resposta == 1){
                p->segundosResposta = (int)(GetTime() - p->inicioPergunta);
                p->inicioFase = GetTime();
                p->fase = FASE_SUSPENSE;
                avisar(p, "");
                tocar(sons, sons->aviso);
            } else if (resposta == 0){
                p->fase = FASE_PERGUNTA;
            }//if
            break;

        case FASE_PARAR:
            snprintf(texto, sizeof(texto), "VOCE LEVA %ld PONTOS (MAIS O BONUS).", pontosDosAcertos(antes));
            resposta = janelaSimNao(r, "QUER MESMO PARAR?", texto);
            if (resposta == 1){
                p->base = pontosDosAcertos(p->acertos);
                terminarPartida(p, ranking, sons, 0);
                return TELA_FIM;
            } else if (resposta == 0){
                p->fase = FASE_PERGUNTA;
            }//if
            break;

        case FASE_SUSPENSE:
            if (GetTime() - p->inicioFase >= DURACAO_SUSPENSE){
                revelar(p, lista, sons);
            }//if
            break;

        case FASE_REVELADA:
            if (botao(r, (Rectangle){ 705, 555, 200, 50 }, "ENTER >", 20, 1)
                || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)){
                if (!p->acertou){
                    terminarPartida(p, ranking, sons, 0);
                    return TELA_FIM;
                }//if
                if (p->etapa == TOTAL_ETAPAS){
                    p->base = pontosDosAcertos(TOTAL_ETAPAS);
                    terminarPartida(p, ranking, sons, 1);
                    return TELA_FIM;
                }//if
                p->etapa++;
                if (!proximaPergunta(p, lista)){
                    //acabaram as perguntas
                    p->base = pontosDosAcertos(p->acertos);
                    terminarPartida(p, ranking, sons, 0);
                    return TELA_FIM;
                }//if
            }//if
            break;

        default:
            break;
    }//switch

    return TELA_JOGO;
}//telaJogo

/* ------------------------------------------------------------------ */
/* Outras telas                                                         */
/* ------------------------------------------------------------------ */

/**
 * Escreve o título LOGICANDO grande, com um cursor piscando no final.
 */
static void desenharTitulo(const Recursos *r, float y){
    const char *titulo = "LOGICANDO";
    float tamanho = 100;
    float largura = larguraTexto(r, titulo, tamanho);
    float x = (LARGURA_JANELA - largura) / 2.0f;

    escrever(r, titulo, x + SOMBRA, y + SOMBRA, tamanho, CINZA_ESCURO);
    escrever(r, titulo, x, y, tamanho, BRANCO);
    if (cursorAceso()){
        DrawRectangle((int)(x + largura + 16), (int)y + 10, 40, (int)tamanho - 20, BRANCO);
    }//if
}//desenharTitulo

/**
 * Menu principal. O Lógica fica dormindo no canto, esperando alguém jogar.
 */
static Tela telaMenu(const Recursos *r, const Lista *lista, char aviso[TAM_MENSAGEM]){
    char texto[TAM_MENSAGEM];
    float x = 320;

    desenharTitulo(r, 90);
    escreverCentro(r, "PUZZLES E RACIOCINIO LOGICO", LARGURA_JANELA / 2.0f, 215, 20, CINZA);
    snprintf(texto, sizeof(texto), "%d PERGUNTAS NO BANCO", lista->quantidade);
    escreverCentro(r, texto, LARGURA_JANELA / 2.0f, 245, 10, CINZA);

    desenharLogicaPixel(r, (Vector2){ 760, 520 }, LOGICA_DORMINDO);
    escreverCentro(r, "PSIU... O LOGICA ESTA DORMINDO", 760 + 18 * TAM_PIXEL, 545, 10, CINZA);

    if (botao(r, (Rectangle){ x, 310, 320, 60 }, "JOGAR", 30, 1) || IsKeyPressed(KEY_ENTER)){
        if (perguntasSuficientes(lista, aviso)){
            aviso[0] = '\0';
            return TELA_NOME;
        }//if
    }//if
    if (botao(r, (Rectangle){ x, 390, 320, 60 }, "RANKING", 30, 1) || IsKeyPressed(KEY_R)){
        return TELA_RANKING;
    }//if
    if (botao(r, (Rectangle){ x, 470, 320, 60 }, "SAIR", 30, 1) || IsKeyPressed(KEY_ESCAPE)){
        return TELA_SAIR;
    }//if

    if (aviso[0] != '\0'){
        escreverCentro(r, aviso, LARGURA_JANELA / 2.0f, 585, 20, BRANCO);
        escreverCentro(r, "CADASTRE MAIS PERGUNTAS NA VERSAO DE TERMINAL", LARGURA_JANELA / 2.0f, 615, 10, CINZA);
    }//if
    escreverCentro(r, "ENTER JOGAR   R RANKING   ESC SAIR", LARGURA_JANELA / 2.0f, 650, 10, CINZA);
    escreverCentro(r, "EQUIPE ROCKSTAR FUNDAMENTOS - UTFPR APUCARANA", LARGURA_JANELA / 2.0f, 685, 10, CINZA_ESCURO);
    return TELA_MENU;
}//telaMenu

/**
 * Tela onde o jogador digita o nome e lê as regras.
 */
static Tela telaNome(const Recursos *r, Partida *p, const Lista *lista, const Ranking *ranking){
    Rectangle campo = { 390, 120, 500, 64 };
    const char *regras[7] = {
        "15 PERGUNTAS. CADA ACERTO VALE 100 PONTOS",
        "PARAR: LEVA OS PONTOS QUE JA FEZ",
        "ERRAR: LEVA METADE (MARCOS NAS PERGUNTAS 5 E 10)",
        "ULTIMA PERGUNTA: SEM AJUDAS, QUEM ERRA PERDE TUDO",
        "AJUDAS: PULAR x3 E CARTAS",
        "BONUS: 3 ACERTOS SEGUIDOS EM MENOS DE 1 MIN = +50",
        "TEMPO: -1 PONTO A CADA 10 SEGUNDOS"
    };
    int tamanho = (int)strlen(p->nome);
    int caractere = GetCharPressed();
    int i;

    //entrada de dados
    while (caractere > 0){
        if (caractere >= 32 && caractere <= 126 && caractere != ',' && tamanho < TAM_NOME - 1){
            p->nome[tamanho++] = (char)caractere;
            p->nome[tamanho] = '\0';
        }//if
        caractere = GetCharPressed();
    }//while
    if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && tamanho > 0){
        p->nome[--tamanho] = '\0';
    }//if

    escreverCentro(r, "SEU NOME", LARGURA_JANELA / 2.0f, 70, 30, BRANCO);
    caixa(campo, PRETO, BRANCO);
    escrever(r, p->nome, campo.x + 20, campo.y + 17, 30, BRANCO);
    if (cursorAceso()){
        DrawRectangle((int)(campo.x + 24 + larguraTexto(r, p->nome, 30)), (int)campo.y + 15, 18, 34, BRANCO);
    }//if

    caixa((Rectangle){ 200, 215, 880, 300 }, PRETO, BRANCO);
    escrever(r, "REGRAS", 230, 235, 20, CINZA);
    for (i = 0; i < 7; i++){
        escrever(r, "-", 230, 272 + i * 30.0f, 20, CINZA);
        escrever(r, regras[i], 260, 272 + i * 30.0f, 20, BRANCO);
    }//for
    escreverCentro(r, "ACERTE PARA DEIXAR O LOGICA FELIZ!", LARGURA_JANELA / 2.0f, 535, 20, BRANCO);

    if ((botao(r, (Rectangle){ 430, 580, 200, 60 }, "COMECAR", 20, tamanho > 0)
         || IsKeyPressed(KEY_ENTER)) && tamanho > 0){
        iniciarPartida(p, lista, ranking);
        return TELA_JOGO;
    }//if
    if (botao(r, (Rectangle){ 650, 580, 200, 60 }, "VOLTAR", 20, 1) || IsKeyPressed(KEY_ESCAPE)){
        return TELA_MENU;
    }//if
    return TELA_NOME;
}//telaNome

/**
 * Tela do fim da partida, com o resumo, o Lógica e a comemoração.
 */
static Tela telaFim(const Recursos *r, Partida *p, const Lista *lista, const Ranking *ranking){
    Rectangle quadro = { 560, 170, 620, 340 };
    const char *rotulos[5] = { "JOGADOR", "ACERTOS", "PONTOS DOS ACERTOS", "BONUS", "TEMPO" };
    char valores[5][40];
    char texto[TAM_MENSAGEM];
    float tempo = GetFrameTime();
    Humor humor = p->acertouTudo ? LOGICA_EM_FESTA : humorAtual(p);
    int i;

    //confetes
    if (p->acertouTudo || p->novoRecorde){
        for (i = 0; i < QTD_CONFETES; i++){
            p->confetes[i].posicao.y += p->confetes[i].velocidade * tempo;
            if (p->confetes[i].posicao.y > ALTURA_JANELA){
                p->confetes[i].posicao.y = -12;
            }//if
            DrawRectangle((int)p->confetes[i].posicao.x, ((int)p->confetes[i].posicao.y / 4) * 4,
                          p->confetes[i].tamanho, p->confetes[i].tamanho, i % 3 == 0 ? CINZA : BRANCO);
        }//for
    }//if

    escreverCentro(r, p->acertouTudo ? "VOCE ACERTOU TUDO!" : "FIM DE JOGO",
                   LARGURA_JANELA / 2.0f, 70, 60, BRANCO);

    //desenha o logica
    desenharLogicaPixel(r, (Vector2){ 120, 470 }, humor);
    snprintf(texto, sizeof(texto), "LOGICA: %s", textoDoHumor(humor));
    escreverCentro(r, texto, 120 + 18 * TAM_PIXEL, 495, 20, BRANCO);

    snprintf(valores[0], sizeof(valores[0]), "%s", p->resultado.nome);
    snprintf(valores[1], sizeof(valores[1]), "%d/%d", p->resultado.acertos, TOTAL_ETAPAS);
    snprintf(valores[2], sizeof(valores[2]), "%ld", p->resultado.base);
    snprintf(valores[3], sizeof(valores[3]), "+%d", p->resultado.bonus);
    snprintf(valores[4], sizeof(valores[4]), "%dM%02dS (-%d)", p->resultado.segundos / 60,
             p->resultado.segundos % 60, p->resultado.segundos / 10);

    caixa(quadro, PRETO, BRANCO);
    for (i = 0; i < 5; i++){
        escrever(r, rotulos[i], quadro.x + 35, quadro.y + 30 + i * 40.0f, 20, CINZA);
        escrever(r, valores[i], quadro.x + 380, quadro.y + 30 + i * 40.0f, 20, BRANCO);
    }//for
    DrawRectangle((int)quadro.x + 35, (int)quadro.y + 232, (int)quadro.width - 70, BORDA, BRANCO);
    escrever(r, "PONTUACAO", quadro.x + 35, quadro.y + 255, 30, BRANCO);
    snprintf(texto, sizeof(texto), "%d", p->resultado.pontos);
    escrever(r, texto, quadro.x + 380, quadro.y + 255, 30, BRANCO);

    if (p->novoRecorde){
        snprintf(texto, sizeof(texto), "NOVO RECORDE! (ANTES: %d)", p->recorde);
        if (cursorAceso()){
            escreverCentro(r, texto, quadro.x + quadro.width / 2, quadro.y + 302, 20, BRANCO);
        }//if
    } else if (p->recorde >= 0){
        snprintf(texto, sizeof(texto), "SEU RECORDE: %d", p->recorde);
        escreverCentro(r, texto, quadro.x + quadro.width / 2, quadro.y + 302, 20, CINZA);
    }//if

    if (botao(r, (Rectangle){ 300, 570, 220, 60 }, "JOGAR DE NOVO", 20, 1)){
        iniciarPartida(p, lista, ranking);
        return TELA_JOGO;
    }//if
    if (botao(r, (Rectangle){ 530, 570, 220, 60 }, "RANKING", 20, 1)){
        return TELA_RANKING;
    }//if
    if (botao(r, (Rectangle){ 760, 570, 220, 60 }, "MENU", 20, 1) || IsKeyPressed(KEY_ENTER)){
        return TELA_MENU;
    }//if
    return TELA_FIM;
}//telaFim

/**
 * Tela do ranking: os 10 melhores jogadores (um resultado por nome).
 */
static Tela telaRanking(const Recursos *r, const Ranking *ranking){
    const float colunas[6] = { 170, 240, 600, 760, 920, 1040 };
    const char *titulos[6] = { "#", "NOME", "PONTOS", "ACERTOS", "TEMPO", "BONUS" };
    int escolhidos[TAMANHO_TOP];
    int quantidade = 0;
    int melhor;
    int jaFoi;
    char texto[60];
    Color cor;
    float y;
    int i;
    int j;
    int k;

    escreverCentro(r, "RANKING", LARGURA_JANELA / 2.0f, 40, 40, BRANCO);
    caixa((Rectangle){ 140, 110, 1000, 480 }, PRETO, BRANCO);
    for (i = 0; i < 6; i++){
        escrever(r, titulos[i], colunas[i], 132, 20, CINZA);
    }//for
    DrawRectangle(170, 160, 940, 2, CINZA_ESCURO);

    //escolhe o top 10
    for (k = 0; k < TAMANHO_TOP && k < ranking->quantidade; k++){
        melhor = -1;
        for (i = 0; i < ranking->quantidade; i++){
            jaFoi = 0;
            for (j = 0; j < quantidade; j++){
                if (escolhidos[j] == i){
                    jaFoi = 1;
                }//if
            }//for
            if (!jaFoi && (melhor < 0
                || ranking->itens[i].pontos > ranking->itens[melhor].pontos
                || (ranking->itens[i].pontos == ranking->itens[melhor].pontos
                    && ranking->itens[i].segundos < ranking->itens[melhor].segundos))){
                melhor = i;
            }//if
        }//for
        escolhidos[quantidade++] = melhor;
    }//for

    if (quantidade == 0){
        escreverCentro(r, "NINGUEM JOGOU AINDA", LARGURA_JANELA / 2.0f, 330, 20, CINZA);
    }//if

    for (k = 0; k < quantidade; k++){
        const Resultado *res = &ranking->itens[escolhidos[k]];

        y = 178 + k * 40.0f;
        cor = BRANCO;
        if (k == 0){
            //destaca o primeiro lugar
            DrawRectangle(160, (int)y - 8, 960, 36, BRANCO);
            cor = PRETO;
        } else if (k >= 3){
            cor = CINZA;
        }//if
        snprintf(texto, sizeof(texto), "%d", k + 1);
        escrever(r, texto, colunas[0], y, 20, cor);
        escrever(r, res->nome, colunas[1], y, 20, cor);
        snprintf(texto, sizeof(texto), "%d", res->pontos);
        escrever(r, texto, colunas[2], y, 20, cor);
        snprintf(texto, sizeof(texto), "%d", res->acertos);
        escrever(r, texto, colunas[3], y, 20, cor);
        snprintf(texto, sizeof(texto), "%dM%02dS", res->segundos / 60, res->segundos % 60);
        escrever(r, texto, colunas[4], y, 20, cor);
        snprintf(texto, sizeof(texto), "%d", res->bonus);
        escrever(r, texto, colunas[5], y, 20, cor);
    }//for

    if (botao(r, (Rectangle){ 540, 620, 200, 60 }, "VOLTAR", 20, 1)
        || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)){
        return TELA_MENU;
    }//if
    return TELA_RANKING;
}//telaRanking

/**
 * Carrega as perguntas, como na versão de terminal: do binário, se
 * existir, ou do CSV na primeira execução.
 */
static void carregarPerguntas(Lista *lista){
    int ignoradas;

    if (FileExists(ARQUIVO_BINARIO) && carregarBinario(lista, ARQUIVO_BINARIO)){
        return;
    }//if
    carregarCSV(lista, ARQUIVO_CSV, &ignoradas);
}//carregarPerguntas

int main(void){
    Lista lista;
    Ranking ranking;
    Recursos recursos;
    Partida partida;
    Tela tela = TELA_MENU;
    Tela anterior = TELA_MENU;
    char aviso[TAM_MENSAGEM] = "";

    srand((unsigned int)time(NULL));
    if (!iniciarLista(&lista) || !iniciarRanking(&ranking)){
        printf("Erro: memoria insuficiente!\n");
        return 1;
    }//if
    carregarPerguntas(&lista);
    carregarRanking(&ranking, ARQUIVO_RANKING);
    memset(&partida, 0, sizeof(Partida));

    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "LOGICANDO - Puzzles e Raciocínio Lógico");
    SetExitKey(KEY_NULL);  //esc nao fecha a janela
    SetTargetFPS(60);
    InitAudioDevice();

    recursos.fonte = GetFontDefault();  //fonte pixelada
    carregarSons(&recursos.sons);

    while (tela != TELA_SAIR && !WindowShouldClose()){
        anterior = tela;
        BeginDrawing();
        ClearBackground(PRETO);
        switch (tela){
            case TELA_MENU:    tela = telaMenu(&recursos, &lista, aviso);                    break;
            case TELA_NOME:    tela = telaNome(&recursos, &partida, &lista, &ranking);       break;
            case TELA_JOGO:    tela = telaJogo(&recursos, &partida, &lista, &ranking);       break;
            case TELA_FIM:     tela = telaFim(&recursos, &partida, &lista, &ranking);        break;
            case TELA_RANKING: tela = telaRanking(&recursos, &ranking);                      break;
            default:           break;
        }//switch
        desenharLinhasTV();
        EndDrawing();

        //limpa o teclado ao trocar de tela
        if (tela != anterior){
            while (GetCharPressed() > 0){
            }//while
        }//if
    }//while

    //salva tudo antes de sair
    salvarBinario(&lista, ARQUIVO_BINARIO);
    salvarRanking(&ranking, ARQUIVO_RANKING);

    liberarSons(&recursos.sons);
    CloseAudioDevice();
    CloseWindow();
    liberarLista(&lista);
    liberarRanking(&ranking);
    return 0;
}//main
