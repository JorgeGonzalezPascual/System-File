//Autores: Jorge González Pascual, Luis Clar Fiol
#include "bloques.h"
static int descriptor = 0;

/**
 *  Función: bmount(const char *camino)
 * ---------------------------------------------------------------------
 * Función para montar el dispositivo virtual, y dado que se trata de 
 * un fichero, esa acción consistirá en abrirlo.
 *
 * In: camino = direccion del dispositivo virtual a montar
 * Out: descriptor
 *      FAILURE
 */
int bmount(const char *camino)
{
    umask(000);
    descriptor = open(camino, O_RDWR | O_CREAT, 0666);

    if (descriptor == FAILURE)
    {
        fprintf(stderr, "Error al abrir el fichero.\n");
        return FAILURE;
    }

    return descriptor;
}

/**
 *  Función: bumount()
 * ---------------------------------------------------------------------
 * Desmonta el dispositivo virtual.
 * 
 * In: -
 * Out: EXIT_SUCCESS
 *      FAILURE
 */
int bumount()
{
    if (close(descriptor) == FAILURE)
    {
        return FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 *  Función: bwrite:
 * ---------------------------------------------------------------------
 * Escribe un bloque en el dispositivo virtual, en el bloque
 * físico especificado por nbloque.
 * 
 * In:  nbloque: posición del bloque a escribir
 *      buf: puntero del contenido a escribir
 * Out: nº de bytes que ha podido escribir
 *      FAILURE
 * 
 */
int bwrite(unsigned int nbloque, const void *buf)
{
    //Colocar el puntero
    if (lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET) != FAILURE)
    {
        //Escribir bloque
        size_t bytes = write(descriptor, buf, BLOCKSIZE);

        //Si no se ha podido escribir
        if (bytes < 0)
        {
            fprintf(stderr, "bwrite(): Error al escribir el bloque.\n");
            return FAILURE;
        }

        return bytes;
    }
    else
    {
        fprintf(stderr, "bwrite(): Error al posicionarse en el fichero.\n");
        return FAILURE;
    }
}

/**
 * Función: bread:
 * ---------------------------------------------------------------------
 * Lee un bloque en el dispositivo virtual, en el bloque físico 
 * especificado por nbloque.
 * 
 * In:  nbloque: posición del bloque a leer
 *      buf: puntero del contenido a guardar
 * Out: nº de bytes que ha podido escribir
 *      FAILURE
 */
int bread(unsigned int nbloque, void *buf)
{
    //Colocar el puntero
    if (lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET) != FAILURE)
    {
        //Leer bloque
        size_t bytes = read(descriptor, buf, BLOCKSIZE);

        //Si no se ha podido leer
        if (bytes < 0)
        {
            fprintf(stderr, "bread(): Error al leer el bloque.\n");
            return FAILURE;
        }
        return bytes;
    }
    else
    {
        fprintf(stderr, "bread(): Error al posicionarse en el fichero.\n");
        return FAILURE;
    }
}