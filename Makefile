CC = gcc
CFLAGS = -Wall -Wextra -O2
SRC = src/main.c
OUT = bin/sand_sim

# Cek OS untuk command yang sesuai
ifdef OS
   RM = del /Q
   MKDIR = if not exist bin mkdir bin
   EXT = .exe
else
   RM = rm -f
   MKDIR = mkdir -p bin
   EXT = 
endif

all: build run

build:
	$(MKDIR)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)$(EXT)

run:
	./$(OUT)$(EXT)

clean:
	$(RM) $(OUT)$(EXT)