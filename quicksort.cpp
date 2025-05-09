#include "diskio.h"
#include "quicksort.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <cstdlib> // exit()
#include <cstdio>  // remove()

void quicksort_externo(const std::string &archivo, size_t M, size_t a, int nivel)
{
    std::ifstream entrada(archivo, std::ios::binary);
    if (!entrada)
    {
        std::perror("fopen");
        exit(1);
    }

    entrada.seekg(0, std::ios::end);
    size_t tam_archivo = entrada.tellg();
    size_t total_elementos = tam_archivo / sizeof(int64_t);
    entrada.seekg(0);

    size_t max_en_memoria = M / sizeof(int64_t);

    if (total_elementos <= max_en_memoria)
    {
        std::vector<int64_t> buffer(total_elementos);
        entrada.read(reinterpret_cast<char *>(buffer.data()), tam_archivo);
        if (!entrada)
        {
            std::cerr << "Error leyendo el archivo.\n";
            exit(1);
        }

        std::sort(buffer.begin(), buffer.end());
        entrada.close();

        std::ofstream salida(archivo, std::ios::binary);
        salida.write(reinterpret_cast<const char *>(buffer.data()), tam_archivo);
        if (!salida)
        {
            std::cerr << "Error escribiendo el archivo.\n";
            exit(1);
        }
        return;
    }

    // Selección de pivotes
    std::vector<int64_t> muestra(ELEMENTS_PER_BLOCK);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, total_elementos / ELEMENTS_PER_BLOCK - 1);
    size_t bloque_aleatorio = dis(gen);
    leer_bloque(entrada, muestra.data(), bloque_aleatorio, ELEMENTS_PER_BLOCK);

    std::shuffle(muestra.begin(), muestra.end(), gen);
    std::vector<int64_t> pivotes(muestra.begin(), muestra.begin() + a - 1);
    std::sort(pivotes.begin(), pivotes.end());

    entrada.seekg(0);

    // Archivos de salida por partición
    std::vector<std::ofstream> salidas(a);
    std::vector<std::string> nombres_partes(a);
    for (size_t i = 0; i < a; ++i)
    {
        nombres_partes[i] = "quick_part_" + std::to_string(nivel) + "_" + std::to_string(i) + ".bin";
        salidas[i].open(nombres_partes[i], std::ios::binary);
        if (!salidas[i])
        {
            std::cerr << "Error creando archivo de salida " << nombres_partes[i] << "\n";
            exit(1);
        }
    }

    // Buffers por partición con flushing a disco
    std::vector<std::vector<int64_t>> buffers_tmp(a);
    for (size_t i = 0; i < a; ++i)
        buffers_tmp[i].reserve(ELEMENTS_PER_BLOCK);

    std::vector<int64_t> buffer(ELEMENTS_PER_BLOCK);
    size_t bloques = (total_elementos + ELEMENTS_PER_BLOCK - 1) / ELEMENTS_PER_BLOCK;

    for (size_t i = 0; i < bloques; ++i)
    {
        size_t cantidad = std::min(ELEMENTS_PER_BLOCK, total_elementos - i * ELEMENTS_PER_BLOCK);
        leer_bloque(entrada, buffer.data(), i, cantidad);

        for (size_t j = 0; j < cantidad; ++j)
        {
            int64_t x = buffer[j];
            size_t idx = std::lower_bound(pivotes.begin(), pivotes.end(), x) - pivotes.begin();
            buffers_tmp[idx].push_back(x);

            if (buffers_tmp[idx].size() == ELEMENTS_PER_BLOCK)
            {
                salidas[idx].write(reinterpret_cast<char *>(buffers_tmp[idx].data()), ELEMENTS_PER_BLOCK * sizeof(int64_t));
                cont_escrituras++;
                buffers_tmp[idx].clear();
            }
        }
    }

    entrada.close();

    // Vaciar buffers restantes
    for (size_t i = 0; i < a; ++i)
    {
        if (!buffers_tmp[i].empty())
        {
            escribir_bloque(salidas[i], buffers_tmp[i].data(), -1, buffers_tmp[i].size());
            cont_escrituras++;
            buffers_tmp[i].clear();
        }
        salidas[i].close();
    }

    // Ordenar recursivamente
    for (size_t i = 0; i < a; ++i)
        quicksort_externo(nombres_partes[i], M, a, nivel + 1);

    // Concatenar particiones
    std::ofstream salida_final(archivo, std::ios::binary);
    std::vector<int64_t> out_buffer(ELEMENTS_PER_BLOCK);

    for (size_t i = 0; i < a; ++i)
    {
        std::ifstream in(nombres_partes[i], std::ios::binary);
        in.seekg(0, std::ios::end);
        size_t tam = in.tellg();
        size_t elementos = tam / sizeof(int64_t);
        in.seekg(0);

        size_t bloques_leer = (elementos + ELEMENTS_PER_BLOCK - 1) / ELEMENTS_PER_BLOCK;
        for (size_t j = 0; j < bloques_leer; ++j)
        {
            size_t cantidad = std::min(ELEMENTS_PER_BLOCK, elementos - j * ELEMENTS_PER_BLOCK);
            leer_bloque(in, out_buffer.data(), j, cantidad);
            escribir_bloque(salida_final, out_buffer.data(), -1, cantidad);
        }

        in.close();
        std::remove(nombres_partes[i].c_str());
    }
}
