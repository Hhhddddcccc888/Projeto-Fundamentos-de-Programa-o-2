# LOGICANDO — Puzzles e Raciocínio Lógico

Jogo de perguntas no estilo do Show do Milhão (SBT), com o tema puzzles e raciocínio lógico.

Projeto de Fundamentos de Programação 2 — UTFPR Apucarana (2º semestre de 2026).

**Equipe Rockstar Fundamentos:** Rafael Dalpozo Bruza Alves, Rafael Henrique Sakakibara e Renato Chapper da Silva Pereira.

## Como compilar e jogar

Linux / MSYS / Git Bash:

```
make        # compila
make run    # compila e executa
make clean  # apaga os arquivos da compilação
make reset  # apaga também perguntas.bin e ranking.bin
```

Windows com MinGW: use `mingw32-make` no lugar de `make` (ex: `mingw32-make run`).

### Versão gráfica (raylib)

Além da versão de terminal, o jogo tem uma versão em janela, feita com a biblioteca gratuita [raylib](https://www.raylib.com). O visual é minimalista, em preto e branco, com fonte pixelada, linhas de TV antiga e sons de videogame (ondas quadradas). Ela usa as mesmas regras e os mesmos arquivos (`perguntas.bin` e `ranking.bin`), então as partidas das duas versões entram no mesmo ranking. Para cadastrar, alterar ou exportar perguntas, use a versão de terminal.

```
make grafico-run    # compila e abre a janela (Windows: mingw32-make grafico-run)
```

- **Windows:** instale a raylib pelo instalador oficial em `C:\raylib`. O Makefile usa o compilador que vem junto (`C:\raylib\w64devkit`).
- **Ubuntu:** é preciso a raylib 5.5 ou mais nova. O pacote do `apt` costuma ser antigo, então o melhor é compilar a raylib do GitHub (instruções em github.com/raysan5/raylib, seção "Working on GNU Linux").

Controles: mouse, ou teclado (A-D responde, 1-2 usa ajudas, P para, S/N confirma, Enter continua, Esc volta).

## Arquivos

| Arquivo | Para que serve |
|---|---|
| `main.c` | menu principal, carrega e salva os dados |
| `pergunta.c/.h` | estrutura `Pergunta` e vetor dinâmico `Lista` |
| `arquivo.c/.h` | leitura do CSV, exportação para CSV, arquivo binário |
| `cadastro.c/.h` | inserir, listar, pesquisar, navegar por nível, alterar, excluir |
| `jogo.c/.h` | partida com as regras do SBT |
| `ajuda.c/.h` | ajudas: pular e cartas |
| `ranking.c/.h` | ranking local, pontuação e recorde pessoal |
| `tela.c/.h` | cores ANSI, beeps, mensagens coloridas, caixas e posição do cursor |
| `visual.c/.h` | letreiro animado, abertura, suspense, comemoração e o Lógica em ASCII |
| `util.c/.h` | leitura segura do teclado |
| `grafico.c` | versão gráfica com raylib (preto e branco, pixelada, mouse e sons gerados pelo programa) |
| `perguntas.csv` | 80 perguntas iniciais (20/20/15/15/10 por nível) |

Na primeira execução as perguntas são lidas do `perguntas.csv`. Ao sair, tudo é salvo em `perguntas.bin`, e nas próximas execuções o jogo lê desse arquivo binário. O ranking fica em `ranking.bin`.

## Regras da partida

- 15 perguntas: 2 muito fáceis, 2 fáceis, 4 médias, 4 difíceis e 3 muito difíceis.
- Cada acerto vale 100 pontos. Marcos de segurança nas perguntas 5 e 10.
- **Parar:** o jogador leva os pontos que já fez.
- **Errar:** leva metade dos pontos, mas nunca menos que o último marco de segurança (500 ou 1000).
- **Última pergunta:** sem ajudas, e quem erra perde tudo.

### Ajudas (inspiradas no programa do SBT)

| Ajuda | Quantidade | O que faz |
|---|---|---|
| Pular | 3 | troca a pergunta por outra do mesmo nível |
| Cartas | 1 | o jogador escolhe 1 de 4 cartas: Rei elimina 0, Ás elimina 1, 2 elimina 2, 3 elimina 3 alternativas erradas |

### O cachorro Lógica

No lugar da escada de prêmios fica o **Lógica**, um cachorro de óculos. Ele começa deitado, dormindo, e vai ficando mais feliz conforme o jogador acerta:

| Acertos | Lógica |
|---|---|
| 0 | deitado, dormindo |
| 1 a 4 | deitado, acordado |
| 5 a 9 | sentado, sorrindo e abanando o rabo |
| 10 a 14 | em pé, com a língua de fora |
| 15 | pulando de alegria (em festa) |
| errou | deitado, triste, de orelhas caídas |

No terminal ele é desenhado em ASCII (`visual.c`). Na versão gráfica ele é desenhado em pixel art, e o rabo abana mais rápido quanto mais feliz ele está (`grafico.c`).

## Parte criativa

- **O cachorro Lógica**, que reage aos acertos e erros do jogador.
- **Ranking local:** o jogador informa o nome; acertos, bônus e tempo viram pontos, que são salvos em `ranking.bin` (TOP 10 no menu 9). Cada nome aparece uma vez só: se o jogador já está no ranking, a pontuação só é trocada quando a nova é maior.
- **Bônus por sequência:** acertar 3 perguntas seguidas em menos de 1 minuto cada vale +50 pontos.
- **Recorde pessoal:** no início da partida o jogo mostra o recorde do jogador e, no final, avisa se ele foi superado.
- Cores para cada nível de dificuldade e sons diferentes para acerto, erro, aviso e vitória.
- Interface no terminal com cara de programa de TV: abertura com letreiro animado, pergunta dentro de caixas, o Lógica na lateral e suspense antes de revelar a resposta. O terminal precisa ter pelo menos 80 colunas.

**Pontuação:** `100 × acertos (ou o que sobrou ao errar) + bônus − 1 ponto a cada 10 segundos` (nunca fica negativa).
