# zbotdetect.so linux makefile

CC = gcc
CFLAGS = -O2 -ffast-math -w -DGAME_INCLUDE
#LDFLAGS=-ldl

OUTFILES = g_main.o

zbotdetect.so: $(OUTFILES)
	$(CC) $(OUTFILES) $(LDFLAGS) -shared -o zbotdetect.so

g_main.o: g_main.c g_local.h q_shared.h game.h

