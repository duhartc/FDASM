PROG = pars
OBJS = parseur.o
CC = gcc
CFLAGS = -c -std=c99 -g -Wall -Wextra

all: $(PROG)

pars: parseur.o
	$(CC) -o $@ $^ -lm

parseur.o: parseur.c
	$(CC) -o $@ -c $< $(CFLAGS) -lm

check:
	./$(PROG) > res.txt
clean: 
	rm $(OBJS) $(PROG)