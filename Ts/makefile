# testsrv linux makefile

CC = gcc
CFLAGS = -O2 -ffast-math -w -DGAME_INCLUDE
LDFLAGS=-ldl

OUTFILES = testsrv.o 

testsrv.so: $(OUTFILES)
	$(CC) $(OUTFILES) $(LDFLAGS) -o ts

testsrv.o: testsrv.c testsrv.h q_shared.h game.h
