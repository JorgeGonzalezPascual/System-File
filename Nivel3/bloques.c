#include "bloques.h"

static int descriptor = 0;

//-----------------------------------------------------------------------------
/**
 * OBSERVACIONES: Preguntar en el bread y bwrite 
 * si hace falta poner fprintf(stderr, "Error al abrir el fichero.\n");
 */
//-----------------------------------------------------------------------------

/**
 *  Función: bmount:
 * ---------------------------------------------------------------------
 * Función para montar el dispositivo virtual, y dado que se trata de 
 * un fichero, esa acción consistirá en abrirlo.
 *
 * 
 * Devuelve -1 (o EXIT_FAILURE) si ha habido error, o el descriptor 
 * obtenido si ha ido bien.
 */
int bmount(const char *camino)
{
    umask(000);
    descriptor = open(camino, O_RDWR | O_CREAT, 0666);

    if (descriptor == -1)
    {
        fprintf(stderr, "Error al abrir el fichero.\n");
        return EXIT_FAILURE;
    }

    return descriptor;
}

/**
 *  Función: bumount:
 * ---------------------------------------------------------------------
 * Desmonta el dispositivo virtual.
 * 
 * Devuelve 0 (o EXIT_SUCCESS) si se ha cerrado el fichero correctamente,
 * o -1 (o EXIT_FAILURE) en caso contrario.
 */
int bumount()
{
    int cerr = close(descriptor);

    if (cerr == -1)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 *  Función: bwrite:
 * ---------------------------------------------------------------------
 * Escribe un bloque en el dispositivo virtual, en el bloque
 * físico especificado por nbloque.
 * 
 * Devuelve el nº de bytes que ha podido escribir (si ha ido
 * bien, será BLOCKSIZE), o -1 (o EXIT_FAILURE) si se produce un error.
 */
int bwrite(unsigned int nbloque, const void *buf)
{
    //Colocar el puntero
    if (lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET) != -1)
    {
        //Escribir bloque
        size_t bytes = write(descriptor, buf, BLOCKSIZE);

        //Si no se ha podido escribir
        if (bytes < 0)
        {
            fprintf(stderr, "Error al escribir el bloque.\n");
            return EXIT_FAILURE;
        }

        return bytes;
    }
    else
    {
        fprintf(stderr, "Error al posicionarse en el fichero.\n");
        return EXIT_FAILURE;
    }
}

/**
 * Función: bread:
 * ---------------------------------------------------------------------
 * Lee un bloque en el dispositivo virtual, en el bloque físico 
 * especificado por nbloque.
 * 
 * Devuelve el nº de bytes que ha podido leer (si ha ido bien, 
 * será BLOCKSIZE),
 *  o -1 (o EXIT_FAILURE) si se produce un error.
 */
int bread(unsigned int nbloque, void *buf)
{
    //Colocar el puntero
    if (lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET) != -1)
    {
        //Leer bloque
        size_t bytes = read(descriptor, buf, BLOCKSIZE);

        //Si no se ha podido leer
        if (bytes < 0)
        {
            fprintf(stderr, "Error al leer el bloque.\n");
            return EXIT_FAILURE;
        }
        return bytes;
    }
    else
    {
        fprintf(stderr, "Error al posicionarse en el fichero.\n");
        return EXIT_FAILURE;
    }
}