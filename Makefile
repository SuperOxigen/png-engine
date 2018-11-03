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

obj/base.o: src/base.c $(BASE_INC)
	@mkdir -p obj
	@echo "[ CC ] src/base.c -> obj/base.o"
	@$(CC) $(CFLAGS) -o obj/base.o -c src/base.c

obj/engine.o: src/engine.c $(BASE_INC)
	@mkdir -p obj
	@echo "[ CC ] src/engine.c -> obj/engine.o"
	@$(CC) $(CFLAGS) -o obj/engine.o -c src/engine.c

BASE_OBJ = obj/base.o obj/engine.o

# Debug Modules

obj/logger.o: src/logger.c $(DEBUG_INC)
	@mkdir -p obj
	@echo "[ CC ] src/logger.c -> obj/logger.o"
	@$(CC) $(CFLAGS) -o obj/logger.o -c src/logger.c

DEBUG_OBJ = obj/logger.o

# PNG Modules

obj/chunk.o: src/chunk.c src/chunk.h $(BASE_INC)
	@mkdir -p obj
	@echo "[ CC ] src/chunk.c -> obj/chunk.o"
	@$(CC) $(CFLAGS) -o obj/chunk.o -c src/chunk.c

obj/imgchunk.o: src/imgchunk.c src/imgchunk.h $(BASE_INC)
	@mkdir -p obj
	@echo "[ CC ] src/imgchunk.c -> obj/imgchunk.o"
	@$(CC) $(CFLAGS) -o obj/imgchunk.o -c src/imgchunk.c

obj/clrchunk.o: src/clrchunk.c src/clrchunk.h $(BASE_INC)
	@mkdir -p obj
	@echo "[ CC ] src/clrchunk.c -> obj/clrchunk.o"
	@$(CC) $(CFLAGS) -o obj/clrchunk.o -c src/clrchunk.c

PNG_OBJ = obj/chunk.o obj/imgchunk.o obj/clrchunk.o

OBJS = $(BASE_OBJ) $(DEBUG_OBJ) $(PNG_OBJ)

bin/img.exe: $(OBJS) src/main.c
	@mkdir -p bin
	@echo "[ CC ] src/main.c" $(OBJS) " -> bin/img.exe"
	@$(CC) $(CFLAGS) -o bin/img.exe $(OBJS) src/main.c

all: bin/img.exe

clean:
	@echo "[ RM ] bin/ obj/"
	@rm -rf bin/ obj/
