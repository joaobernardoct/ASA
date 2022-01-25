# Makefile
# ASA 2017-18

CC       = gcc
CFLAGS   = -ansi -Wall

.PHONY: all clean zip

all: p1asa

p1asa: p1asa.o
	$(CC) $(CFLAGS) -g -o $@ $+

p1asa.o: proj.c
	$(CC) $(CFLAGS) -g -o $@ -c $<

clean:
	rm -f *.o p1asa

run:
	./proj 6 9 1 2 2 3 3 1 2 4 5 4 4 5 3 5 3 6 6 5

zip: p1asa.zip

p1asa.zip: Makefile p1asa.c
	zip $@ $+
