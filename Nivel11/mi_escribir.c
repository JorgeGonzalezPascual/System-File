//Autores: Jorge González Pascual, Luis Clar Fiol
#include "directorios.h"
#define DEBUGGER 1

int main(int argc, char const *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "Error de sintaxis: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>\n");
        return FAILURE;
    }
    //Comprabar que se trata de un fichero
    if ((argv[2][strlen(argv[2]) - 1]) == '/')
    {
        fprintf(stderr, "No es un fichero.\n");
        return FAILURE;
    }

    int bytes_escritos;
    // Monta el dispositivo virtual en el sistema.
    if (bmount(argv[1]) == FAILURE)
    {
        fprintf(stderr, "mi_escribir.c: Error al montar el dispositivo virtual.\n");
        return FAILURE;
    }

#if DEBUGGER
    fprintf(stderr, "Longitud texto: %ld\n", strlen(argv[3]));
#endif

    bytes_escritos = mi_write(argv[2], argv[3], atoi(argv[4]), strlen(argv[3]));
    if (bytes_escritos < 0)
    {
        mostrar_error_buscar_entrada(bytes_escritos);
        bytes_escritos = 0;
    }

    if (bumount() == FAILURE)
    {
        return FAILURE;
    }

#if DEBUGGER
    fprintf(stderr, "Bytes escritos: %d\n", bytes_escritos);
#endif

    return EXIT_SUCCESS;
}