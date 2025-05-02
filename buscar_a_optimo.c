#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "mergesort.h"
#include "diskio.h"
#include "utils.h"

#define ARCHIVO_PRUEBA "entrada.bin"
#define ARCHIVO_COPIA "entrada_tmp.bin"

size_t ejecutar_con_a(const char *archivo, size_t M, size_t a) {
    // Clonar archivo original (para que mergesort no lo destruya)
    FILE *in = fopen(archivo, "rb");
    FILE *out = fopen(ARCHIVO_COPIA, "wb");

    int64_t buffer[ELEMENTS_PER_BLOCK];
    size_t leidos;
    while ((leidos = fread(buffer, sizeof(int64_t), ELEMENTS_PER_BLOCK, in)) > 0) {
        fwrite(buffer, sizeof(int64_t), leidos, out);
    }

    fclose(in);
    fclose(out);

    // Calcular M real para que derive en 'a'
    size_t M_real = (a + 1) * BLOCK_SIZE;

    // Resetear contadores
    cont_lecturas = 0;
    cont_escrituras = 0;

    // Ejecutar mergesort
    mergesort_externo(ARCHIVO_COPIA, M_real);

    return cont_lecturas + cont_escrituras;
}

size_t buscar_a_optimo(const char *archivo, size_t B) {
    size_t l = 2;
    size_t r = B / sizeof(int64_t);  // b = B / 8

    size_t mejor_a = l;
    size_t min_ios = SIZE_MAX;

    while (r - l > 3) {
        size_t m1 = l + (r - l) / 3;
        size_t m2 = r - (r - l) / 3;

        size_t io1 = ejecutar_con_a(archivo, BLOCK_SIZE, m1);
        size_t io2 = ejecutar_con_a(archivo, BLOCK_SIZE, m2);

        if (io1 < io2) {
            r = m2 - 1;
            if (io1 < min_ios) {
                min_ios = io1;
                mejor_a = m1;
            }
        } else {
            l = m1 + 1;
            if (io2 < min_ios) {
                min_ios = io2;
                mejor_a = m2;
            }
        }
    }

    // Revisión final de los valores cercanos
    for (size_t a = l; a <= r; a++) {
        size_t ios = ejecutar_con_a(archivo, BLOCK_SIZE, a);
        if (ios < min_ios) {
            min_ios = ios;
            mejor_a = a;
        }
    }

    return mejor_a;
}
