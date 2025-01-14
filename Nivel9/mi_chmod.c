//Autores: Jorge González Pascual, Luis Clar Fiol
#include "directorios.h"

int main(int argc, char const *argv[])
{

    unsigned char permisos;
    int r;

    if (argc != 4)
    {
        fprintf(stderr, "Error de sintaxis: ./mi_mkdir <disco> <permisos> </ruta>\n");
        return EXIT_FAILURE;
    }

    permisos = atoi(argv[2]);
    if (permisos > 7)
    {

        fprintf(stderr, "Error de sintaxis: los permisos son incorrectos.\n");

        return EXIT_FAILURE;
    }

    // Monta el disco en el sistema.
    if (bmount(argv[1]) == EXIT_FAILURE)
    {
        fprintf(stderr, "Error de montaje de disco.\n");
        return EXIT_FAILURE;
    }

    r = mi_chmod(argv[3], permisos);
    if (r < 0)
    {
        mostrar_error_buscar_entrada(r);
        return EXIT_FAILURE;
    }

    bumount();
    return EXIT_SUCCESS;
}