//Autores: Jorge González Pascual, Luis Clar Fiol
#include "directorios.h"
#define DEBUG8 0
#define DEBUG9 0

static struct UltimaEntrada UltimaEntrada[CACHE];
int MAXCACHE = CACHE;

/**
 * Función: int extraer_camino(const char *camino, char *inicial, char *final, char *tipo);
 * ---------------------------------------------------------------------
 * Dada una cadena de caracteres camino (que comience por '/'), separa su contenido en dos
 * 
 * In:  camino: dirección del fichero del dispositivo virtual.
 *      inicial: directorio / fichero más proximo del directorio en el que estamos
 *      final: Camino restante (camino - inicial)
 *      tipo: f -> fichero
 *            d -> directorio
 * Out: EXIT_SUCCES
 *      Error: FAILURE
 * 
 */
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{
    // Si el camino no comienza con '/' entonces error.
    if (camino[0] != '/')
    {
        return FAILURE;
    }

    // Localizar la primera barra despues de la inicial.
    //+1 para evitar la primera '/'
    char *rest = strchr((camino + 1), '/');
    strcpy(tipo, "f");

    //Si se ha encotrado el caracter '/'
    if (rest)
    {
        //Inicial = camino - resto (Copiamos todo en inicial menos el resto)
        strncpy(inicial, (camino + 1), (strlen(camino) - strlen(rest) - 1));
        //Final = resto
        strcpy(final, rest);

        //Mirar si se trata de un directorio
        if (final[0] == '/')
        {
            strcpy(tipo, "d");
        }
    }
    else //Si no se ha encotrado
    {
        //Inicial = camino
        strcpy(inicial, (camino + 1));
        //Final: vacio
        strcpy(final, "");
    }

    return EXIT_SUCCESS;
}

/**
 * Función: int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo,
 *  unsigned int *p_entrada, char reservar, unsigned char permisos);
 * ---------------------------------------------------------------------
 * Función que buscara una entrada en el sistema de archivos, y reservara o no segun el parametro
 * @reservar un inodo para dicho elemento. Si ha ido bien devulve el id del inodo del elemento.
 * 
 * In:  camino_parcial: camino a recorrer
 *      p_inodo_dir: id del inodo del directorio padre
 *      p_inodo: id del inodo del destino final a obtener
 *      p_entrada: numero de entrada dentro del directorio padre.
 *      reservar: indicado si se ha de reservar un nuevo elemento: (1 si o 0 no).
 *      permisos: permisos si se tiene que reservar
 * 
 * Out: EXIT_SUCCESS
 *      Error: El codigo de error
 * 
 * Nota: Para procesar y visualizar el codigo de error se requiere el proceso: mostrar_error_buscar_entrada()
 * 
 */
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo,
                   unsigned int *p_entrada, char reservar, unsigned char permisos)
{

    struct entrada entrada;
    struct inodo dir_inodo;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    char tipo;
    int cant_entradas_inodo, num_entrada_inodo;

    memset(inicial, 0, sizeof(entrada.nombre));
    memset(final, 0, strlen(camino_parcial));
    memset(entrada.nombre, 0, sizeof(entrada.nombre));

    //camino_parcial es “/”
    if (!strcmp(camino_parcial, "/"))
    {
        struct superbloque SB;
        bread(posSB, &SB);

        *p_inodo = SB.posInodoRaiz; //nuestra raiz siempre estará asociada al inodo 0
        *p_entrada = 0;

        return EXIT_SUCCESS;
    }

    if (extraer_camino(camino_parcial, inicial, final, &tipo) == FAILURE)
    {
        return ERROR_CAMINO_INCORRECTO;
    }

#if DEBUG8
    fprintf(stderr, "[buscar_entrada()->inicial: %s, final: %s, reservar: %d]\n", inicial, final, reservar);
#endif

    //buscamos la entrada cuyo nombre se encuentra en inicial
    leer_inodo(*p_inodo_dir, &dir_inodo);
    //Comprobamos que el inodo tenga permiso de lectura.
    if ((dir_inodo.permisos & 4) != 4)
    {
        return ERROR_PERMISO_LECTURA;
    }

    // Inicializa un array de entradas que caben en un bloque.
    memset(entrada.nombre, 0, sizeof(entrada.nombre));

    cant_entradas_inodo = dir_inodo.tamEnBytesLog / sizeof(struct entrada); //cantidad de entradas que contiene el inodo
    num_entrada_inodo = 0;                                                  //nº de entrada inicial

    if (cant_entradas_inodo > 0)
    {
        if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) < 0)
        {
            return ERROR_PERMISO_LECTURA;
        }

        //previamente volver a inicializar el buffer de lectura con 0s
        while (num_entrada_inodo < cant_entradas_inodo && strcmp(inicial, entrada.nombre) != 0)
        {
            num_entrada_inodo++;
            memset(entrada.nombre, 0, sizeof(entrada.nombre));
            if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) < 0)
            {
                return ERROR_PERMISO_LECTURA;
            }
        }
    }

    // Si inicial ≠ entrada.nombre:
    // Si inicial no se ha encontrado y se han procesado todas las entradas.
    if ((num_entrada_inodo == cant_entradas_inodo) && (strcmp(entrada.nombre, inicial) != 0))
    { //la entrada no existe
        switch (reservar)
        {
        case 0: //modo consulta. Como no existe retornamos error
            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            break;
        case 1: //modo escritura
            //Creamos la entrada en el directorio referenciado por *p_inodo_dir
            //si es fichero no permitir escritura
            if (dir_inodo.tipo == 'f')
            {
                return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
            }
            //si es directorio comprobar que tiene permiso de escritura
            if ((dir_inodo.permisos & 2) != 2)
            {
                return ERROR_PERMISO_ESCRITURA;
            }
            else
            {
                strcpy(entrada.nombre, inicial);
                if (tipo == 'd')
                {
                    if (strcmp(final, "/") == 0)
                    {
                        //reservar un nuevo inodo como directorio y asignarlo a la entrada
                        entrada.ninodo = reservar_inodo('d', permisos);
#if DEBUG8
                        fprintf(stderr, "[buscar_entrada()->reservado inodo: %d tipo %c con permisos %d para '%s']\n", entrada.ninodo, tipo, permisos, entrada.nombre);
#endif
                    }
                    else
                    { //cuelgan más diretorios o ficheros
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else
                { //es un fichero
                    //reservar un inodo como fichero y asignarlo a la entrada
                    entrada.ninodo = reservar_inodo('f', permisos);
#if DEBUG8
                    fprintf(stderr, "[buscar()->reservado inodo: %d tipo %c con permisos %d para '%s']\n", entrada.ninodo, tipo, permisos, entrada.nombre);
#endif
                }

#if DEBUG8
                fprintf(stderr, "[buscar_entrada()->creada entrada: %s, %d] \n", inicial, entrada.ninodo);
#endif

                //escribir la entrada en el directorio padre
                if (mi_write_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) == FAILURE)
                {
                    if (entrada.ninodo != FAILURE)
                    {
                        liberar_inodo(entrada.ninodo);
#if DEBUG8
                        fprintf(stderr, "[buscar_entrada()-> liberado inodo %i, reservado a %s\n", num_entrada_inodo, inicial);
#endif
                    }
                    return FAILURE;
                }
            }
        }
    }

    //Si hemos llegado al final del camino
    if (!strcmp(final, "/") || !strcmp(final, ""))
    {
        if ((num_entrada_inodo < cant_entradas_inodo) && (reservar == 1))
        {
            //modo escritura y la entrada ya existe
            return ERROR_ENTRADA_YA_EXISTENTE;
        }

        //cortamos la recursividad
        *p_inodo = entrada.ninodo;      //asignar a *p_inodo el numero de inodo del directorio o fichero creado o leido
        *p_entrada = num_entrada_inodo; //asignar a *p_entrada el número de su entrada dentro del último directorio que lo contiene

        return EXIT_SUCCESS;
    }
    else
    {

        *p_inodo_dir = entrada.ninodo; //asignamos a *p_inodo_dir el puntero al inodo que se indica en la entrada encontrada;
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }
    return EXIT_SUCCESS;
}

/**
 * Procedimiento: void mostrar_error_buscar_entrada(int error);
 * ---------------------------------------------------------------------
 * Procedimiento que dado un error lo muestra por pantalla.
 * 
 * In: error: error de buscar_entrada()
 * Out: -
 * 
 */
void mostrar_error_buscar_entrada(int error)
{
    switch (error)
    {
    case FAILURE:
        fprintf(stderr, "Error: Camino incorrecto.\n");
        break;
    case -2:
        fprintf(stderr, "Error: Permiso denegado de lectura.\n");
        break;
    case -3:
        fprintf(stderr, "Error: No existe el archivo o el directorio.\n");
        break;
    case -4:
        fprintf(stderr, "Error: No existe algún directorio intermedio.\n");
        break;
    case -5:
        fprintf(stderr, "Error: Permiso denegado de escritura.\n");
        break;
    case -6:
        fprintf(stderr, "Error: El archivo ya existe.\n");
        break;
    case -7:
        fprintf(stderr, "Error: No es un directorio.\n");
        break;
    }
}

/**
 * Función:  int mi_creat(const char *camino, unsigned char permisos);
 * ---------------------------------------------------------------------
 * Función de la capa de directorios que crea un fichero/directorio y 
 * su entrada de directorio.
 * 
 * In:  camino: patch completo a crearse el fichero/directorio
 *      permisos: permisos del fichero/directorio
 * Out: EXIT_SUCCESS
 *      Error: El codigo de error
 * 
 * Nota: Para procesar y visualizar el codigo de error se requiere el proceso: mostrar_error_buscar_entrada()
 * 
 */
int mi_creat(const char *camino, unsigned char permisos)
{
    mi_waitSem();

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos)) < 0)
    {
        mi_signalSem();
        return error;
    }
    mi_signalSem();
    return EXIT_SUCCESS;
}

/* Funcion: mi_dir:
* ---------------------------------------------------------------------
* Esta funcion devuelve el contenido del directorio en el buffer.
*
* In:   camino: directorio
*       buffer: para guardar el contenido del directorio para imprimir posteriormente
*       tipo: para diferenciar el tipo
*
* Out:  El numero de entradas leidas.
*       FAILURE
*/
int mi_dir(const char *camino, char *buffer, char tipo)
{
    struct tm *tm;

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    int nEntradas = 0;

    error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4); //Permisos para leer
    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
        return FAILURE;
    }

    struct inodo inodo;
    if (leer_inodo(p_inodo, &inodo) == FAILURE)
    {
        return FAILURE;
    }
    if ((inodo.permisos & 4) != 4)
    {
        return FAILURE;
    }

    //struct entrada entrada;

    char tmp[100];       //Para el tiempo
    char tamEnBytes[10]; //10 = valor maximo de un unsigned int
    struct entrada entrada;
    if (tipo == 'd')
    {
        if (leer_inodo(p_inodo, &inodo) == FAILURE)
        {
            return FAILURE;
        }

        //Buffer de salida
        struct entrada entradas[BLOCKSIZE / sizeof(struct entrada)];
        memset(&entradas, 0, sizeof(struct entrada));

        nEntradas = inodo.tamEnBytesLog / sizeof(struct entrada);

        int offset = 0;
        offset += mi_read_f(p_inodo, entradas, offset, BLOCKSIZE);

        //Leemos todos las entradas
        for (int i = 0; i < nEntradas; i++)
        {
            //Leer el inodo correspndiente
            if (leer_inodo(entradas[i % (BLOCKSIZE / sizeof(struct entrada))].ninodo, &inodo) == FAILURE)
            {
                return FAILURE;
            }

            //Tipo
            if (inodo.tipo == 'd')
            {
                strcat(buffer, MAGENTA);
                strcat(buffer, "d");
            }
            else
            {
                strcat(buffer, CYAN);
                strcat(buffer, "f");
            }
            strcat(buffer, "\t");

            //Permisos
            strcat(buffer, BLUE);
            strcat(buffer, ((inodo.permisos & 4) == 4) ? "r" : "-");
            strcat(buffer, ((inodo.permisos & 2) == 2) ? "w" : "-");
            strcat(buffer, ((inodo.permisos & 1) == 1) ? "x" : "-");
            strcat(buffer, "\t");

            //mTime
            strcat(buffer, YELLOW);
            tm = localtime(&inodo.mtime);
            sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
            strcat(buffer, tmp);
            strcat(buffer, "\t");

            //Tamaño
            strcat(buffer, LBLUE);
            sprintf(tamEnBytes, "%d", inodo.tamEnBytesLog);
            strcat(buffer, tamEnBytes);
            strcat(buffer, "\t");

            //Nombre
            strcat(buffer, LRED);
            strcat(buffer, entradas[i % (BLOCKSIZE / sizeof(struct entrada))].nombre);
            while ((strlen(buffer) % TAMFILA) != 0)
            {
                strcat(buffer, " ");
            }

            //Siguiente
            strcat(buffer, RESET);
            strcat(buffer, "\n");

            if (offset % (BLOCKSIZE / sizeof(struct entrada)) == 0)
            {
                offset += mi_read_f(p_inodo, entradas, offset, BLOCKSIZE);
            }
        }
    }
    else
    { // No es un directorio, es un archivo
        mi_read_f(p_inodo_dir, &entrada, sizeof(struct entrada) * p_entrada, sizeof(struct entrada));
        leer_inodo(entrada.ninodo, &inodo);

        //Tipo
        strcat(buffer, CYAN);
        strcat(buffer, "f");

        strcat(buffer, "\t");

        //Permisos
        strcat(buffer, BLUE);
        strcat(buffer, ((inodo.permisos & 4) == 4) ? "r" : "-");
        strcat(buffer, ((inodo.permisos & 2) == 2) ? "w" : "-");
        strcat(buffer, ((inodo.permisos & 1) == 1) ? "x" : "-");
        strcat(buffer, "\t");

        //mTime
        strcat(buffer, YELLOW);
        tm = localtime(&inodo.mtime);
        sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        strcat(buffer, tmp);
        strcat(buffer, "\t");

        //Tamaño
        strcat(buffer, LBLUE);
        sprintf(tamEnBytes, "%d", inodo.tamEnBytesLog);
        strcat(buffer, tamEnBytes);
        strcat(buffer, "\t");

        //Nombre
        strcat(buffer, LRED);
        strcat(buffer, entrada.nombre);
        while ((strlen(buffer) % TAMFILA) != 0)
        {
            strcat(buffer, " ");
        }

        //Siguiente
        strcat(buffer, RESET);
        strcat(buffer, "\n");
    }
    return nEntradas;
}

/* Funcion: mi_chmod(const char *camino, unsigned char permisos)
* ---------------------------------------------------------------------
* Funcion que cambia los permisos de un fichero o directorio,
*
* In: camino: direccion del elemento
*     permisos: nuevos permisos a tener
*
* Out: EXIT_SUCCESS 
*      Error: El codigo de error
* 
* Nota: Para procesar y visualizar el codigo de error se requiere el proceso: mostrar_error_buscar_entrada()
*
*/
int mi_chmod(const char *camino, unsigned char permisos)
{

    // Inicializacion de variables.
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, permisos);

    if (error < 0)
    {
        return error;
    }

    mi_chmod_f(p_inodo, permisos);

    return EXIT_SUCCESS;
}

/* Funcion: mi_stat(const char *camino, struct STAT *p_stat)
* ---------------------------------------------------------------------
* Funcion que obtiene la metainformacion del elemento del camino.
*
* In:   camino: direccion del elemento a obtener la informacion.
*       p_stat: metainformacion del elemento.
*
* Out: p_inodo 
*      Error: El codigo de error o FAILURE
* 
* Nota: Para procesar y visualizar el codigo de error se requiere el proceso: mostrar_error_buscar_entrada()
*
*/
int mi_stat(const char *camino, struct STAT *p_stat)
{

    // Inicializacion de variables.
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int r = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, p_stat->permisos);
    if (r < 0)
    {
        return r;
    }

    if (mi_stat_f(p_inodo, p_stat) == FAILURE)
    {
        return FAILURE;
    }

    return p_inodo;
}

/* Funcion: mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes)
* ---------------------------------------------------------------------
* Funcion para escribir contenido en un fichero.
*
* In:   camino: direccion donde se escribir
*       buffer: buffer del contenido a escribir
*       offset: bytes de desplazamiento
*       nbytes: tamaño bytes del buffer
*
* Out: número de bytes escritos
*      Error: El codigo de error
* 
* Nota: Para procesar y visualizar el codigo de error se requiere el proceso: mostrar_error_buscar_entrada()
*
*/
int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes)
{
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    int bytes_escritos = 0;
    int error = 0;
    int are = 0;

    //Recorrido del cache para comprobar si la escritura es sobre un inodo anterior
    for (int i = 0; i < (MAXCACHE - 1); i++)
    {

        if (strcmp(camino, UltimaEntrada[i].camino) == 0)
        {
            p_inodo = UltimaEntrada[i].p_inodo;
            are = 1;
            break;
        }
    }
    //Si no esta
    if (!are)
    {
        //Obtenemos el inodo del elemento
        error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4); // 4 -> Permisos de lectura
        if (error < 0)
        {
            return error;
        }

        //si aun no se ha llenado la caché
        if (MAXCACHE > 0)
        {
            strcpy(UltimaEntrada[CACHE - MAXCACHE].camino, camino);
            UltimaEntrada[CACHE - MAXCACHE].p_inodo = p_inodo;
            --MAXCACHE;
#if DEBUG9
            fprintf(stderr, "[mi_write() → Actualizamos la caché de escritura]\n");
#endif
        }
        else //Remplazo FIFO
        {
            for (int i = 0; i < CACHE - 1; i++)
            {
                strcpy(UltimaEntrada[i].camino, UltimaEntrada[i + 1].camino);
                UltimaEntrada[i].p_inodo = UltimaEntrada[i + 1].p_inodo;
            }
            strcpy(UltimaEntrada[CACHE - 1].camino, camino);
            UltimaEntrada[CACHE - 1].p_inodo = p_inodo;

#if DEBUG9
            fprintf(stderr, "[mi_write() → Actualizamos la caché de escritura]\n");
#endif
        }
    }
    //Escribimos en el archivo
    bytes_escritos = mi_write_f(p_inodo, buf, offset, nbytes);
    if (bytes_escritos == FAILURE)
    {
        bytes_escritos = 0;
    }
    return bytes_escritos;
}

/* Funcion: int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes)
* ---------------------------------------------------------------------
* Funcion  para leer contenido en un fichero.
*
* In:   camino: direccion donde se escribira
*       buffer: buffer del contenido a escribir
*       offset: bytes de desplazamiento
*       nbytes: tamaño bytes del buffer
*
* Out: número de bytes leídos
*      Error: El codigo de error
* 
* Nota: Para procesar y visualizar el codigo de error se requiere el proceso: mostrar_error_buscar_entrada()
*
*/

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes)
{

    // Inicializacion de variables.
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error = 0;
    int are = 0;
    int bytes_leidos = 0;

    //Recorrido del cache para comprobar si la lectura es sobre un inodo anterior
    for (int i = 0; i < (MAXCACHE - 1); i++)
    {

        if (strcmp(camino, UltimaEntrada[i].camino) == 0)
        {
            p_inodo = UltimaEntrada[i].p_inodo;
            are = 1;
#if DEBUG9
            fprintf(stderr, "[mi_read() → Utilizamos la caché de lectura en vez de llamar a buscar_entrada()]\n");
#endif
            break;
        }
    }

    if (!are)
    {
        //Obtenemos el inodo del elemento
        error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4);
        if (error < 0)
        {
            return error;
        }

        //si aun no se ha llenado la caché
        if (MAXCACHE > 0)
        {
            strcpy(UltimaEntrada[CACHE - MAXCACHE].camino, camino);
            UltimaEntrada[CACHE - MAXCACHE].p_inodo = p_inodo;
            --MAXCACHE;
#if DEBUG9
            fprintf(stderr, "[mi_read() → Actualizamos la caché de lectura]\n");
#endif
        }
        else //Remplazo FIFO
        {
            for (int i = 0; i < CACHE - 1; i++)
            {
                strcpy(UltimaEntrada[i].camino, UltimaEntrada[i + 1].camino);
                UltimaEntrada[i].p_inodo = UltimaEntrada[i + 1].p_inodo;
            }
            strcpy(UltimaEntrada[CACHE - 1].camino, camino);
            UltimaEntrada[CACHE - 1].p_inodo = p_inodo;

#if DEBUG9
            fprintf(stderr, "[mi_read() → Actualizamos la caché de lectura]\n");
#endif
        }
    }

    // Realiza la lectura del archivo.
    bytes_leidos = mi_read_f(p_inodo, buf, offset, nbytes);
    if (bytes_leidos == FAILURE)
    {
        return ERROR_PERMISO_LECTURA;
    }
    return bytes_leidos;
}

/**
 * Función: int mi_link(const char *camino1, const char *camino2);
 * ---------------------------------------------------------------------
 * Crea el enlace de una entrada de directorio camino2 al inodo 
 * especificado por otra entrada de directorio camino1.
 * 
 * In:  camino1: camino del archivo a linkear.
 *      camino2: camino donde se crea el link al archivo.
 * Out: EXIT_SUCCESS
 *      Error: FAILURE
 */
int mi_link(const char *camino1, const char *camino2)
{
    mi_waitSem();
    unsigned int p_inodo_dir1 = 0;
    unsigned int p_inodo1 = 0;
    unsigned int p_entrada1 = 0;
    int error;

    unsigned int p_inodo_dir2 = 0;
    unsigned int p_inodo2 = 0;
    unsigned int p_entrada2 = 0;
    struct inodo inodo;

    //Revisamos el camino 1
    error = buscar_entrada(camino1, &p_inodo_dir1, &p_inodo1, &p_entrada1, 0, 4);
    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return FAILURE;
    }

    if (leer_inodo(p_inodo1, &inodo) < 0)
    {
        mi_signalSem();
        return FAILURE;
    }
    if (inodo.tipo != 'f')
    {
        fprintf(stderr, "mi_link: %s ha de ser un fichero\n");
        mi_signalSem();
        return FAILURE;
    }
    if ((inodo.permisos & 4) != 4)
    {
        fprintf(stderr, "mi_link: %s no tiene permisos de lectura\n", camino1);
        mi_signalSem();
        return ERROR_PERMISO_LECTURA;
    }

    //Revisamos el camino 2
    error = buscar_entrada(camino2, &p_inodo_dir2, &p_inodo2, &p_entrada2, 1, 6);
    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return FAILURE;
    }

    struct entrada entrada2;
    if (mi_read_f(p_inodo_dir2, &entrada2, sizeof(struct entrada) * (p_entrada2), sizeof(struct entrada)) < 0)
    {
        mi_signalSem();
        return FAILURE;
    }

    //Creamos el enlace
    entrada2.ninodo = p_inodo1;

    //Escribimos la entrada modificada
    if (mi_write_f(p_inodo_dir2, &entrada2, sizeof(struct entrada) * (p_entrada2), sizeof(struct entrada)) < 0)
    {
        mi_signalSem();
        return FAILURE;
    }
    //Liberamos el inodo
    if (liberar_inodo(p_inodo2) < 0)
    {
        mi_signalSem();
        return FAILURE;
    }

    //Incrementamos la cantidad de enlaces
    inodo.nlinks++;
    inodo.ctime = time(NULL);
    if (escribir_inodo(p_inodo1, inodo) == FAILURE)
    {
        mi_signalSem();
        return FAILURE;
    }

    mi_signalSem();
    return EXIT_SUCCESS;
}

/**
 * Función: int mi_unlink(const char *camino);
 * ---------------------------------------------------------------------
 * Función de la capa de directorios que borra la entrada de directorio 
 * especificada (no hay que olvidar actualizar la cantidad de enlaces en
 * el inodo) y, en caso de que fuera el último enlace existente, borrar
 * el propio fichero/directorio.
 * Es decir que esta función nos servirá tanto para borrar un enlace a 
 * un fichero como para eliminar un fichero o directorio que no contenga 
 * enlaces.
 *
 * In:  camino1: direccion del archivo a borrar
 * Out: EXIT_SUCCESS
 *      Error: FAILURE
 */
int mi_unlink(const char *camino)
{
    mi_waitSem();

    struct superbloque SB;
    bread(posSB, &SB);
    unsigned int p_inodo_dir, p_inodo;
    p_inodo_dir = p_inodo = SB.posInodoRaiz;

    unsigned int p_entrada = 0;
    int error;

    // Busca el archivo a linkear en el disco.
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4)) < 0)
    {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return FAILURE;
    }

    //Mirar que el inodo no sea el inodo raiz
    if (SB.posInodoRaiz == p_inodo)
    {
        mi_signalSem();
        fprintf(stderr, "mi_unlink: El inodo es el inodo raiz\n");
        return FAILURE;
    }

    //Inodo del archivo a borrar.
    struct inodo inodo;
    if (leer_inodo(p_inodo, &inodo) == FAILURE)
    {
        mi_signalSem();
        return FAILURE;
    }
    if ((inodo.tipo == 'd') && (inodo.tamEnBytesLog > 0))
    {
        mi_signalSem();
        return FAILURE;
    }

    //Inodo del directorio.
    struct inodo inodo_dir;
    if (leer_inodo(p_inodo_dir, &inodo_dir) == FAILURE)
    {
        mi_signalSem();
        return FAILURE;
    }

    //Obtener el numero de entradas
    int num_entrada = inodo_dir.tamEnBytesLog / sizeof(struct entrada);

    //Entrada a eliminar es la última
    //Si hay mas entradas
    if (p_entrada != num_entrada - 1)
    {

        // Leer la última y colocarla en la posición de la entrada que queremos eliminar
        struct entrada entrada;
        if (mi_read_f(p_inodo_dir, &entrada, sizeof(struct entrada) * (num_entrada - 1), sizeof(struct entrada)) < 0)
        {
            mi_signalSem();
            return FAILURE;
        }

        if (mi_write_f(p_inodo_dir, &entrada, sizeof(struct entrada) * (p_entrada), sizeof(struct entrada)) < 0)
        {
            mi_signalSem();
            return FAILURE;
        }
    }
    // Elimina la ultima entrada.
    if (mi_truncar_f(p_inodo_dir, sizeof(struct entrada) * (num_entrada - 1)) == FAILURE)
    {
        mi_signalSem();
        return FAILURE;
    }

    inodo.nlinks--;

    //Si no quedan enlaces (nlinks) entonces liberaremos el inodo
    if (!inodo.nlinks)
    {
        if (liberar_inodo(p_inodo) == FAILURE)
        {
            mi_signalSem();
            return FAILURE;
        }
    }
    else //Actualizamos su ctime y escribimos el inodo.
    {
        inodo.ctime = time(NULL);
        if (escribir_inodo(p_inodo, inodo) == FAILURE)
        {
            mi_signalSem();
            return FAILURE;
        }
    }
    mi_signalSem();
    return EXIT_SUCCESS;
}