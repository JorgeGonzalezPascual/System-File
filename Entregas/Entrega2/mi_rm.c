//Autores: Jorge González Pascual, Luis Clar Fiol
#include "directorios.h"

/**
 * ---------------------------------------------------------------------
 *                          mi_rm.c
 * ---------------------------------------------------------------------
 * 
 * Programa mi_rm.c que borra un fichero o directorio, llamando a la 
 * función mi_unlink() de la capa de directorios.
 * 
*/

int main(int argc, char **argv)
{
    //Comprobamos que los parametros sean correctos
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis: ./mi_rm <disco> </ruta_fichero>\n");
        return FAILURE;
    }
    // Comprueba si es un directorio
/*     if ((argv[2][strlen(argv[2]) - 1] == '/'))
    {
        fprintf(stderr, "Error: La ruta no es un fichero.\n");
        return FAILURE;
    } */
    // Monta el dispositivo virtual en el sistema.
    if (bmount(argv[1]) < 0)
    {
        fprintf(stderr, "Error: al montar el dispositivo.\n");
        return FAILURE;
    }

    if (mi_unlink(argv[2]) < 0)
    {
        return FAILURE;
    }
    bumount();
    return EXIT_SUCCESS;
}