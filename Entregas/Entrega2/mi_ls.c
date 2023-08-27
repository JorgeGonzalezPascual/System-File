//Autores: Jorge González Pascual, Luis Clar Fiol
#include "directorios.h"

/**
 * ---------------------------------------------------------------------
 *                          mi_ls.c:
 * ---------------------------------------------------------------------
 * 
 * Programa (comando) que lista el contenido de un directorio (nombres 
 * de las entradas), llamando a la función mi_dir() de la capa de 
 * directorios, que es quien construye el buffer que mostrará mi_ls.c. 
 * Indicaremos el total de entradas. 
 * 
*/

#define DEBUGGER 1

int main(int argc, char const *argv[])
{
    // Comprueba que la sintaxis sea correcta.
    if (argc != 3)
    {
        fprintf(stderr,
                "Error de sintaxis: ./mi_ls <disco></ruta_directorio>\n");
        return FAILURE;
    }

    // Monta el disco en el sistema.
    if (bmount(argv[1]) == FAILURE)
    {
        fprintf(stderr, "Error de montaje de disco.\n");
        return FAILURE;
    }
    char tipo = 'd';
    char buffer[TAMBUFFER];
    memset(buffer, 0, TAMBUFFER);
    int total;
    if ((argv[2][strlen(argv[2]) - 1] != '/')) //si no es un fichero
    {
        tipo = 'f';
    }
    if ((total = mi_dir(argv[2], buffer, tipo)) < 0)
    {
        return FAILURE;
    }
#if DEBUGGER
    printf("Total: %d\n\n", total);
#endif
    if (total > 0)
    {

        printf("Tipo\tModo\tmTime\t\t\tTamaño\tNombre\n");
        printf("----------------------------------------------------------"
               "----------------------\n");
        printf("%s\n", buffer);
    }
    bumount();
    return EXIT_SUCCESS;
}