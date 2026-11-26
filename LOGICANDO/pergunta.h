/**
 * @file pergunta.h
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Estrutura Pergunta e o vetor dinâmico (Lista) que guarda as perguntas.
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef PERGUNTA_H
#define PERGUNTA_H

#define TAM_ENUNCIADO 200
#define TAM_ALTERNATIVA 80

/** Níveis de dificuldade */
enum Nivel {
    MUITO_FACIL = 1,
    FACIL,
    MEDIO,
    DIFICIL,
    MUITO_DIFICIL
};//enum

/** Uma pergunta do jogo */
typedef struct {
    int id;                                 /**< identificador da pergunta */
    char enunciado[TAM_ENUNCIADO];          /**< texto da pergunta */
    char alternativas[4][TAM_ALTERNATIVA];  /**< alternativas A, B, C e D */
    char correta;                           /**< letra da alternativa correta */
    int nivel;                              /**< nível de dificuldade (1 a 5) */
} Pergunta;//struct

/** Vetor de tamanho dinâmico com as perguntas */
typedef struct {
    Pergunta *itens;   /**< vetor alocado com malloc/realloc */
    int quantidade;    /**< quantas perguntas existem */
    int capacidade;    /**< quantas posições foram alocadas */
    int proximoId;     /**< id que a próxima pergunta vai receber */
} Lista;//struct

/**
 * Cria a lista vazia (aloca a capacidade inicial).
 * @param lista endereço da lista
 * @return 1 se deu certo, 0 se faltou memória
 */
int iniciarLista(Lista *lista);

/**
 * Libera a memória da lista.
 * @param lista endereço da lista
 */
void liberarLista(Lista *lista);

/**
 * Insere uma pergunta no final da lista (o id é gerado automaticamente).
 * O vetor dobra de tamanho quando fica cheio.
 * @param lista endereço da lista
 * @param p pergunta que será inserida
 * @return o id da pergunta, ou -1 se faltou memória
 */
int inserirPergunta(Lista *lista, Pergunta p);

/**
 * Remove a pergunta que está em uma posição do vetor.
 * O vetor diminui pela metade quando sobra pouca coisa.
 * @param lista endereço da lista
 * @param indice posição da pergunta no vetor
 */
void removerPergunta(Lista *lista, int indice);

/**
 * Procura uma pergunta pelo id.
 * @param lista endereço da lista
 * @param id id procurado
 * @return posição no vetor, ou -1 se não achou
 */
int buscarPorId(const Lista *lista, int id);

/**
 * Conta quantas perguntas existem em um nível.
 * @param lista endereço da lista
 * @param nivel nível desejado
 * @return quantidade de perguntas do nível
 */
int contarNivel(const Lista *lista, int nivel);

/**
 * Retorna o nome de um nível.
 * @param nivel número do nível (1 a 5)
 * @return texto com o nome do nível
 */
const char *nomeNivel(int nivel);

/**
 * Mostra uma pergunta na tela.
 * @param p pergunta que será mostrada
 * @param mostrarResposta 1 para indicar a alternativa correta, 0 para não indicar
 */
void mostrarPergunta(Pergunta p, int mostrarResposta);

#endif /* PERGUNTA_H */
