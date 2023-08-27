//Autores: Jorge González Pascual, Luis Clar Fiol
#include "directorios.h"

/**
 * ---------------------------------------------------------------------
 *                          mi_link.c
 * ---------------------------------------------------------------------
 * 
 * que crea un enlace a un fichero, llamando a la función mi_link() de 
 * la capa de directorios.
 * 
*/

int main(int argc, char **argv)
{
    //Comprobamos que los parametros sean correctos
    if (argc != 4)
    {
        fprintf(stderr, "Sintaxis errónea: ./mi_link disco /ruta_fichero_original /ruta_enlace\n");
        return FAILURE;
    }

    //si es un fichero
    if (argv[2][strlen(argv[2]) - 1] == '/')
    {
        fprintf(stderr, "Error: La ruta del fichero original no es un fichero\n");
        return FAILURE;
    }
    if (argv[3][strlen(argv[3]) - 1] == '/')
    {
        fprintf(stderr, "Error: La ruta de enlace no es un fichero\n");
        return FAILURE;
    }
    // Monta el dispositivo en el sistema.
    if (bmount(argv[1]) == FAILURE)
    {
        fprintf(stderr, "Error: al montar el dispositivo.\n");
        return FAILURE;
    }

    //Linkeamos
    if (mi_link(argv[2], argv[3]) < 0)
    {
        return FAILURE;
    }
    
    bumount();
    return EXIT_SUCCESS;
}