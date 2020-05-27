IDIR   = ../headers
CFLAGS = -std=gnu99 -Wall -Werror -I$(IDIR) -O2
LIBFLAGS = -lncurses
CC = gcc
OUTFILE = ../hamurabi

all:
	$(CC) $(CFLAGS) -o $(OUTFILE) ncurses_windows.c city.c main.c $(LIBFLAGS)

clean:
	rm -f *.o *.gch