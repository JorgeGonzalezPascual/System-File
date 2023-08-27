//Autores: Jorge González Pascual, Luis Clar Fiol
#include "directorios.h"

int main(int argc, char const *argv[])
{
    // Comprueba que la sintaxis sea correcta.
    if (argc != 4)
    {
        fprintf(stderr, "Error de sintaxis: ./mi_touch <disco><permisos></ruta>\n");
        return FAILURE;
    }

    // Comprobación de permisos
    if (atoi(argv[2]) < 0 || atoi(argv[2]) > 7)
    {
        fprintf(stderr, "mi_touch.c: Permisos no válidos.\n");
    }

    unsigned char permisos = atoi(argv[2]);

    if ((argv[3][strlen(argv[3]) - 1] != '/')) //Si no es un fichero
    {
        // Monta el disco en el sistema.
        if (bmount(argv[1]) == FAILURE)
        {
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
    else
    {
        fprintf(stderr, "No es una ruta de fichero válida\n");
    }
    return EXIT_SUCCESS;
}