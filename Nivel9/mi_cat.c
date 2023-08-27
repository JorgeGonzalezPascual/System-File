//Autores: Jorge González Pascual, Luis Clar Fiol
#include "directorios.h"

int main(int argc, char **argv)
{
    //Comprobamos que los parametros sean correctos
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis errónea: ./mi_cat <disco> </ruta_fichero>\n");
        return EXIT_FAILURE;
    }
    // Montar el dispositivo en el sistema.
    if (bmount(argv[1]) == EXIT_FAILURE)
    {
        fprintf(stderr, "mi_cat.c: Error al montar el dispositivo.\n");
        return EXIT_FAILURE;
    }

    //Obtenemos los parámetros de argv
    int tambuffer = BLOCKSIZE * 4;
    int bytes_leidos = 0;
    int bytes_leidosAux = 0;
    int offset = 0;
    char *camino = argv[2]; // path
    char buffer[tambuffer];

    memset(buffer, 0, sizeof(buffer));

    //Leemos todo el fichero o hasta completar el buffer
    bytes_leidosAux = mi_read(camino, buffer, offset, tambuffer);
    while (bytes_leidosAux > 0)
    {
        //Actualiza el número de bytes leidos.
        bytes_leidos += bytes_leidosAux;

        //Escribe el contenido del buffer en el destino indicado.
        write(1, buffer, bytes_leidosAux); // imprime pr pantalla

        //Limpia el buffer de lectura, actualiza el offset y vuelve a leer.
        memset(buffer, 0, sizeof(buffer));
        offset += tambuffer;
        //fprintf(stderr, " \n");
        bytes_leidosAux = mi_read(camino, buffer, offset, tambuffer);
    }

    fprintf(stderr, " \n");
    if (bytes_leidos < 0)
    {
        mostrar_error_buscar_entrada(bytes_leidos);
        bytes_leidos = 0;
    }
    fprintf(stderr, "Total_leidos: %d\n", bytes_leidos);

    // Desmonta el dispositivo del sistema
    bumount();
    return EXIT_SUCCESS;
}