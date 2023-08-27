//Jorge Gonzáñez Pascual, Luis Clar Font
#include "ficheros_basico.h"

int main(int argc, char **argv)
{

    //Comprobamos que los parametros sean correctos
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis errónea: ./mi_fks <nombre del fichero> <numero de bloques>\n");
        return EXIT_FAILURE;
    }

    //Obtenemos los parámetros de argv
    char *camino = argv[1];
    int nbloques = atoi(argv[2]);
    int ninodos = nbloques / 4;
    //Inicializamos el buffer
    unsigned char buf[BLOCKSIZE];
    if (!memset(buf, 0, BLOCKSIZE))
    {
        return EXIT_FAILURE;
    }

    //Montaje
    if (bmount(camino) == EXIT_FAILURE)
    {
        fprintf(stderr, "Error al montar el dispositivo virtual.\n");
        return EXIT_FAILURE;
    }

    //Escritura
    for (int i = 0; i < nbloques; i++)
    {
        if (bwrite(i, buf) == EXIT_FAILURE)
        {
            fprintf(stderr, "Error al escribir en el dispositivo virtual en la posición %i\n", i);
            return EXIT_FAILURE;
        }
    }

    //Inicialización de metadatos
    if (initSB(nbloques, ninodos) == EXIT_FAILURE)
    {
        fprintf(stderr, "Error generando el superbloque en el dispositivo virtual.\n");
        return EXIT_FAILURE;
    }
    if (initMB() == EXIT_FAILURE)
    {
        fprintf(stderr, "Error en la generación del mapa de bits del dispositivo virtual.\n");
        return EXIT_FAILURE;
    }
    if (initAI() == EXIT_FAILURE)
    {
        fprintf(stderr, "Error en la generación el array de inodos del dispositivo.\n");
        return EXIT_FAILURE;
    }

    //Creacion de direccorio raíz
    reservar_inodo('d', 7);

    //Liberación
    if (bumount() == EXIT_FAILURE)
    {
        fprintf(stderr, "Error al desmontar el dispositivo virtual.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}