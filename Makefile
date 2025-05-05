# Nombre base del ejecutable
TARGET = test

# Detectar sistema operativo (añade .exe en Windows)
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
endif

CC = gcc
CFLAGS = -Wall -O2
OBJ = main.o mergesort.o diskio.o utils.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o test test.exe entrada.bin entrada_tmp.bin run_*.bin run_nivel*.bin
