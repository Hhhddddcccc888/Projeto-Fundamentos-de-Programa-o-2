/**
 * @file ranking.h
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Ranking local das partidas e recorde pessoal de cada jogador.
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 * Cada partida jogada vira um Resultado. Todos os resultados ficam em um
 * vetor dinâmico (Ranking), que é salvo em um arquivo binário para não
 * se perder quando o programa fecha.
 */

#ifndef RANKING_H
#define RANKING_H

#define ARQUIVO_RANKING "ranking.bin"
#define TAM_NOME 30
#define TAMANHO_TOP 10

/** Resultado de uma partida */
typedef struct {
    char nome[TAM_NOME];  /**< nome do jogador */
    int pontos;           /**< pontuação final (ver calcularPontos) */
    long base;            /**< pontos dos acertos, antes do bônus e do desconto do tempo */
    int acertos;          /**< perguntas acertadas */
    int segundos;         /**< tempo total da partida */
    int bonus;            /**< pontos ganhos com bônus de sequência */
} Resultado;//struct

/** Vetor de tamanho dinâmico com os resultados */
typedef struct {
    Resultado *itens;  /**< vetor alocado com malloc/realloc */
    int quantidade;    /**< quantos resultados existem */
    int capacidade;    /**< quantas posições foram alocadas */
} Ranking;//struct

/**
 * Cria o ranking vazio.
 * @param ranking endereço do ranking
 * @return 1 se deu certo, 0 se faltou memória
 */
int iniciarRanking(Ranking *ranking);

/**
 * Libera a memória do ranking.
 * @param ranking endereço do ranking
 */
void liberarRanking(Ranking *ranking);

/**
 * Guarda o resultado de uma partida. Cada jogador aparece uma vez só:
 * se o nome já existe (sem diferenciar maiúsculas de minúsculas), o
 * resultado só é trocado quando a pontuação nova é maior; se for menor
 * ou igual, o antigo é mantido. Jogador novo entra no final do vetor,
 * que dobra de tamanho quando enche.
 * @param ranking endereço do ranking
 * @param r resultado da partida
 * @return 1 se deu certo, 0 se faltou memória
 */
int adicionarResultado(Ranking *ranking, Resultado r);

/**
 * Calcula a pontuação final de uma partida:
 * pontos = pontos dos acertos + bônus - 1 a cada 10 segundos.
 * A pontuação nunca fica negativa.
 * @param base pontos dos acertos (100 por acerto, ou o que sobrou ao errar)
 * @param bonus pontos de bônus por sequência
 * @param segundos tempo total da partida
 * @return pontuação final
 */
int calcularPontos(long base, int bonus, int segundos);

/**
 * Procura a maior pontuação de um jogador (recorde pessoal).
 * O nome é comparado sem diferenciar maiúsculas de minúsculas.
 * @param ranking endereço do ranking
 * @param nome nome do jogador
 * @return maior pontuação, ou -1 se o jogador nunca jogou
 */
int recordePessoal(const Ranking *ranking, const char *nome);

/**
 * Mostra as 10 melhores partidas, da maior para a menor pontuação.
 * @param ranking endereço do ranking
 */
void mostrarRanking(const Ranking *ranking);

/**
 * Mostra o recorde e o histórico de partidas de um jogador.
 * @param ranking endereço do ranking
 * @param nome nome do jogador
 */
void mostrarHistoricoJogador(const Ranking *ranking, const char *nome);

/**
 * Salva o ranking em um arquivo binário.
 * @param ranking endereço do ranking
 * @param nome nome do arquivo
 * @return 1 se deu certo, 0 caso contrário
 */
int salvarRanking(const Ranking *ranking, const char *nome);

/**
 * Lê o ranking de um arquivo binário. Se o arquivo não existir
 * (primeira execução), o ranking continua vazio.
 * @param ranking endereço do ranking (já iniciado)
 * @param nome nome do arquivo
 * @return 1 se leu o arquivo, 0 se ele não existe ou está com problema
 */
int carregarRanking(Ranking *ranking, const char *nome);

#endif /* RANKING_H */
