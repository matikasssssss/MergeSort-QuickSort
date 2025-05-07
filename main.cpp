#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include "utils.h"
#include "mergesort.h"
#include "diskio.h"

void verificar_orden(const std::string &nombre_archivo)
{
    std::ifstream f(nombre_archivo, std::ios::binary);
    if (!f.is_open())
    {
        std::cerr << "No se pudo abrir el archivo " << nombre_archivo << "\n";
        return;
    }

    int64_t anterior = 0, actual = 0;
    if (!f.read(reinterpret_cast<char *>(&anterior), sizeof(int64_t)))
    {
        std::cout << "Archivo vacío o error de lectura.\n";
        return;
    }

    size_t pos = 1;
    while (f.read(reinterpret_cast<char *>(&actual), sizeof(int64_t)))
    {
        if (actual < anterior)
        {
            std::cout << "ERROR: Datos no ordenados en la posición "
                      << pos << ": " << anterior << " > " << actual << "\n";
            return;
        }
        anterior = actual;
        pos++;
    }

    std::cout << "¡Archivo ordenado correctamente!\n";
}

int main()
{
    size_t size_in_mb = 8 * 50;
    generate_binary_file("entrada.bin", size_in_mb);

    cont_lecturas = 0;
    cont_escrituras = 0;

    mergesort_externo("entrada.bin", BLOCK_SIZE, 172);

    std::cout << "Lecturas: " << cont_lecturas << "\n";
    std::cout << "Escrituras: " << cont_escrituras << "\n";

    verificar_orden("entrada_tmp.bin");

    return 0;
}
