//Autores: Jorge González Pascual, Luis Clar Fiol
#include "directorios.h"
#define DEBUG 1

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
 *      Error: EXIT_FAILURE
 * 
 * https://stackoverflow.com/questions/4170249/int-num-int-number-what-does-this-do
 */
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{
    int dev;

    // Si el camino no comienza con '/' entonces error.
    if (camino[0] != '/')
    {
        return ERROR_CAMINO_INCORRECTO;
    }

    // Localizar la primera barra despues de la inicial.
    //+1 para evitar la primera '/'
    char *rest = strchr((camino + 1), '/');
    strcpy(tipo, "f");
    dev = 0;

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
            dev = 1;
        }
    }
    else //Si no se ha encotrado
    {
        //Inicial = camino
        strcpy(inicial, (camino + 1));
        //Final: vacio
        strcpy(final, "");
    }

    //return dev;
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

        *(p_inodo) = SB.posInodoRaiz; //nuestra raiz siempre estará asociada al inodo 0
        *(p_entrada) = 0;

        return EXIT_SUCCESS;
    }

    memset(inicial, 0, sizeof(entrada.nombre));
    memset(final, 0, strlen(camino_parcial));
    if (extraer_camino(camino_parcial, inicial, final, &tipo) == EXIT_FAILURE)
    {
        return ERROR_CAMINO_INCORRECTO;
    }

#if DEBUG
    printf("[buscar_entrada()->inicial: %s, final: %s, reservar: %d]\n", inicial, final, reservar);
#endif

    //buscamos la entrada cuyo nombre se encuentra en inicial
    leer_inodo(*(p_inodo_dir), &dir_inodo);
    //Comprobamos que el inodo tenga permiso de lectura.
    if ((dir_inodo.permisos & 4) != 4)
    {
        return ERROR_PERMISO_LECTURA;
    }

    // Inicializa un array de entradas que caben en un bloque.
    struct entrada buff_lec[BLOCKSIZE / sizeof(struct entrada)];
    memset(buff_lec, 0, (BLOCKSIZE / sizeof(struct entrada)) * sizeof(struct entrada));

    cant_entradas_inodo = dir_inodo.tamEnBytesLog / sizeof(struct entrada); //cantidad de entradas que contiene el inodo
    num_entrada_inodo = 0;                                                  //nº de entrada inicial

    /*     if (cant_entradas_inodo > 0)
    {
        if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) < 0)
        {
            return ERROR_PERMISO_LECTURA;
        }

        //memset(buff_lec, 0, (BLOCKSIZE / sizeof(struct entrada)) * sizeof(struct entrada));
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
    } */
    int b_leidos = 0;
    if (cant_entradas_inodo > 0)
    {
        b_leidos += mi_read_f(*p_inodo_dir, &buff_lec, b_leidos, BLOCKSIZE);

        while ((num_entrada_inodo < cant_entradas_inodo) && (strcmp(inicial, buff_lec[num_entrada_inodo].nombre) != 0))
        {

            num_entrada_inodo++;
            if ((num_entrada_inodo % (BLOCKSIZE / sizeof(struct entrada))) == 0)
            {
                b_leidos += mi_read_f(*p_inodo_dir, &buff_lec, b_leidos, BLOCKSIZE);
            }
        }
    }
    // Si inicial ≠ entrada.nombre:
    // Si inicial no se ha encontrado y se han procesado todas las entradas.
    //if (num_entrada_inodo == cant_entradas_inodo && (inicial != buff_lec[num_entrada_inodo % (BLOCKSIZE / sizeof(struct entrada))].nombre))
    //if (strcmp(entrada.nombre, inicial) != 0)
    if (strcmp(buff_lec[num_entrada_inodo].nombre, inicial) != 0)
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
                        //entrada.ninodo = reservar_inodo(tipo, permisos);
                        entrada.ninodo = reservar_inodo('d', 6);
#if DEBUG
                        printf("[buscar_entrada()->reservado inodo: %d tipo %c con permisos %d para '%s']\n", entrada.ninodo, tipo, permisos, entrada.nombre);
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
                    //entrada.ninodo = reservar_inodo(tipo, permisos);
                    entrada.ninodo = reservar_inodo('f', 6);
#if DEBUG
                    printf("[buscar()->reservado inodo: %d tipo %c con permisos %d para '%s']\n", entrada.ninodo, tipo, permisos, entrada.nombre);
#endif
                }

#if DEBUG
                fprintf(stderr, "[buscar_entrada()->creada entrada: %s, %d] \n", inicial, entrada.ninodo);
#endif

                //escribir la entrada en el directorio padre
                //if (mi_write_f(*p_inodo_dir, &entrada, dir_inodo.tamEnBytesLog, sizeof(struct entrada)) == -1)
                if (mi_write_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) == -1)
                {
                    if (entrada.ninodo != -1)
                    {
                        liberar_inodo(entrada.ninodo);
#if DEBUG
                        fprintf(stderr, "[buscar_entrada()-> liberado inodo %i, reservado a %s\n", num_entrada_inodo, inicial);
#endif
                    }
                    return -1; //return EXIT_FAILURE;
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
        *p_inodo = buff_lec[num_entrada_inodo].ninodo; //asignar a *p_inodo el numero de inodo del directorio o fichero creado o leido
        *p_entrada = num_entrada_inodo;                //asignar a *p_entrada el número de su entrada dentro del último directorio que lo contiene

        return EXIT_SUCCESS;
    }
    else
    {

        *p_inodo_dir = buff_lec[num_entrada_inodo].ninodo; //asignamos a *p_inodo_dir el puntero al inodo que se indica en la entrada encontrada;
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
    case -1:
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
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos)) < 0)
    {
        return error;
    }
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
*       EXIT_FAILURE
*/
int mi_dir(const char *camino, char *buffer, char *tipo)
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
        return error;
    }

    struct inodo inodo;
    if (leer_inodo(p_inodo, &inodo) == EXIT_FAILURE)
    {
        return -1;
    }
    if ((inodo.permisos & 4) != 4)
    {
        return -1;
    }

    //struct entrada entrada;

    char tmp[100];       //Para el tiempo
    char tamEnBytes[10]; //10 = valor maximo de un unsigned int


        if (leer_inodo(p_inodo, &inodo) == EXIT_FAILURE)
        {
            return -1;
        }
        *tipo = inodo.tipo;

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
            if (leer_inodo(entradas[i % (BLOCKSIZE / sizeof(struct entrada))].ninodo, &inodo) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
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

            strcat(buffer, RESET);
            strcat(buffer, "\n"); //Preparamos el string para la siguiente entrada

            if (offset % (BLOCKSIZE / sizeof(struct entrada)) == 0)
            {
                offset += mi_read_f(p_inodo, entradas, offset, BLOCKSIZE);
            }
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
*      Error: El codigo de error o -1
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

    if (mi_stat_f(p_inodo, p_stat) == EXIT_FAILURE)
    {
        return -1;
    }

    return p_inodo;
}