
# Makefile - Tarea 1: Mergesort vs Quicksort Externo

CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall

# Archivos fuente comunes
SRC_COMMON = diskio.cpp utils.cpp mergesort.cpp

# Objetivos
all: buscar_a experimento

buscar_a: main_buscar_a.cpp buscar_a_optimo.cpp $(SRC_COMMON)
	$(CXX) $(CXXFLAGS) -o $@ $^

experimento: main_experimentos.cpp quicksort.cpp $(SRC_COMMON)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f buscar_a experimento *.o *.bin resultados.txt

.PHONY: all clean
