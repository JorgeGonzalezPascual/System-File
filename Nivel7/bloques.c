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
 *      EXIT_FAILURE
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
 *  Función: bumount()
 * ---------------------------------------------------------------------
 * Desmonta el dispositivo virtual.
 * 
 * In: -
 * Out: EXIT_SUCCESS
 *      EXIT_FAILURE
 */
int bumount()
{
    if (close(descriptor) == -1)
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
 * In:  nbloque: posición del bloque a escribir
 *      buf: puntero del contenido a escribir
 * Out: nº de bytes que ha podido escribir
 *      EXIT_FAILURE
 * 
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
            fprintf(stderr, "bwrite(): Error al escribir el bloque.\n");
            return EXIT_FAILURE;
        }

        return bytes;
    }
    else
    {
        fprintf(stderr, "bwrite(): Error al posicionarse en el fichero.\n");
        return EXIT_FAILURE;
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
 *      EXIT_FAILURE
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
            fprintf(stderr, "bread(): Error al leer el bloque.\n");
            return EXIT_FAILURE;
        }
        return bytes;
    }
    else
    {
        fprintf(stderr, "bread(): Error al posicionarse en el fichero.\n");
        return EXIT_FAILURE;
    }
}