#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <cstdint>
#include <algorithm>
#include "mergesort.h"
#include "diskio.h"
#include "utils.h"

#define ARCHIVO_PRUEBA "entrada.bin"
#define ARCHIVO_COPIA "entrada_tmp.bin"

using namespace std;

size_t ejecutar_con_a(const string &archivo, size_t a)
{
    ifstream in(archivo, ios::binary);
    ofstream out("entrada_tmp.bin", ios::binary);

    if (!in || !out)
    {
        cerr << "Error al abrir archivos." << endl;
        return numeric_limits<size_t>::max();
    }

    // Copia el archivo original al temporal
    vector<int64_t> buffer(ELEMENTS_PER_BLOCK);
    while (in.read(reinterpret_cast<char *>(buffer.data()), ELEMENTS_PER_BLOCK * sizeof(int64_t)) || in.gcount() > 0)
    {
        out.write(reinterpret_cast<char *>(buffer.data()), in.gcount());
    }

    in.close();
    out.close();

    // Reinicia contadores
    cont_lecturas = 0;
    cont_escrituras = 0;

    // Mensaje antes de ejecutar
    cout << "  > Probando a = " << a << "..." << flush;

    mergesort_externo("entrada_tmp.bin", a);

    // Mostrar resultados
    cout << " Lecturas: " << cont_lecturas
         << ", Escrituras: " << cont_escrituras
         << " (Total I/Os: " << cont_lecturas + cont_escrituras << ")" << endl;

    return cont_lecturas + cont_escrituras;
}

size_t buscar_a_optimo(const string &archivo, size_t B)
{
    size_t l = 2;
    size_t r = 512;

    size_t mejor_a = l;
    size_t min_ios = numeric_limits<size_t>::max();

    while (r - l > 3)
    {
        size_t m1 = l + (r - l) / 3;
        size_t m2 = r - (r - l) / 3;

        size_t io1 = ejecutar_con_a(archivo, m1);
        size_t io2 = ejecutar_con_a(archivo, m2);

        if (io1 < io2)
        {
            r = m2 - 1;
            if (io1 < min_ios)
            {
                min_ios = io1;
                mejor_a = m1;
            }
        }
        else
        {
            l = m1 + 1;
            if (io2 < min_ios)
            {
                min_ios = io2;
                mejor_a = m2;
            }
        }
    }

    for (size_t a = l; a <= r; ++a)
    {
        size_t ios = ejecutar_con_a(archivo, a);
        if (ios < min_ios)
        {
            min_ios = ios;
            mejor_a = a;
        }
    }

    return mejor_a;
}
