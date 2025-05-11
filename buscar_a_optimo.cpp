#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <cstdint>
#include <algorithm>
#include "mergesort.h"
#include "diskio.h"
#include "utils.h"
#include <set>

#define ARCHIVO_PRUEBA "entrada.bin"
#define ARCHIVO_COPIA "entrada_tmp.bin"

using namespace std;

/**
 * @brief Ejecuta el algoritmo Mergesort Externo sobre una copia del archivo dado con una aridad específica.
 *
 * Esta función:
 * 1. Copia el archivo original a un archivo temporal llamado "entrada_tmp.bin".
 * 2. Reinicia los contadores globales de lecturas y escrituras.
 * 3. Ejecuta `mergesort_externo()` con la aridad `a`.
 * 4. Retorna el número total de accesos a disco realizados durante la ejecución.
 *
 * Es utilizada como parte del proceso de búsqueda del mejor valor de aridad (`a`) para Mergesort externo.
 *
 * @param archivo Nombre del archivo binario original a ordenar.
 * @param a Aridad a usar en la ejecución de Mergesort Externo.
 *
 * @return Total de I/Os realizados (lecturas + escrituras) durante la ejecución.
 *         Devuelve `std::numeric_limits<size_t>::max()` si hay error al abrir archivos.
 *
 * @see mergesort_externo, cont_lecturas, cont_escrituras
 */
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

/**
 * @brief Busca el valor óptimo de aridad `a` para Mergesort Externo que minimiza la cantidad de I/Os.
 *
 * Aplica una búsqueda ternaria sobre el rango de valores posibles para `a`, evaluando
 * el rendimiento de `mergesort_externo()` para cada candidato. Compara el número total de I/Os
 * (lecturas + escrituras) y retorna el valor de `a` con mejor desempeño.
 *
 * Para evitar repeticiones innecesarias, se almacena un conjunto de valores ya probados.
 * Al final, se realiza una búsqueda lineal fina sobre el rango reducido.
 *
 * @param archivo Archivo binario de entrada sobre el cual se evalúan los diferentes valores de `a`.
 * @param B Tamaño del bloque en bytes. No se usa directamente en esta implementación, pero puede ser útil para extensiones.
 *
 * @return Valor de `a` que minimiza la cantidad de I/Os al ejecutar Mergesort Externo.
 *
 * @see ejecutar_con_a, mergesort_externo
 */
size_t buscar_a_optimo(const string &archivo, size_t B)
{
    size_t l = 2;
    size_t r = 512;

    size_t mejor_a = l;
    size_t min_ios = numeric_limits<size_t>::max();
    set<size_t> ya_probamos;

    while (r - l > 3)
    {
        size_t m1 = l + (r - l) / 3;
        size_t m2 = r - (r - l) / 3;

        cout << "   -> Rango actual: [" << l << ", " << r << "], m1 = " << m1 << ", m2 = " << m2 << endl;

        size_t io1 = numeric_limits<size_t>::max();
        if (!ya_probamos.count(m1))
        {
            io1 = ejecutar_con_a(archivo, m1);
            ya_probamos.insert(m1);
        }

        size_t io2 = numeric_limits<size_t>::max();
        if (!ya_probamos.count(m2))
        {
            io2 = ejecutar_con_a(archivo, m2);
            ya_probamos.insert(m2);
        }

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
        if (ya_probamos.count(a))
            continue;
        size_t ios = ejecutar_con_a(archivo, a);
        ya_probamos.insert(a);
        if (ios < min_ios)
        {
            min_ios = ios;
            mejor_a = a;
        }
    }

    return mejor_a;
}
