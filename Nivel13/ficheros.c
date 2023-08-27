//Autores: Jorge González Pascual, Luis Clar Fiol
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
 *      Error: FAILURE
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
    if (leer_inodo(ninodo, &inodo) == FAILURE)
    {
        fprintf(stderr, "mi_write_f(): Error leer_inodo() \n");
        return FAILURE;
    }

    //Comprobamos que el inodo tenga los permisos para escribir
    if ((inodo.permisos & 2) != 2)
    {
        fprintf(stderr, "No hay permisos de escritura\n");
        return FAILURE;
    }

    //Asignaciones de las variables.
    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;

    desp1 = offset % BLOCKSIZE;
    desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    //Obtencion del numero de bloque fisico
    mi_waitSem();
    nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
    if (nbfisico == FAILURE)
    {
        fprintf(stderr, "mi_write_f(): Error al traducir_bloque_inodo()\n");
        mi_signalSem();
        return FAILURE;
    }
    mi_signalSem();

    //Leemos el bloque fisico
    if (bread(nbfisico, buf_bloque) == FAILURE)
    {
        fprintf(stderr, "mi_write_f(): Error al leer el bloque fisico\n");
        return FAILURE;
    }

    //Caso en el que lo que queremos escribir cabe en un bloque fisico
    if (primerBL == ultimoBL)
    {
        memcpy(buf_bloque + desp1, buf_original, nbytes);

        //Escribimos el bloque fisico en el disco
        auxByteEscritos = bwrite(nbfisico, buf_bloque);
        if (auxByteEscritos == FAILURE)
        {
            fprintf(stderr, "Error mi_write_f(): bwrite()\n");
            return FAILURE;
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
        if (auxByteEscritos == FAILURE)
        {
            fprintf(stderr, "Error mi_write_f(): bwrite()\n");
            return FAILURE;
        }
        bytesescritos += auxByteEscritos - desp1;

        //Parte 2: Bloques intermedios
        for (int i = primerBL + 1; i < ultimoBL; i++)
        {
            //Obtenemos los bloques intermedios
            mi_waitSem();
            nbfisico = traducir_bloque_inodo(ninodo, i, 1);
            if (nbfisico == FAILURE)
            {
                fprintf(stderr, "Error mi_write_f(): traducir_bloque_inodo()\n");
                mi_signalSem();
                return FAILURE;
            }
            mi_signalSem();

            //Escribimos los bloques intermedios
            auxByteEscritos = bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE);
            if (auxByteEscritos == FAILURE)
            {
                fprintf(stderr, "Error mi_write_f(): bwrite()\n");
                return FAILURE;
            }
            bytesescritos += auxByteEscritos;
        }

        //Parte 3: Último bloque escrito parcialmente
        //Obtenemos el bloque fisico
        mi_waitSem();
        nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 1);
        if (nbfisico == FAILURE)
        {
            fprintf(stderr, "Error mi_write_f(): traducir_bloque_inodo()\n");
            mi_signalSem();
            return FAILURE;
        }
        mi_signalSem();
        //Leemos el bloque fisico
        if (bread(nbfisico, buf_bloque) == FAILURE)
        {
            fprintf(stderr, "Error mi_write_f(): bread()\n");
            return FAILURE;
        }

        memcpy(buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);

        auxByteEscritos = bwrite(nbfisico, buf_bloque);
        if (auxByteEscritos == FAILURE)
        {
            fprintf(stderr, "Error mi_write_f(): bwrite()\n");
            return FAILURE;
        }

        bytesescritos += desp2 + 1;
    }

    //Leer el inodo actualizado.
    mi_waitSem(); //Semaforos
    if (leer_inodo(ninodo, &inodo) == FAILURE)
    {
        fprintf(stderr, "Error leer_inodo(): mi_write_f() \n");
        mi_signalSem();
        return FAILURE;
    }

    //Actualizar la metainformación
    //Comprobación si lo que hemos escrito es mas grande que el fichero
    if (inodo.tamEnBytesLog < (nbytes + offset))
    {
        inodo.tamEnBytesLog = nbytes + offset;
        inodo.ctime = time(NULL);
    }

    inodo.mtime = time(NULL);

    if (escribir_inodo(ninodo, inodo) == FAILURE)
    {
        fprintf(stderr, "Error escribir_inodo(): mi_write_f() \n");
        mi_signalSem();
        return FAILURE;
    }
    mi_signalSem(); //Semaforos

    //Comprobar que no haya errores de escritura y que se haya escrito todo bien.
    if (nbytes == bytesescritos)
    {
#if DEBUGGER
        fprintf(stderr, "\tmi_write_f: BIEN\n");
        fprintf(stderr, "\tmi_read_f(): nbfisico = %i\n", nbfisico);
#endif
        return bytesescritos;
    }
    else
    {
#if DEBUGGER
        fprintf(stderr, "mi_write_f: MAL\n\tnbytes:%i\n\tbytesescritos:%i\n", nbytes, bytesescritos);
#endif
        return FAILURE;
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
 * Out: bytesleidos: Cantidad de bytes bytesleidos
 *      error: FAILURE
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
    if (leer_inodo(ninodo, &inodo) == FAILURE)
    {
        fprintf(stderr, "Error in mi_read_f(): leer_inodo()\n");
        return bytesleidos;
    }

    //Comprobamos que el inodo tenga los permisos para leer
    if ((inodo.permisos & 4) != 4)
    {
        fprintf(stderr, "Error in mi_read_f(): No hay permisos de lectura!\n");
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
        if (nbfisico != FAILURE)
        {
            //Leemos el bloque fisico del disco
            auxByteLeidos = bread(nbfisico, buf_bloque);
            if (auxByteLeidos == FAILURE)
            {
                fprintf(stderr, "Error mi_read_f(): bread()\n");
                return FAILURE;
            }
            memcpy(buf_original, buf_bloque + desp1, nbytes);
        }
        bytesleidos = nbytes;
    }
    //Caso en el que la lectura ocupa mas de un bloque fisico
    else if (primerBL < ultimoBL)
    {
        //Parte 1: Primero bloque leido parcialmente
        if (nbfisico != FAILURE)
        {
            //Leemos el bloque fisico del disco
            auxByteLeidos = bread(nbfisico, buf_bloque);
            if (auxByteLeidos == FAILURE)
            {
                fprintf(stderr, "Error mi_read_f(): bread()\n");
                return FAILURE;
            }
            memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
        }

        bytesleidos = BLOCKSIZE - desp1;

        //Parte 2: Bloques intermedios
        for (int i = primerBL + 1; i < ultimoBL; i++)
        {
            //Obtenemos los bloques intermedios
            nbfisico = traducir_bloque_inodo(ninodo, i, 0);
            if (nbfisico != FAILURE)
            {
                //Leemos el bloque fisico del disco
                auxByteLeidos = bread(nbfisico, buf_bloque);
                if (auxByteLeidos == FAILURE)
                {
                    fprintf(stderr, "Error mi_read_f(): bread()\n");
                    return FAILURE;
                }
                memcpy(buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE, buf_bloque, BLOCKSIZE);
            }
            bytesleidos += BLOCKSIZE;
        }

        //Parte 3: Último bloque leido parcialmente
        //Obtenemos el bloque fisico
        nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 0);
        //Parte 1: Primero bloque leido parcialmente
        if (nbfisico != FAILURE)
        {
            //Leemos el bloque fisico del disco
            auxByteLeidos = bread(nbfisico, buf_bloque);
            if (auxByteLeidos == FAILURE)
            {
                fprintf(stderr, "Error mi_read_f(): bread()\n");
                return FAILURE;
            }
            //Calculamos el byte lógico del último bloque hasta donde hemos de leer
            memcpy(buf_original + (nbytes - desp2 - 1), buf_bloque, desp2 + 1);
        }
        bytesleidos += desp2 + 1;
    }

    //Leer el inodo actualizado.
    mi_waitSem(); //Semaforos
    if (leer_inodo(ninodo, &inodo) == FAILURE)
    {
        fprintf(stderr, "Error leer_inodo(): mi_read_f()\n");
        return FAILURE;
    }

    //Actualizar la metainformación
    inodo.atime = time(NULL);

    //Escribimos inodo
    if (escribir_inodo(ninodo, inodo) == FAILURE)
    {
        fprintf(stderr, "Error escribir_inodo(): mi_read_f()\n");
        return FAILURE;
    }
    mi_signalSem(); //Semaforos

    //Comprobar que no haya errores de escritura y que se haya escrito todo bien.
    if (nbytes == bytesleidos)
    {
#if DEBUGGER
        fprintf(stderr, "\tmi_read_f: BIEN\n");
#endif
        return bytesleidos;
    }
    else
    {
#if DEBUGGER
        fprintf(stderr, "mi_read_f(): MAL\n\tnbytes:%i\n\tbytesleidos:%i\n", nbytes, bytesleidos);
#endif
        return FAILURE;
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
 *      FAILURE
 */
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    //Leemos el inodo
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo))
    {
        fprintf(stderr, "Error mi_stat_f(): leer_inodo()\n");
        return FAILURE;
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
 * In:  ninodo: Numero del inodo del array del inodo a cambiar permisos
 *      permisos: Valor de permisos
 * Out: EXIT_SUCCESS
 *      FAILURE
 */
int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
{
    mi_waitSem();
    //Leemos el inodo
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == FAILURE)
    {
        fprintf(stderr, "Error mi_chmod_f(): leer_inodo()\n");
        mi_signalSem();
        return FAILURE;
    }

    //Cambiar los permisos del archivo
    inodo.permisos = permisos;
    inodo.ctime = time(NULL);

    if (escribir_inodo(ninodo, inodo) == FAILURE)
    {
        fprintf(stderr, "Error mi_chmod_f(): leer_inodo()\n");
        mi_signalSem();
        return FAILURE;
    }
    mi_signalSem();
    return EXIT_SUCCESS;
}

/**
 * Función: mi_truncar_f(unsigned int ninodo, unsigned int nbytes);
 * ---------------------------------------------------------------------
 * Truncar inodo pasado.
 * 
 * Nota: Escribe un escribe el inodo en el dispositivo. LEER otra vez SB
 * 
 * In:  ninodo: Número de inodo en el array de inodos.
 *      nbytes: Tamaño del fichero al truncar.
 * 
 * Out: EXIT_SUCCESS
 *      FAILURE
 */
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes)
{
    //Declaraciones
    int lib;
    struct inodo inodo;

    //Leer el inodo.
    if (leer_inodo(ninodo, &inodo) == FAILURE)
    {
        fprintf(stderr, " Error mi_truncar_f(): leer_inodo()\n");
        return FAILURE;
    }

    //Comprobamos que el inodo tenga los permisos para escribir
    if ((inodo.permisos & 2) != 2)
    {
        fprintf(stderr, " Error mi_truncar_f(): El inodo no tiene permisos.\n");
        return FAILURE;
    }

    //Comprobamos que no intenten truncar mas allá del tamaño de bytes lógicos
    if (nbytes > inodo.tamEnBytesLog)
    {
        fprintf(stderr, " Error mi_truncar_f(): No se puede truncar porque, nbytes > tamaño de bytes lógicos");
        return FAILURE;
    }

    //Obtener bloque logico
    int primerBL;
    if (nbytes % BLOCKSIZE == 0)
    {
        primerBL = nbytes / BLOCKSIZE;
    }
    else
    {
        primerBL = (nbytes / BLOCKSIZE) + 1;
    }

    //Liberar bloques a partir del primerBL del inodo.
    lib = liberar_bloques_inodo(primerBL, &inodo);
    //Actulizar informacion
    inodo.mtime = time(NULL);
    inodo.ctime = time(NULL);
    inodo.tamEnBytesLog = nbytes;
    inodo.numBloquesOcupados -= lib;

    // Escribe el inodo en el dispositivo.
    if (escribir_inodo(ninodo, inodo) == FAILURE)
    {
        fprintf(stderr, "Error mi_truncar_f(): escribir_inodo()\n");
        return FAILURE;
    }

    return lib;
}