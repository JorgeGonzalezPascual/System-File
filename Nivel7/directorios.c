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

    // Si el camino no comienza con '/' entonces error.
    if (camino[0] != '/')
    {
        return EXIT_FAILURE;
    }

    // Localizar la primera barra despues de la inicial.
    //+1 para evitar la primera '/'
    char *rest = strchr((camino + 1), '/');
    strcpy(tipo, "f");

    //Si se ha encotrado el caracter '/'
    if (rest)
    {
        //Copiamos todo en inicial menos el resto
        strncpy(inicial, (camino + 1), (strlen(camino) - strlen(rest) - 1));
        //Copiamos el resto en final
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

    //fprintf(stderr, "\tCamino: %s\n\tInicio: %s\n\tFinal: %s\n", camino, inicial, final);
    //fprintf(stderr, "\tTipo: %s\n", tipo);

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
        bread(posSB, &SB); //CONTROL DE ERRORES?

        *(p_inodo) = SB.posInodoRaiz; //nuestra raiz siempre estará asociada al inodo 0
        *(p_entrada) = 0;

        return EXIT_SUCCESS;
    }

    if (extraer_camino(camino_parcial, inicial, final, &tipo) == EXIT_FAILURE)
    {
        return ERROR_CAMINO_INCORRECTO;
    }

#if DEBUG
    printf("[buscar_entrada()->inicial: %s, final: %s, reservar: %d]\n", inicial,
           final, reservar);
#endif

    //buscamos la entrada cuyo nombre se encuentra en inicial
    if (leer_inodo(*p_inodo_dir, &dir_inodo) == EXIT_FAILURE)
    {
        return ERROR_PERMISO_LECTURA;
    }
    //fprintf(stderr, "\t\tp_inodo_dir: %d \n\t\t\dir_inodo.tipo: %c\n", *p_inodo_dir,dir_inodo.tipo);

    // Inicializa un array de entradas que caben en un bloque.
    struct entrada buff_lec[BLOCKSIZE / sizeof(struct entrada)];
    memset(buff_lec, 0, (BLOCKSIZE / sizeof(struct entrada)) * sizeof(struct entrada));

    cant_entradas_inodo = dir_inodo.tamEnBytesLog / sizeof(struct entrada); //cantidad de entradas que contiene el inodo
    num_entrada_inodo = 0;                                                  //nº de entrada inicial

    //int offset = 0;
    if (cant_entradas_inodo > 0)
    {
        if ((dir_inodo.permisos & 4) != 4)
        {
            return ERROR_PERMISO_LECTURA;
        }
        if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) < 0)
        {
            return ERROR_PERMISO_LECTURA;
        }

        while (num_entrada_inodo < cant_entradas_inodo && strcmp(inicial, entrada.nombre) != 0)
        {
            num_entrada_inodo++;
            if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) < 0)
                return ERROR_PERMISO_LECTURA;
        }
    }
    // Si inicial ≠ entrada.nombre:
    // Si inicial no se ha encontrado y se han procesado todas las entradas.
    if (num_entrada_inodo == cant_entradas_inodo && (inicial != buff_lec[num_entrada_inodo % (BLOCKSIZE / sizeof(struct entrada))].nombre))
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
                //copiar *inicial en el nombre de la entrada
                strcpy(entrada.nombre, inicial);
                if (tipo == 'd')
                {
                    //fprintf(stderr, "FINAL; %s\n", final);
                    if (strcmp(final, "/") == 0)
                    {
                        //reservar un nuevo inodo como directorio y asignarlo a la entrada
                        entrada.ninodo = reservar_inodo(tipo, permisos);
                        //entrada.ninodo = reservar_inodo('d', 6);
#if DEBUG
                        printf("[buscar_entrada()->reservado inodo: %d tipo %c con permisos %d para '%s']\n", entrada.ninodo, tipo, permisos, entrada.nombre);
#endif
                    }
                    else
                    { //cuelgan más diretorios o ficheros
                        //fprintf(stderr, "\tentrada.nombre: %s\n\tdir_inodo.tipo: %c\n", entrada.nombre, dir_inodo.tipo);
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else
                { //es un fichero
                    //reservar un inodo como fichero y asignarlo a la entrada
                    entrada.ninodo = reservar_inodo(tipo, permisos);
                    //entrada.ninodo = reservar_inodo('f', 6);
#if DEBUG
                    printf("[buscar()->reservado inodo: %d tipo %c con permisos %d para '%s']\n", entrada.ninodo, tipo, permisos, entrada.nombre);
#endif
                }

#if DEBUG
                fprintf(stderr, "[buscar_entrada()->creada entrada: %s, %d] \n", inicial, entrada.ninodo);
#endif

                //escribir la entrada en el directorio padre
                if (mi_write_f(*p_inodo_dir, &entrada, dir_inodo.tamEnBytesLog, sizeof(struct entrada)) == EXIT_FAILURE)
                {
                    if (entrada.ninodo != -1)
                    {
                        liberar_inodo(entrada.ninodo);
#if DEBUG
                        fprintf(stderr, "[buscar_entrada()-> liberado inodo %i, reservado a %s\n", num_entrada_inodo, inicial);
#endif
                    }
                    return EXIT_FAILURE;
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

        *(p_inodo) = num_entrada_inodo; //asignar a *p_inodo el numero de inodo del directorio o fichero creado o leido
        *(p_entrada) = entrada.ninodo;  //asignar a *p_entrada el número de su entrada dentro del último directorio que lo contiene

        //cortamos la recursividad
        return EXIT_SUCCESS;
    }
    else
    {

        *(p_inodo_dir) = entrada.ninodo; //asignamos a *p_inodo_dir el puntero al inodo que se indica en la entrada encontrada;
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }
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
