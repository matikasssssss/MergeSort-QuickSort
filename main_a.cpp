#include <iostream>
#include <fstream>
#include "utils.h"
#include "diskio.h"
#include "mergesort.h"
#include "buscar_a_optimo.h"

#define ARCHIVO_PRUEBA "entrada.bin"

using namespace std;

int main()
{
    const size_t size_in_mb = 4 * 50;
    cout << " Generando archivo binario de prueba (" << size_in_mb << " MB)...\n";
    generate_binary_file(ARCHIVO_PRUEBA, size_in_mb);

    cout << " Buscando aridad optima (a) mediante búsqueda ternaria...\n";
    size_t a_optimo = buscar_a_optimo(ARCHIVO_PRUEBA, BLOCK_SIZE);

    cout << " Aridad optima encontrada: a = " << a_optimo << endl;

    return 0;
}
