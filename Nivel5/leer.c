#include <stdlib.h>
#include "ficheros.h"
/**
 * Son programas externos ficticios, sólo para probar, temporalmente, 
 * las funcionalidades de lectura/escritura y cambio de permisos, 
 * que involucran funciones de las 3 capas inferiores de nuestra 
 * biblioteca del sistema de ficheros, pero estos programas 
 * NO forman parte del sistema de ficheros.
 * 
 * Programa que comprueba si el número de argumentos es correcto y
 * en caso contrario mostrar la sintaxis.
 * 
 * ---------------------------------------------------------------------
 *                          leer.c:
 * ---------------------------------------------------------------------
 * 
 * Le pasaremos por línea de comandos un nº de inodo obtenido con el 
 * programa anterior (escribir.c). ninodo, (además del nombre del 
 * dispositivo). Su funcionamiento tiene que ser similar al comando 
 * cat de Linux, explorando TODO el fichero.
 * 
*/
#define tambuffer = 1500

int main(int argc, char const *argv[])
{

    //Variables
    int ninodo;
    struct superbloque SB;
    struct inodo inodo;
    //Para lectura
    int offset = 0;
    int nbytes = 1500;
    int bytesleidos = 0;
    char buffer[nbytes];

    //Sintaxis correcta
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis: leer <nombre_dispositivo><numero_inodo>\n");
        return EXIT_FAILURE;
    }

    //Inicializacion del buffer a 0.
    memset(buffer, 0, nbytes);
    ninodo = atoi(argv[2]);
    // Montar el dispositivo en el sistema.
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "leer.c: Error al montar el dispositivo.\n");
        return EXIT_FAILURE;
    }

    //Leer superbloque
    if (bread(0, &SB) == EXIT_FAILURE)
    {
        fprintf(stderr, "leer.c: Error de lectura del superbloque.\n");
        return EXIT_FAILURE;
    }

    // Lee del fichero hasta llenar el buffer o fin de fichero.
    int auxBytesbytesleidos = mi_read_f(ninodo, buffer, offset, nbytes);
    while (auxBytesbytesleidos > 0)
    {
        bytesleidos = bytesleidos + auxBytesbytesleidos;
        // Escribe el contenido del buffer en el destino indicado.
        write(1, buffer, auxBytesbytesleidos);

        //Limpiar buffer
        memset(buffer, 0, nbytes);
        //Actulizar offset
        offset = offset + nbytes;
        //Leemos otra vez
        auxBytesbytesleidos = mi_read_f(ninodo, buffer, offset, nbytes);
    }

    // Leer el inodo del archivo
    if (leer_inodo(ninodo, &inodo))
    {
        fprintf(stderr, "Error con la lectura del inodo.\n");
        return EXIT_FAILURE;
    }

    fprintf(stderr, "total_bytesleidos: %d\ntamEnBytesLog: %d\n", bytesleidos, inodo.tamEnBytesLog);

    // Desmonta el dispositivo virtual
    if (bumount() == EXIT_FAILURE)
    {
        fprintf(stderr, "leer.c: Error al desmonta el dispositivo virtual.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}