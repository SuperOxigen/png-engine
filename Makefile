#	Image-Formats Makefile
#
#	Copyright (C) 2018 Alex Dale
#	See LICENSE for details.

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -D_DEBUG

.PHONY: all clean
.DEFAULT_GOAL := all

BASE_INC = src/base.h src/engine.h

DEBUG_INC = src/debuggable.h src/logger.h $(BASE_INC)

# Base modules

obj/engine.o: src/engine.c $(BASE_INC)
	@mkdir -p obj
	@echo "[ CC ] src/engine.c -> obj/engine.o"
	@$(CC) $(CFLAGS) -o obj/engine.o -c src/engine.c

# Debug Modules

obj/logger.o: src/logger.c $(DEBUG_INC)
	@mkdir -p obj
	@echo "[ CC ] src/logger.c -> obj/logger.o"
	@$(CC) $(CFLAGS) -o obj/logger.o -c src/logger.c

# PNG Modules

obj/chunk.o: src/chunk.c src/chunk.h $(BASE_INC)
	@mkdir -p obj
	@echo "[ CC ] src/chunk.c -> obj/chunk.o"
	@$(CC) $(CFLAGS) -o obj/chunk.o -c src/chunk.c

obj/ihdr.o: src/ihdr.c src/ihdr.h $(BASE_INC)
	@mkdir -p obj
	@echo "[ CC ] src/ihdr.c -> obj/ihdr.o"
	@$(CC) $(CFLAGS) -o obj/ihdr.o -c src/ihdr.c

obj/plte.o: src/plte.c src/plte.h $(BASE_INC)
	@mkdir -p obj
	@echo "[ CC ] src/plte.c -> obj/plte.o"
	@$(CC) $(CFLAGS) -o obj/plte.o -c src/plte.c

OBJS = obj/chunk.o obj/engine.o obj/ihdr.o obj/plte.o obj/logger.o

bin/img.exe: $(OBJS) src/main.c
	@mkdir -p bin
	@echo "[ CC ] src/main.c" $(OBJS) " -> bin/img.exe"
	@$(CC) $(CFLAGS) -o bin/img.exe $(OBJS) src/main.c


all: bin/img.exe

clean:
	@echo "[ RM ] bin/ obj/"
	@rm -rf bin/ obj/
