//Autores: Jorge González Pascual, Luis Clar Fiol
#include "ficheros.h"

/**
* ---------------------------------------------------------------------
*                          permitir.c:
* ---------------------------------------------------------------------
* 
* Programa externo ficticio sólo para probar, temporalmente, 
* la funcionalidad de cambio de permisos.
* 
* Llamada a mi_chmod_f() con los argumentos recibidos, convertidos a entero
* 
*/

int main(int argc, char *argv[])
{
    int ninodo = atoi(argv[2]);
    int permisos = atoi(argv[3]);
    
    if (argc != 4)
    {
        fprintf(stderr, "Sintaxis: permitir <nombre_dispositivo> <ninodo> <permisos>\n");
        return EXIT_FAILURE;
    }

    // Monta el dispositivo en el sistema.
    if (bmount(argv[1]) == EXIT_FAILURE)
    {
        fprintf(stderr, "permitir.c: Error montando el dispositivo en el sistema.\n");
        return EXIT_FAILURE;
    }

    //Cambio de permisos
    if (mi_chmod_f(ninodo, permisos))
    {
        fprintf(stderr, "permitir.c: Error con la modificacion de los permisos.\n");
        return EXIT_FAILURE;
    }

    // Desmonta el dispositivo virtual
    if (bumount() == EXIT_FAILURE)
    {
        fprintf(stderr, "permitir.c: Error al desmonta el dispositivo virtual.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}