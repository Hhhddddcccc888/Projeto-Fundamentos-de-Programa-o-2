/**
 * @file util.h
 * @author Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira
 * @brief Funções auxiliares para ler dados do teclado com segurança.
 * @version 1.0
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef UTIL_H
#define UTIL_H

/**
 * Lê uma linha do teclado e remove o '\n' do final.
 * @param destino vetor que recebe o texto
 * @param tamanho tamanho do vetor destino
 * @return 1 se leu normalmente, 0 se a entrada acabou (EOF), -1 se o texto era grande demais
 */
int lerTexto(char *destino, int tamanho);

/**
 * Lê um texto que não pode ser vazio nem ter vírgula (o CSV usa vírgula
 * para separar os campos). Repete a pergunta até o texto ser válido.
 * @param mensagem texto mostrado antes da leitura
 * @param destino vetor que recebe o texto
 * @param tamanho tamanho do vetor destino
 * @return 1 se leu o texto, 0 se a entrada acabou (EOF)
 */
int lerTextoObrigatorio(const char *mensagem, char *destino, int tamanho);

/**
 * Lê um texto opcional. Se o usuário só apertar ENTER, o destino não muda.
 * @param mensagem texto mostrado antes da leitura
 * @param destino vetor que será alterado (somente se algo for digitado)
 * @param tamanho tamanho do vetor destino
 * @return 1 se leu normalmente, 0 se a entrada acabou (EOF)
 */
int lerTextoOpcional(const char *mensagem, char *destino, int tamanho);

/**
 * Lê um número inteiro dentro de um intervalo. Repete até ser válido.
 * @param mensagem texto mostrado antes da leitura
 * @param minimo menor valor aceito
 * @param maximo maior valor aceito
 * @return o número lido (se a entrada acabar, retorna o mínimo)
 */
int lerInteiro(const char *mensagem, int minimo, int maximo);

/**
 * Lê uma letra que esteja entre as opções válidas (aceita minúscula).
 * @param mensagem texto mostrado antes da leitura
 * @param validas letras aceitas, em maiúsculas
 * @return a letra lida em maiúscula (se a entrada acabar, retorna a primeira letra de validas)
 */
char lerLetra(const char *mensagem, const char *validas);

/**
 * Espera o usuário apertar ENTER.
 */
void pausar(void);

/**
 * Verifica se um texto contém uma palavra, sem diferenciar maiúsculas de minúsculas.
 * @param texto texto onde será procurado
 * @param busca palavra procurada
 * @return 1 se encontrou, 0 se não encontrou
 */
int contemTexto(const char *texto, const char *busca);

/**
 * Compara dois textos sem diferenciar maiúsculas de minúsculas.
 * @param a primeiro texto
 * @param b segundo texto
 * @return 1 se forem iguais, 0 caso contrário
 */
int textosIguais(const char *a, const char *b);

#endif /* UTIL_H */
