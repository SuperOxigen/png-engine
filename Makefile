#	Image-Formats Makefile
#
#	Copyright (C) 2018 Alex Dale
#	See LICENSE for details.

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra
DEBUG_FLAGS = -g

.PHONY: all clean
.DEFAULT_GOAL := all

BASE_INC = src/base.h src/engine.h

obj/chunk.o: src/chunk.c src/chunk.h $(BASE_INC)
	@mkdir -p obj
	$(CC) $(CFLAGS) -o obj/chunk.o -c src/chunk.c

obj/engine.o: src/engine.c $(BASE_INC)
	@mkdir -p obj
	$(CC) $(CFLAGS) -o obj/engine.o -c src/engine.c

obj/ihdr.o: src/ihdr.c src/ihdr.h $(BASE_INC)
	@mkdir -p obj
	$(CC) $(CFLAGS) -o obj/ihdr.o -c src/ihdr.c

OBJS = obj/chunk.o obj/engine.o obj/ihdr.o

bin/img.exe: $(OBJS) src/main.c
	@mkdir -p bin
	$(CC) $(CFLAGS) -o bin/img.exe $(OBJS) src/main.c


all: bin/img.exe
