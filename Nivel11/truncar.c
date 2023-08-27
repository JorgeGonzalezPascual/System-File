//Autores: Jorge González Pascual, Luis Clar Fiol
#include "ficheros.h"

/**
* ---------------------------------------------------------------------
*                          truncar.c:
* ---------------------------------------------------------------------
* 
* Programa externo ficticio sólo para probar, temporalmente, 
* la funcionalidad d borrado parcial o total del contenido de un archivo
* del dispositivo virtual.
*  
*/

int main(int argc, char *argv[])
{

    //Comprobamos que los parametros sean correctos
    if (argc != 4)
    {
        fprintf(stderr, "Sintaxis errónea: ./truncar <nombre_dispositivo> <ninodo> <nbytes>\n");
        return EXIT_FAILURE;
    }

    //Obtenemos los parámetros de argv
    int ninodo = atoi(argv[2]);
    int nbytes = atoi(argv[3]);

    //Montaje del dispositivo virtual
    if (bmount(argv[1]) < 0)
    {
        fprintf(stderr, "truncar.c: Error al montar el dispositivo virtual.\n");
        return EXIT_FAILURE;
    }

    // Si n_bytes = 0: Liberamos el inodo si no truncamos el archivo.
    if (nbytes == 0)
    {
        if (liberar_inodo(ninodo) < 0)
        {
            fprintf(stderr, "truncar.c: Error al librar el inodo %i.\n", ninodo);
            return EXIT_FAILURE;
        }
    }
    else
    {
        mi_truncar_f(ninodo, nbytes);
    }

    //Inodo liberado
    struct STAT p_stat;
    if (mi_stat_f(ninodo, &p_stat))
    {
        fprintf(stderr, "truncar.c: Error mi_stat_f()\n");
        return FAILURE;
    }

    // Variables utilizadas para cambiar el formato de la fecha y hora.
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];
    ts = localtime(&p_stat.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

    //Información del inodo escrito
    printf("\nDATOS INODO %d:\n", ninodo);
    printf("tipo=%c\n", p_stat.tipo);
    printf("permisos=%d\n", p_stat.permisos);
    printf("atime: %s\n", atime);
    printf("ctime: %s\n", ctime);
    printf("mtime: %s\n", mtime);
    printf("nLinks= %d\n", p_stat.nlinks);
    printf("tamEnBytesLog= %d\n", p_stat.tamEnBytesLog);
    printf("numBloquesOcupados= %d\n", p_stat.numBloquesOcupados);

    //Liberación
    if (bumount() < 0)
    {
        fprintf(stderr, "truncar.c: Error al desmontar el dispositivo virtual.\n");
        return FAILURE;
    }

    return EXIT_SUCCESS;
}