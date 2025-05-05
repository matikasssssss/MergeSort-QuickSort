#include "mergesort.h"
#include "diskio.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

void intercambiar_elementos(std::vector<int64_t> &arr, size_t i, size_t j)
{
    std::swap(arr[i], arr[j]);
}

bool comparar_int64(int64_t a, int64_t b)
{
    return a < b;
}

size_t crear_runs_iniciales(std::ifstream &entrada, size_t elementos_totales, size_t a, std::vector<std::string> &nombres_runs)
{
    size_t elementos_por_run = a * ELEMENTS_PER_BLOCK;
    size_t num_runs = (elementos_totales + elementos_por_run - 1) / elementos_por_run;

    std::vector<int64_t> buffer(elementos_por_run);

    for (size_t i = 0; i < num_runs; i++)
    {
        size_t elementos_a_leer = std::min(elementos_por_run, elementos_totales - i * elementos_por_run);
        size_t bloques_a_leer = (elementos_a_leer + ELEMENTS_PER_BLOCK - 1) / ELEMENTS_PER_BLOCK;
        size_t bloque_inicio = i * a;

        for (size_t j = 0; j < bloques_a_leer; j++)
        {
            size_t offset = j * ELEMENTS_PER_BLOCK;
            size_t restantes = elementos_a_leer - offset;
            size_t cantidad = std::min(restantes, (size_t)ELEMENTS_PER_BLOCK);

            leer_bloque(entrada, buffer.data() + offset, bloque_inicio + j, cantidad);
        }

        std::sort(buffer.begin(), buffer.begin() + elementos_a_leer, comparar_int64);

        std::string nombre_run = "run_" + std::to_string(i) + ".bin";
        nombres_runs.push_back(nombre_run);

        std::ofstream f_run(nombre_run, std::ios::binary);
        for (size_t j = 0; j < bloques_a_leer; j++)
        {
            size_t offset = j * ELEMENTS_PER_BLOCK;
            size_t restantes = elementos_a_leer - offset;
            size_t cantidad = std::min(restantes, (size_t)ELEMENTS_PER_BLOCK);

            escribir_bloque(f_run, buffer.data() + offset, j, cantidad);
        }
    }

    return num_runs;
}

struct RunEntrada
{
    std::ifstream f;
    int64_t elementos_restantes = 0;
    std::vector<int64_t> buffer;
    size_t pos = 0;
    size_t elementos_validos = 0;
    size_t bloque_actual = 0;
    bool activo = true;
};

void mezclar_archivos(const std::vector<std::string> &input_names, const std::string &output_name)
{
    size_t count = input_names.size();
    std::vector<RunEntrada> entradas(count);
    size_t tam_bloque = ELEMENTS_PER_BLOCK;

    for (size_t i = 0; i < count; i++)
    {
        entradas[i].f.open(input_names[i], std::ios::binary);
        entradas[i].f.seekg(0, std::ios::end);
        size_t tam_archivo = entradas[i].f.tellg();
        entradas[i].f.seekg(0);

        entradas[i].elementos_restantes = tam_archivo / sizeof(int64_t);
        entradas[i].buffer.resize(tam_bloque);

        size_t cantidad = std::min((int64_t)tam_bloque, entradas[i].elementos_restantes);
        leer_bloque(entradas[i].f, entradas[i].buffer.data(), entradas[i].bloque_actual, cantidad);
        entradas[i].elementos_validos = cantidad;
        entradas[i].elementos_restantes -= cantidad;
        entradas[i].bloque_actual++;
    }

    std::ofstream out(output_name, std::ios::binary);
    std::vector<int64_t> out_buffer(tam_bloque);
    size_t out_pos = 0;
    size_t bloques_escritos = 0;

    while (true)
    {
        int min_idx = -1;
        for (size_t i = 0; i < count; ++i)
        {
            if (entradas[i].activo && entradas[i].pos < entradas[i].elementos_validos)
            {
                if (min_idx == -1 || entradas[i].buffer[entradas[i].pos] < entradas[min_idx].buffer[entradas[min_idx].pos])
                {
                    min_idx = i;
                }
            }
        }

        if (min_idx == -1)
            break;

        out_buffer[out_pos++] = entradas[min_idx].buffer[entradas[min_idx].pos++];

        if (out_pos == tam_bloque)
        {
            escribir_bloque(out, out_buffer.data(), bloques_escritos++, out_pos);
            out_pos = 0;
        }

        if (entradas[min_idx].pos == entradas[min_idx].elementos_validos)
        {
            if (entradas[min_idx].elementos_restantes > 0)
            {
                size_t cantidad = std::min((int64_t)tam_bloque, entradas[min_idx].elementos_restantes);
                leer_bloque(entradas[min_idx].f, entradas[min_idx].buffer.data(), entradas[min_idx].bloque_actual, cantidad);
                entradas[min_idx].elementos_validos = cantidad;
                entradas[min_idx].elementos_restantes -= cantidad;
                entradas[min_idx].pos = 0;
                entradas[min_idx].bloque_actual++;
            }
            else
            {
                entradas[min_idx].activo = false;
            }
        }
    }

    if (out_pos > 0)
    {
        escribir_bloque(out, out_buffer.data(), bloques_escritos++, out_pos);
    }
}

void mezclar_runs(std::vector<std::string> &nombres_runs, size_t a, int nivel)
{
    if (nombres_runs.size() == 1)
    {
        std::rename(nombres_runs[0].c_str(), "entrada_tmp.bin");
        return;
    }

    size_t nuevos_runs = (nombres_runs.size() + a - 1) / a;
    std::vector<std::string> nuevos_nombres;

    for (size_t i = 0; i < nuevos_runs; i++)
    {
        size_t inicio = i * a;
        size_t fin = std::min(inicio + a, nombres_runs.size());

        std::string nombre_out = "run_nivel" + std::to_string(nivel) + "_" + std::to_string(i) + ".bin";
        nuevos_nombres.push_back(nombre_out);

        std::vector<std::string> subset(nombres_runs.begin() + inicio, nombres_runs.begin() + fin);
        mezclar_archivos(subset, nombre_out);

        for (size_t j = inicio; j < fin; j++)
        {
            std::remove(nombres_runs[j].c_str());
        }
    }

    mezclar_runs(nuevos_nombres, a, nivel + 1);
}

void mergesort_externo(const std::string &nombre_archivo, size_t M, size_t a)
{
    std::ifstream entrada(nombre_archivo, std::ios::binary);
    if (!entrada)
    {
        std::perror("fopen");
        exit(1);
    }

    entrada.seekg(0, std::ios::end);
    size_t tam_archivo = entrada.tellg();
    size_t elementos_totales = tam_archivo / sizeof(int64_t);
    entrada.seekg(0);

    std::vector<std::string> nombres_runs;
    crear_runs_iniciales(entrada, elementos_totales, a, nombres_runs);
    entrada.close();

    mezclar_runs(nombres_runs, a, 0);
}
