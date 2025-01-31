#include "ficheros.h"
#define DEBUGGER 0

/**
 * Función: int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes);
 * ---------------------------------------------------------------------
 * Escribe el contenido procedente de un buffer en el disco virtual.
 * 
 * In:  ninodo: Posicion del inodo del array del inodo
 *      buf_original: Contenido a escrbir
 *      offset: Posición de escritura inicial
 *      nbytes: Cantidad de bytes a escribir
 * Out: bytesescritos: Cantidad de bytes escritos
 *      Error: EXIT_FAILURE
 */
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes)
{
    //Declaraciones
    unsigned int primerBL, ultimoBL;
    int desp1, desp2, nbfisico;
    int bytesescritos = 0;
    int auxByteEscritos = 0;
    char unsigned buf_bloque[BLOCKSIZE];
    struct inodo inodo;

    //Leer el inodo.
    if (leer_inodo(ninodo, &inodo) == EXIT_FAILURE)
    {
        fprintf(stderr, "Error in mi_write_f(): leer_inodo() \n");
        return EXIT_FAILURE;
    }

    //Comprobamos que el inodo tenga los permisos para escribir
    if ((inodo.permisos & 2) != 2)
    {
        fprintf(stderr, "El inodo no tiene permisos para escribir en mi_write_f() \n");
        return EXIT_FAILURE;
    }

    //Asignaciones de las variables.
    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;

    desp1 = offset % BLOCKSIZE;
    desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    //Obtencion del numero de bloque fisico
    nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
    if (nbfisico == -1)
    {
        fprintf(stderr, "Error in mi_write_f(): traducir_bloque_inodo()\n");
        return EXIT_FAILURE;
    }

    //Leemos el bloque fisico
    if (bread(nbfisico, buf_bloque) == EXIT_FAILURE)
    {
        fprintf(stderr, "Error in mi_write_f(): bread()\n");
        return EXIT_FAILURE;
    }

    //Caso en el que lo que queremos escribir cabe en un bloque fisico
    if (primerBL == ultimoBL)
    {
        memcpy(buf_bloque + desp1, buf_original, nbytes);
 
        //Escribimos el bloque fisico en el disco
        auxByteEscritos = bwrite(nbfisico, buf_bloque);
        if (auxByteEscritos == EXIT_FAILURE)
        {
            fprintf(stderr, "Error in mi_write_f(): bwrite()\n");
            return EXIT_FAILURE;
        }
        bytesescritos += nbytes;
    }

    //Caso en el que la escritura ocupa mas de un bloque fisico
    else if (primerBL < ultimoBL)
    {
        //Parte 1: Primero bloque escrito parcialmente
        memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);

        //Escribimos el bloque fisico en el disco
        auxByteEscritos = bwrite(nbfisico, buf_bloque);
        if (auxByteEscritos == EXIT_FAILURE)
        {
            fprintf(stderr, "Error in mi_write_f(): bwrite()\n");
            return EXIT_FAILURE;
        }
        bytesescritos += auxByteEscritos - desp1;

        //Parte 2: Bloques intermedios
        for (int i = primerBL + 1; i < ultimoBL; i++)
        {
            //Obtenemos los bloques intermedios
            nbfisico = traducir_bloque_inodo(ninodo, i, 1);
            if (nbfisico == -1)
            {
                fprintf(stderr, "Error in mi_write_f(): traducir_bloque_inodo()\n");
                return EXIT_FAILURE;
            }

            //Escribimos los bloques intermedios
            auxByteEscritos = bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE);
            if (auxByteEscritos == EXIT_FAILURE)
            {
                fprintf(stderr, "Error in mi_write_f(): bwrite()\n");
                return EXIT_FAILURE;
            }
            bytesescritos += auxByteEscritos;
        }

        //Parte 3: Último bloque escrito parcialmente
        /// EN CASO DE FALLO bytesescritos += offset;
        //Obtenemos el bloque fisico
        nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 1);
        if (nbfisico == -1)
        {
            fprintf(stderr, "Error in mi_write_f(): traducir_bloque_inodo()\n");
            return EXIT_FAILURE;
        }
        //Leemos el bloque fisico
        if (bread(nbfisico, buf_bloque) == EXIT_FAILURE)
        {
            fprintf(stderr, "Error in mi_write_f(): bread()\n");
            return EXIT_FAILURE;
        }

        memcpy(buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);

        auxByteEscritos = bwrite(nbfisico, buf_bloque);
        if (auxByteEscritos == EXIT_FAILURE)
        {
            fprintf(stderr, "Error in mi_write_f(): bwrite()\n");
            return EXIT_FAILURE;
        }

        bytesescritos += desp2 + 1;
    }

    //Leer el inodo actualizado.
    if (leer_inodo(ninodo, &inodo) == EXIT_FAILURE)
    {
        fprintf(stderr, "Error in leer_inodo(): mi_write_f() \n");
        return EXIT_FAILURE;
    }

    //Actualizar la metainformación

    //Comprobar si lo que hemos escrito es mas grande que el fichero
    if (inodo.tamEnBytesLog < (nbytes + offset))
    {
        inodo.tamEnBytesLog = nbytes + offset;
        inodo.ctime = time(NULL);
    }

    inodo.mtime = time(NULL);

    if (escribir_inodo(ninodo, inodo) == EXIT_FAILURE)
    {
        fprintf(stderr, "Error in escribir_inodo(): mi_write_f() \n");
        return EXIT_FAILURE;
    }

    //Comprobar que no haya errores de escritura y que se haya escrito todo bien.
    if (nbytes == bytesescritos)
    {
#if DEBUGGER
        printf("\tmi_write_f: BIEN\n");
        printf("\tmi_read_f(): nbfisico = %i\n", nbfisico);

#endif
        return bytesescritos;
    }
    else
    {
#if DEBUGGER
        printf("mi_write_f: MAL\n\tnbytes:%i\n\tbytesescritos:%i\n", nbytes, bytesescritos);
#endif
        return EXIT_FAILURE;
    }
}

/**
 * Función: int mi_read_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes);
 * ---------------------------------------------------------------------
 * Escribe el contenido procedente de un buffer en el disco virtual.
 * 
 * In:  ninodo: Posicion del inodo del array del inodo
 *      buf_original: Contenido a escrbir
 *      offset: Posición de lectura inicial
 *      nbytes: Cantidad de bytes a leer
 * Out: bytesescritos: Cantidad de bytes bytesleidos
 */
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
{
    //Declaraciones
    unsigned int primerBL, ultimoBL;
    int desp1, desp2, nbfisico;
    int bytesleidos = 0;
    int auxByteLeidos = 0;
    char unsigned buf_bloque[BLOCKSIZE];
    struct inodo inodo;

    //Leer el inodo.
    if (leer_inodo(ninodo, &inodo) == EXIT_FAILURE)
    {
        fprintf(stderr, "Error in mi_read_f(): leer_inodo()\n");
        return bytesleidos;
    }

    //Comprobamos que el inodo tenga los permisos para leer
    if ((inodo.permisos & 4) != 4)
    {
        fprintf(stderr, "No hay permisos de lectura\n");
        return bytesleidos;
    }

    if (offset >= inodo.tamEnBytesLog)
    {
        // No podemos leer nada
        return bytesleidos;
    }

    if ((offset + nbytes) >= inodo.tamEnBytesLog)
    { // pretende leer más allá de EOF
        nbytes = inodo.tamEnBytesLog - offset;
        // leemos sólo los bytes que podemos desde el offset hasta EOF
    }

    //Asignaciones de las variables.
    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;

    desp1 = offset % BLOCKSIZE;
    desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    //Obtencion del numero de bloque fisico
    nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
    //Caso el cual lo que queremos leer cabe en un bloque fisico
    if (primerBL == ultimoBL)
    {
        if (nbfisico != -1)
        {
            //Leemos el bloque fisico del disco
            auxByteLeidos = bread(nbfisico, buf_bloque);
            if (auxByteLeidos == EXIT_FAILURE)
            {
                fprintf(stderr, "Error in mi_read_f(): bread()\n");
                return EXIT_FAILURE;
            }
            memcpy(buf_original, buf_bloque + desp1, nbytes);
        }
        
        bytesleidos = nbytes; 
    }

    //Caso en el que la lectura ocupa mas de un bloque fisico
    else if (primerBL < ultimoBL)
    {
        //Parte 1: Primero bloque leido parcialmente
        if (nbfisico != -1)
        {
            //Leemos el bloque fisico del disco
            auxByteLeidos = bread(nbfisico, buf_bloque);
            if (auxByteLeidos == EXIT_FAILURE)
            {
                fprintf(stderr, "Error in mi_read_f(): bread()\n");
                return EXIT_FAILURE;
            }
            memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
        }

        bytesleidos =  BLOCKSIZE - desp1;


        //Parte 2: Bloques intermedios
        for (int i = primerBL + 1; i < ultimoBL; i++)
        {
            //Obtenemos los bloques intermedios
            nbfisico = traducir_bloque_inodo(ninodo, i, 0);
            if (nbfisico != -1)
            {
                //Leemos el bloque fisico del disco
                auxByteLeidos = bread(nbfisico, buf_bloque);
                if (auxByteLeidos == EXIT_FAILURE)
                {
                    fprintf(stderr, "Error in mi_read_f(): bread()\n");
                    return EXIT_FAILURE;
                }
                memcpy(buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE, buf_bloque, BLOCKSIZE);
            }

            bytesleidos += BLOCKSIZE;
        }

        //Parte 3: Último bloque leido parcialmente
        //Obtenemos el bloque fisico
        nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 0);
        //Parte 1: Primero bloque leido parcialmente
        if (nbfisico != -1)
        {
            //Leemos el bloque fisico del disco
            auxByteLeidos = bread(nbfisico, buf_bloque);
            if (auxByteLeidos == EXIT_FAILURE)
            {
                fprintf(stderr, "Error in mi_read_f(): bread()\n");
                return EXIT_FAILURE;
            }
            //Calculamos el byte lógico del último bloque hasta donde hemos de leer
            memcpy(buf_original + (nbytes - desp2 - 1), buf_bloque, desp2 + 1);
        }

        bytesleidos += desp2 + 1;

    }

    //Leer el inodo actualizado.
    if (leer_inodo(ninodo, &inodo) == EXIT_FAILURE)
    {
        fprintf(stderr, "Error in leer_inodo(): mi_read_f()\n");
        return EXIT_FAILURE;
    }

    //Actualizar la metainformación
    inodo.atime = time(NULL);

    //Escribimos inodo
    if (escribir_inodo(ninodo, inodo) == EXIT_FAILURE)
    {
        fprintf(stderr, "Error in escribir_inodo(): mi_read_f()\n");
        return EXIT_FAILURE;
    }

    //Comprobar que no haya errores de escritura y que se haya escrito todo bien.
    if (nbytes == bytesleidos)
    {
#if DEBUGGER
        printf("\tmi_read_f: BIEN\n");
#endif
        return bytesleidos;
    }
    else
    {
#if DEBUGGER
        printf("mi_read_f(): MAL\n\tnbytes:%i\n\tbytesleidos:%i\n", nbytes, bytesleidos);
#endif
        return -1;
    }
}

/**
 * Función: int mi_stat_f(unsigned int ninodo, struct STAT *p_stat);
 * ---------------------------------------------------------------------
 * Devuelve la metainformación de un fichero/directorio.
 * 
 * In:  ninodo: Posicion del inodo del array del inodo
 *      p_stat: Struct con los mismos campos que un inodo 
 *              excepto los punteros.
 * Out: EXIT_SUCCESS
 *      EXIT_FAILURE
 */
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    //Leemos el inodo
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo))
    {
        fprintf(stderr, "Error in mi_stat_f(): leer_inodo()\n");
        return EXIT_FAILURE;
    }

    // Guardar valores del inodo
    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;
    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->atime = inodo.atime;
    p_stat->ctime = inodo.ctime;
    p_stat->mtime = inodo.mtime;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;

    return EXIT_SUCCESS;
}

/**
 * Función: int mi_chmod_f(unsigned int ninodo, unsigned char permisos);
 * ---------------------------------------------------------------------
 * Cambia los permisos de un fichero/directorio.
 * 
 * IN:  ninodo: Posicion del inodo del array del inodo a cambiar permisos
 *      permisos: Valor de permisos
 * OUT: EXIT_SUCCESS
 *      EXIT_FAILURE
 */
int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
{
    //Leemos el inodo
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo))
    {
        fprintf(stderr, "Error in mi_chmod_f(): leer_inodo()\n");
        return EXIT_FAILURE;
    }

    //Cambiar los permisos del archivo
    inodo.permisos = permisos;
    inodo.ctime = time(NULL);

    if (escribir_inodo(ninodo, inodo))
    {
        fprintf(stderr, "Error in mi_chmod_f(): leer_inodo()\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}