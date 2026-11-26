# Makefile do LOGICANDO
#   make        -> compila
#   make run    -> compila e executa
#   make clean  -> apaga os arquivos gerados na compilacao
#   make reset  -> apaga tambem os dados salvos (perguntas.bin e ranking.bin),
#                  para o jogo voltar a ler o perguntas.csv
#
# No Windows (MinGW) use "mingw32-make" no lugar de "make".

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
OBJETOS = main.o util.o tela.o visual.o pergunta.o arquivo.o cadastro.o ajuda.o ranking.o jogo.o

ifeq ($(OS),Windows_NT)
    EXECUTAVEL = logicando.exe
    RODAR = logicando.exe
    APAGAR = del /Q /F
else
    EXECUTAVEL = logicando
    RODAR = ./logicando
    APAGAR = rm -f
endif

all: $(EXECUTAVEL)

$(EXECUTAVEL): $(OBJETOS)
	$(CC) $(CFLAGS) -o $(EXECUTAVEL) $(OBJETOS)

main.o: main.c pergunta.h arquivo.h cadastro.h jogo.h ranking.h tela.h util.h visual.h
	$(CC) $(CFLAGS) -c main.c

visual.o: visual.c visual.h pergunta.h jogo.h ranking.h tela.h
	$(CC) $(CFLAGS) -c visual.c

util.o: util.c util.h tela.h
	$(CC) $(CFLAGS) -c util.c

tela.o: tela.c tela.h pergunta.h
	$(CC) $(CFLAGS) -c tela.c

pergunta.o: pergunta.c pergunta.h tela.h
	$(CC) $(CFLAGS) -c pergunta.c

arquivo.o: arquivo.c arquivo.h pergunta.h
	$(CC) $(CFLAGS) -c arquivo.c

cadastro.o: cadastro.c cadastro.h pergunta.h arquivo.h util.h tela.h
	$(CC) $(CFLAGS) -c cadastro.c

ajuda.o: ajuda.c ajuda.h pergunta.h tela.h util.h
	$(CC) $(CFLAGS) -c ajuda.c

ranking.o: ranking.c ranking.h tela.h util.h
	$(CC) $(CFLAGS) -c ranking.c

jogo.o: jogo.c jogo.h pergunta.h ranking.h ajuda.h tela.h util.h visual.h
	$(CC) $(CFLAGS) -c jogo.c

run: all
	$(RODAR)

# ---------------------------------------------------------------------
# Versao grafica (raylib)
#   make grafico      -> compila logicando_grafico
#   make grafico-run  -> compila e abre a janela do jogo
# No Windows usa o compilador que vem junto com o instalador da raylib
# (C:\raylib\w64devkit), porque a libraylib.a dele e de 64 bits.
# No Ubuntu: precisa da raylib 5.5 ou mais nova (compile a do GitHub)
# ---------------------------------------------------------------------
GRAFICO_FONTES = grafico.c jogo.c ajuda.c ranking.c pergunta.c arquivo.c tela.c util.c visual.c

ifeq ($(OS),Windows_NT)
    RAYLIB = C:/raylib
    GRAFICO_CC = $(RAYLIB)/w64devkit/bin/gcc.exe
    GRAFICO_EXECUTAVEL = logicando_grafico.exe
    GRAFICO_RODAR = logicando_grafico.exe
    GRAFICO_FLAGS = -I$(RAYLIB)/raylib/src -L$(RAYLIB)/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm
else
    GRAFICO_CC = gcc
    GRAFICO_EXECUTAVEL = logicando_grafico
    GRAFICO_RODAR = ./logicando_grafico
    GRAFICO_FLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

grafico: $(GRAFICO_EXECUTAVEL)

$(GRAFICO_EXECUTAVEL): $(GRAFICO_FONTES) *.h
	$(GRAFICO_CC) $(CFLAGS) -O2 -o $(GRAFICO_EXECUTAVEL) $(GRAFICO_FONTES) $(GRAFICO_FLAGS)

grafico-run: grafico
	$(GRAFICO_RODAR)

clean:
	-$(APAGAR) *.o $(EXECUTAVEL) $(GRAFICO_EXECUTAVEL)

reset: clean
	-$(APAGAR) perguntas.bin ranking.bin

.PHONY: all run clean reset grafico grafico-run
