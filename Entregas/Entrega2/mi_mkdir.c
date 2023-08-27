//Autores: Jorge González Pascual, Luis Clar Fiol
#include "directorios.h"

/**
 * ---------------------------------------------------------------------
 *                          mi_mkdir.c:
 * ---------------------------------------------------------------------
 * 
 * Programa (comando) que crea un fichero o directorio, llamando a la 
 * función mi_creat(). Dependiendo de si la ruta acaba en / o no se 
 * estará indicando si hay que crear un directorio o un fichero.
 * 
*/

int main(int argc, char const *argv[])
{
    // Comprueba que la sintaxis sea correcta.
    if (argc != 4)
    {
        fprintf(stderr, "Error de sintaxis: ./mi_mkdir <disco> <permisos> </ruta>\n");
        return FAILURE;
    }

    if (atoi(argv[2]) < 0 || atoi(argv[2]) > 7)
    {
        fprintf(stderr, "Error: Modo <<%d>> inválido.\n", atoi(argv[2]));
        return FAILURE;
    }

    unsigned char permisos = atoi(argv[2]);

    if ((argv[3][strlen(argv[3]) - 1] == '/')) //si no es un fichero
    {
        // Monta el disco en el sistema.
        if (bmount(argv[1]) == FAILURE)
        {
            fprintf(stderr, "Error de montaje de disco.\n");
            return FAILURE;
        }
        int error;
        if ((error = mi_creat(argv[3], permisos)) < 0)
        {
            mostrar_error_buscar_entrada(error);
            return FAILURE;
        }

        bumount();
    }
    else //si es un directorio
    {
        fprintf(stderr, "No es una ruta de directorio válida. Se trata de un fichero.\n");
    }
    return EXIT_SUCCESS;
}