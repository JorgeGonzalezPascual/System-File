#include "ficheros.h"

/**
 * Son programas externos ficticios, sólo para probar, temporalmente, 
 * las funcionalidades de lectura/escritura y cambio de permisos, 
 * que involucran funciones de las 3 capas inferiores de nuestra 
 * biblioteca del sistema de ficheros, pero estos programas 
 * NO forman parte del sistema de ficheros.
 * 
 * Programa que comprueba si el número de argumentos es correcto y
 * en caso contrario mostrar la sintaxis.
 * 
 * ---------------------------------------------------------------------
 *                          permitir.c:
 * ---------------------------------------------------------------------
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