# Makefile do Jogo do Milhao
#   make        -> compila
#   make run    -> executa
#   make clean  -> apaga os arquivos gerados

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
OBJETOS = main.o util.o pergunta.o arquivo.o cadastro.o jogo.o

all: $(OBJETOS)
	$(CC) $(CFLAGS) -o milhao $(OBJETOS)

main.o: main.c pergunta.h arquivo.h cadastro.h jogo.h util.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

pergunta.o: pergunta.c pergunta.h
	$(CC) $(CFLAGS) -c pergunta.c

arquivo.o: arquivo.c arquivo.h pergunta.h
	$(CC) $(CFLAGS) -c arquivo.c

cadastro.o: cadastro.c cadastro.h pergunta.h arquivo.h util.h
	$(CC) $(CFLAGS) -c cadastro.c

jogo.o: jogo.c jogo.h pergunta.h util.h
	$(CC) $(CFLAGS) -c jogo.c

run: all
	./milhao

clean:
	rm -f *.o milhao milhao.exe

.PHONY: all run clean
