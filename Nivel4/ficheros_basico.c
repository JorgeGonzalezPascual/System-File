//Autores: Jorge González Pascual, Luis Clar Fiol
#include "ficheros_basico.h"

#define DEBUG3 0 //Debugger del nivel 3
#define DEBUG4 1 //Debugger del nivel 4

/**
 *  Función: tamMB(unsigned int nbloques)
 * ---------------------------------------------------------------------
 * Calcula el tamaño en bloques necesarios para el mapa de 
 * bits.
 * 
 * IN: nbloques = número de bloques en el disco.
 * OUT: tamaño del mapa de bits en bloques.
 */
int tamMB(unsigned int nbloques)
{
    div_t tam = div((nbloques / 8), BLOCKSIZE);
    //Si el resto es diferente a 0
    if (tam.rem > 0)
    {
        //Coeciente + 1. Ajustamos la cantidad
        return tam.quot + 1;
    }

    return tam.quot;
}

/**
 *  Función: tamAI(unsigned int ninodos)
 * ---------------------------------------------------------------------
 * Calcula el tamaño en bloques del array de inodos.
 * 
 * IN: ninodos = número de indodos en el disco
 * OUT: tamaño del Array de inodos en bloques.
 */
int tamAI(unsigned int ninodos)
{
    div_t tam = div((ninodos * INODOSIZE), BLOCKSIZE);
    //Si el resto es diferente a 0
    if (tam.rem > 0)
    {
        //Coeciente + 1. Ajustamos la cantidad
        return tam.quot + 1;
    }

    return tam.quot;
}

/**
 *  Función: initSB(unsigned int nbloques, unsigned int ninodos)
 * ---------------------------------------------------------------------
 * Inicializa los datos del superbloque.
 * 
 * IN: nbloques = número de bloques en el disco
 *     ninodos = número de indodos en el disco
 * OUT: EXIT_SUCCESS
 *      EXIT_FAILURE
 */
int initSB(unsigned int nbloques, unsigned int ninodos)
{
    //Creacion del superbloque
    struct superbloque SB;

    //Posición del primer bloque del mapa de bits
    SB.posPrimerBloqueMB = posSB + tamSB;
    //Posición del último bloque del mapa de bits
    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
    //Posición del primer bloque del array de inodos
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
    //Posición del último bloque del array de inodos
    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;
    //Posición del primer bloque de datos
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
    //Posición del último bloque de datos
    SB.posUltimoBloqueDatos = nbloques - 1;
    //Posición del inodo del directorio raíz en el array de inodos
    SB.posInodoRaiz = 0;
    //Posición del primer inodo libre en el array de inodos
    SB.posPrimerInodoLibre = 0; 
    //Cantidad de bloques libres en el SF
    SB.cantBloquesLibres = nbloques;
    //Cantidad de inodos libres en el array de inodos
    SB.cantInodosLibres = ninodos;
    //Cantidad total de bloques
    SB.totBloques = nbloques;
    //Cantidad total de inodos
    SB.totInodos = ninodos;

    //Para finalizar, escribimos la estructura en el bloque posSB con bwrite()
    if (bwrite(posSB, &SB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 *  Función: initMB()
 * ---------------------------------------------------------------------
 * Inicializa el mapa de bits.
 * 
 * IN: -
 * OUT: EXIT_SUCCESS
 *      EXIT_FAILURE
*/
int initMB()
{
    //Buffer
    unsigned char buffer[BLOCKSIZE];

    //Todas posiciones a 0.
    if (memset(buffer, 0, BLOCKSIZE) == NULL)
    {
        return EXIT_FAILURE;
    }

    //Leemos el superbloque
    struct superbloque SB;
    if (bread(posSB, &SB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    //Tamaño MB
    int tamMB = SB.posUltimoBloqueMB - SB.posPrimerBloqueMB;
    //Inicializa bloque a bloque el Mapa de bits
    for (int i = SB.posPrimerBloqueMB; i <= tamMB + SB.posPrimerBloqueMB; i++)
    {
        if (bwrite(i, buffer) == EXIT_FAILURE)
        {
            return EXIT_FAILURE;
        }
    }

    //Ponemos a 1 en el MB los bits que corresponden a los bloques que ocupa el
    //superbloque, el propio MB, y el array de inodos.
    for (unsigned int i = posSB; i < SB.posPrimerBloqueDatos; i++)
    {
        //Podriamos reservar todos los bloques de los metadatos:
        reservar_bloque();
    }

    return EXIT_SUCCESS;
}

/**
 *  Función: initAI()
 * ---------------------------------------------------------------------
 * Inicializar la lista de inodos libres.
 * 
 * IN: -
 * OUT: EXIT_SUCCESS
 *      EXIT_FAILURE
 */
int initAI()
{
    //Buffer
    unsigned char buffer[BLOCKSIZE];

    //Todas posiciones a 0.
    if (memset(buffer, 0, BLOCKSIZE) == NULL)
    {
        return EXIT_FAILURE;
    }

    //Inicialización estructura de inodos.
    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    //Leemos el superbloque
    struct superbloque SB;
    if (bread(posSB, &SB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    int end = 0;
    int contInodos = SB.posPrimerInodoLibre + 1;
    for (int i = SB.posPrimerBloqueAI; (i <= SB.posUltimoBloqueAI) && end == 0; i++)
    {
        for (int j = 0; j < (BLOCKSIZE / INODOSIZE); j++)
        {
            inodos[j].tipo = LIBRE; //Inicializar el contenido del inodo
            if (contInodos < SB.totInodos)
            {
                inodos[j].punterosDirectos[0] = contInodos;
                contInodos++;
            }
            else
            {
                //Al llegar al último nodo
                inodos[j].punterosDirectos[0] = UINT_MAX;
                //Forzar salida
                end = 1;
                break;
            }
        }

        //Escribir el bloque de inodos en el dispositivo virtual
        if (bwrite(i, &inodos) == EXIT_FAILURE)
        {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/**
 *  Función: int escribir_bit(unsigned int nbloque, unsigned int bit);
 * ---------------------------------------------------------------------
 * Escribe el valor indicado por el parámetro en un determinado bit del MB.
 * La utilizaremos cada vez que necesitemos reservar o liberar un bloque.
 * 
 * NOTA: Después de esta función se tendría que volver a LEER el SB
 *
 * IN: nbloque: bloque que pertenece al bit del MB 
 *     bit: 0 (libre) ó 1 (ocupado)
 * OUT: EXIT_SUCCESS
 *      EXIT_FAILURE
 */
int escribir_bit(unsigned int nbloque, unsigned int bit)
{
    //Leer el superbloque para obtener la localización del MB.
    struct superbloque SB;
    if (bread(posSB, &SB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    //Declaración y cálculo de variables.
    unsigned int posbyte = nbloque / 8;                         //Calculamos la posición del byte en el MB
    unsigned int posbit = nbloque % 8;                          //Luego la posición del bit dentro de ese byte
    unsigned int nbloqueMB = posbyte / BLOCKSIZE;               //Determinar luego en qué bloque del MB
    unsigned int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB; //Posición absoluta del dispositivo virtual

    unsigned char bufferMB[BLOCKSIZE];

    //Leemos el bloque que contiene el bit
    if (bread(nbloqueabs, bufferMB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    //Localizacion del byte dentro del bloque
    posbyte = posbyte % BLOCKSIZE;

    //Máscara y desplazamiento de bits
    unsigned char mascara = 128; // 10000000
    mascara >>= posbit;          // desplazamiento de bits a la derecha

    //Segun el parámetro 'bit' tendremos que cambiar la logica de la máscara
    //unsigned int cero = 0;
    //if (bit != cero)
    if (bit == 1)
    {
        bufferMB[posbyte] |= mascara; //  operador OR para bits
    }
    else
    {
        bufferMB[posbyte] &= ~mascara; // AND y NOT.
    }

    //Escribimos el bufferMB en el dispositivo virtual
    if (bwrite(nbloqueabs, bufferMB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 *  Función: char leer_bit(unsigned int nbloque);
 * ---------------------------------------------------------------------
 * Lee un determinado bit del MB y devuelve el valor del bit leído.
 * 
 * IN: nbloque: bloque que pertenece al bit del MB.
 * OUT: Valor del bit leído.
 */
char leer_bit(unsigned int nbloque)
{
    //Leer el superbloque para obtener la localización del MB.
    struct superbloque SB;
    if (bread(posSB, &SB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    //Declaración y cálculo de variables.
    unsigned int posbyte = nbloque / 8;                         //Calculamos la posición del byte en el MB
    unsigned int posbit = nbloque % 8;                          //Luego la posición del bit dentro de ese byte
    unsigned int nbloqueMB = posbyte / BLOCKSIZE;               //Determinar luego en qué bloque del MB
    unsigned int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB; //Posición absoluta del dispositivo virtual
    unsigned char bufferMB[BLOCKSIZE];

    //Leemos el bloque que contiene el bit
    if (bread(nbloqueabs, bufferMB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    //Localizacion del byte dentro del bloque
    posbyte = posbyte % BLOCKSIZE;

    //Máscara y desplazamiento de bits
    unsigned char mascara = 128;  // 10000000
    mascara >>= posbit;           // desplazamiento de bits a la derecha
    mascara &= bufferMB[posbyte]; // operador AND para bits
    mascara >>= (7 - posbit);     // desplazamiento de bits a la derecha

#if DEBUG3
    printf("[leer_bit(%i) → posbyte:%i, posbit:%i, nbloqueMB:%i, nbloqueabs:%i)]\n\n", nbloque, posbyte, posbit, nbloqueMB, nbloqueabs);
#endif

    return mascara;
}

/**
 *  Función: int reservar_bloque();
 * ---------------------------------------------------------------------
 * Encuentra el primer bloque libre, consultando el MB, lo ocupa 
 * (con la ayuda de la función escribir_bit()) y devuelve su posición.
 *
 * NOTA: Después de esta función se tendría que volver a LEER el SB
 *
 * IN: -
 * OUT: nº de bloque que hemos reservado
 */
int reservar_bloque()
{
    //Leer el superbloque del dispositivo virtual
    struct superbloque SB;
    if (bread(posSB, &SB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    //Compruebamos si hay bloques libres en el disco.
    if (SB.cantBloquesLibres == 0)
    {
        return EXIT_FAILURE;
    }

    unsigned char bufferMB[BLOCKSIZE];
    unsigned char bufferaux[BLOCKSIZE];
    unsigned int posBloqueMB = SB.posPrimerBloqueMB;
    int libre = 0;

    //llenamos de 1's el buffer auxiliar
    if (memset(bufferaux, 255, BLOCKSIZE) == NULL)
    {
        return EXIT_FAILURE;
    }

    //Localizamos el primer bloque que contenga un 0
    while (libre == 0) // No lee mas que una linea
    {
        if (bread(posBloqueMB, bufferMB) == EXIT_FAILURE) // Que cojones esta leyendo aqui
        {
            fprintf(stderr, "Error while reading at reservar_bloque()\n");
            return EXIT_FAILURE;
        }

        int iguales = memcmp(bufferMB, bufferaux, BLOCKSIZE); // AQUI FALLA porque ve que hay un bloque libre
        if (iguales != 0)
        {
            libre = 1;
            break;
        }
        posBloqueMB++;
    }

    //Localizamos el byte que contiene el 0 dentro del bloque encontrado anteriormente
    unsigned int posbyte = 0;
    while (bufferMB[posbyte] == 255)
    {
        posbyte++;
    }

    //Localizamos el bit que es un 0 dentro del byte encontrado anteriormente
    unsigned char mascara = 128; // 10000000
    unsigned int posbit = 0;
    while (bufferMB[posbyte] & mascara) // operador AND para bits
    {
        bufferMB[posbyte] <<= 1; // desplazamiento de bits a la izquierda
        posbit++;
    }

    //Posición absoluta del bit en el dispositivo virtual
    unsigned int nbloque = ((posBloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE + posbyte) * 8 + posbit;
    // printf("posBloqueMB : %d \nSB.posPrimerBloqueMB : %d \nposbyte : %d\nposbit : %d\n\n",posBloqueMB, SB.posPrimerBloqueMB, posbyte, posbit);

    if (escribir_bit(nbloque, 1) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    //Decrementamos el número de bloques libres en 1
    SB.cantBloquesLibres--;

    // Rellenar el bufffer con 0's
    if (memset(bufferaux, 0, BLOCKSIZE) == NULL)
    {
        fprintf(stderr, "Error while memset in reservar_bloque()\n");
        return EXIT_FAILURE;
    }

    //POSIBLE PROBLEMA FALTA REVISAR///////////////////////////////////////////////
    //Escribimos en ese bloque el buffer anterior por si habia información "basura"
    if (bwrite(SB.posPrimerBloqueDatos + nbloque - 1, bufferaux) == EXIT_FAILURE)
    {
        fprintf(stderr, "Error while writting in reservar_bloque()\n");
        return EXIT_FAILURE;
    }
    ////////////////////////////////////////////////////////////////////////////////

    //Salvamos el superbloque
    if (bwrite(posSB, &SB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    return nbloque;
}

/**
 * Función: int liberar_bloque(unsigned int nbloque);
 * ---------------------------------------------------------------------
 *Libera un bloque determinado (con la ayuda de la función escribir_bit()).
 * 
 * NOTA: Después de esta función se tendría que volver a LEER el SB
 * 
 * IN: nbloque a liberar
 * OUT: nº de bloque liberado
*/
int liberar_bloque(unsigned int nbloque)
{
    //Leer el superbloque del dispositivo virtual
    struct superbloque SB;
    if (bread(posSB, &SB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    escribir_bit(nbloque, 0);

    SB.cantBloquesLibres++;

    //Salvamos el superbloque
    if (bwrite(posSB, &SB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    return nbloque;
}

/**
 *  Función: int escribir_inodo(unsigned int ninodo, struct inodo inodo);
 * ---------------------------------------------------------------------
 * Escribe el contenido de una variable de tipo struct inodo en un 
 * determinado inodo del array de inodos, inodos.
 * 
 * NOTA: Después de esta función se tendría que volver a LEER el SB
 *
 * IN: unsigned int ninodo 
 *     struct inodo inodo
 * OUT: EXIT_SUCCESS
 *      EXIT_FAILURE
 */
int escribir_inodo(unsigned int ninodo, struct inodo inodo)
{
    //Leer el superbloque del dispositivo virtual
    struct superbloque SB;
    if (bread(posSB, &SB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    //Operación para obtener el bloque donde se encuentra un inodo dentro del AI
    unsigned int posBloqueInodo = SB.posPrimerBloqueAI + (ninodo / (BLOCKSIZE / INODOSIZE));

    //Array de inodos
    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    //Leemos el bloque del array de inidos correspondiente
    if (bread(posBloqueInodo, inodos) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    //Escribimos el inodo en el lugar correspondiente del array
    inodos[ninodo % (BLOCKSIZE / INODOSIZE)] = inodo;

    //El bloque modificado lo escribimos en el dispositivo virtual
    if (bwrite(posBloqueInodo, inodos) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 *  Función: int leer_inodo(unsigned int ninodo, struct inodo *inodo);
 * ---------------------------------------------------------------------
 * Lee un determinado inodo del array de inodos para volcarlo en una 
 * variable de tipo struct inodo pasada por referencia.
 * 
 * IN: unsigned int ninodo 
 *     struct inodo *inodo
 * OUT: EXIT_SUCCESS
 *      EXIT_FAILURE
 */
int leer_inodo(unsigned int ninodo, struct inodo *inodo)
{
    //Leer el superbloque del dispositivo virtual
    struct superbloque SB;
    if (bread(posSB, &SB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    //Operación para obtener el bloque donde se encuentra un inodo dentro del AI
    //unsigned int posBloqueInodo = ((ninodo * INODOSIZE) / BLOCKSIZE) + SB.posPrimerBloqueAI;
    unsigned int posBloqueInodo = SB.posPrimerBloqueAI + (ninodo / (BLOCKSIZE / INODOSIZE));
    //Array de inodos
    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    //Leemos el bloque del array de inidos correspondiente
    if (bread(posBloqueInodo, inodos) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    //Escribimos el inodo en el lugar correspondiente del array
    *inodo = inodos[ninodo % (BLOCKSIZE / INODOSIZE)];
    return EXIT_SUCCESS;
}

/**
 *  Función: int reservar_inodo(unsigned char tipo, unsigned char permisos);
 * ---------------------------------------------------------------------
 * Encuentra el primer inodo libre (dato almacenado en el superbloque),
 * lo reserva (con la ayuda de la función escribir_inodo())
 * 
 * Nota: Después de esta función se tendría que volver a LEER el SB
 *
 * In: unsigned char tipo: tipo de archivo
 *     unsigned char permisos: permisos a tener
 * Out: posInodoReservado
 *      EXIT_FAILURE
 * 
 */
int reservar_inodo(unsigned char tipo, unsigned char permisos)
{
    //Leer el superbloque del dispositivo virtual
    struct superbloque SB;
    if (bread(posSB, &SB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    //Compruebamos si hay bloques libres en el disco.
    if (SB.cantBloquesLibres == 0)
    {
        return EXIT_FAILURE;
    }

    //Actualizar la lista enlazada de inodos libres
    unsigned int posInodoReservado = SB.posPrimerInodoLibre;
    SB.posPrimerInodoLibre++;
    SB.cantInodosLibres--;

    struct inodo inodoAUX;
    //Inicialización
    inodoAUX.tipo = tipo;
    inodoAUX.permisos = permisos;
    inodoAUX.nlinks = 1;
    inodoAUX.tamEnBytesLog = 0;
    inodoAUX.atime = time(NULL);
    inodoAUX.mtime = time(NULL);
    inodoAUX.ctime = time(NULL);
    inodoAUX.numBloquesOcupados = 0;
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            inodoAUX.punterosIndirectos[j] = 0;
        }
        inodoAUX.punterosDirectos[i] = 0;
    }

    //Escribir el inodo inicializado en la posición del que era el primer inodo libre
    if (escribir_inodo(posInodoReservado, inodoAUX) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    //Escribimos el superbloque actualizado
    if (bwrite(posSB, &SB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    return posInodoReservado;
}

/**
 *  Función: int obtener_nRangoBL (struct inodo *inodo, unsigned int nblogico, unsigned int *ptr)
 * ---------------------------------------------------------------------
 * Función para obtener el rango de punteros en el que se sitúa el bloque lógico que buscamos
 * y obtenemos además la dirección almacenada en el puntero correspondiente del inodo. 
 * 
 * IN: struct inodo *inodo: inodo que contendra los punteros
 *     unsigned int nblogico: número de la posicion logica del bloque del inodo
 *     unsigned int *ptr: Puntero al siguiente nivel
 * OUT: Dirección almacenada en el puntero correspondiente del inodo
 *      Error: -1  
 */
int obtener_nRangoBL(struct inodo *inodo, unsigned int nblogico, unsigned int *ptr)
{
    if (nblogico < DIRECTOS)
    {
        *ptr = inodo->punterosDirectos[nblogico];
        return 0;
    }
    else if (nblogico < INDIRECTOS0)
    {
        *ptr = inodo->punterosIndirectos[0];
        return 1;
    }
    else if (nblogico < INDIRECTOS1)
    {
        *ptr = inodo->punterosIndirectos[1];
        return 2;
    }
    else if (nblogico < INDIRECTOS2)
    {
        *ptr = inodo->punterosIndirectos[2];
        return 3;
    }
    else
    { //Fuera de rango
        *ptr = 0;
        fprintf(stderr, "Error in obtener_nRangoBL(): Logic block out of range\n");
        return -1;
    }
}

/**
 * Función: int obtener_indice(int nblogico, int nivel_punteros)
 * ---------------------------------------------------------------------
 * Función para generalizar la obtención de los índices de los bloques 
 * de punteros.
 * 
 * IN: int nblogico
 *     int nivel_punteros
 * OUT: El indice de los bloques de punteros
 *      Error: -1 
 */
int obtener_indice(int nblogico, int nivel_punteros)
{

    //Nivel de bloques directos, comprende del 0 al 12
    if (nblogico < DIRECTOS)
    {
        return nblogico;
    }

    //Nivel de bloques indirectos 0, comprende del 13 al 268
    else if (nblogico < INDIRECTOS0)
    {
        return (nblogico - DIRECTOS);
    }

    //Nivel de bloques indirectos 1, comprende del 269 al 65.804
    else if (nblogico < INDIRECTOS1)
    {

        if (nivel_punteros == 2)
        {
            return ((nblogico - INDIRECTOS0) / NPUNTEROS);
        }
        else if (nivel_punteros == 1)
        {
            return ((nblogico - INDIRECTOS0) % NPUNTEROS);
        }

        //Nivel de bloques indirectos 2, comprende del 65.805 al 16.843.020
    }
    else if (nblogico < INDIRECTOS2)
    {

        if (nivel_punteros == 3)
        {
            return ((nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS));
        }
        else if (nivel_punteros == 2)
        {
            return (((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) / NPUNTEROS);
        }
        else if (nivel_punteros == 1)
        {
            return (((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) % NPUNTEROS);
        }
    }

    //Si nblogico > INDIRECTOS2, es un error y devolvemos -1 porque no podemos tratarlo.
    return -1;
}

/**
 * Función: traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar)
 * ---------------------------------------------------------------------
 * Esta función se encarga de obtener el nº de bloque físico correspondiente 
 * a un bloque lógico determinado del inodo indicado. Enmascara la gestión de 
 * los diferentes rangos de punteros directos e indirectos del inodo, de manera
 * que funciones externas no tienen que preocuparse de cómo acceder a los 
 * bloques físicos apuntados desde el inodo.
 * 
 * IN:  unsigned int ninodo: numero de inodo en el array de inodos
 *      unsigned int nblogico: posicion logica del bloque
 *      char reservar:  0 -> Solo consultar
 *                      1 -> Consultar y reservar un bloque
 *                      
 * OUT: Devuelve la posición fisica del bloque
 *      Error: -1        
 */
int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar)
{
    struct inodo inodo;
    unsigned int ptr;
    int ptr_ant, salvar_inodo, nRangoBL, nivel_punteros, indice;
    int buffer[NPUNTEROS];

    if (leer_inodo(ninodo, &inodo) == EXIT_FAILURE)
    {
        fprintf(stderr, "Error in traducir_bloque_inodo(): leer_inodo()\n");
        return -1;
    }

    ptr = 0;
    ptr_ant = 0;
    salvar_inodo = 0;

    nRangoBL = obtener_nRangoBL(&inodo, nblogico, &ptr); //0:D, 1:I0, 2:I1, 3:I2
    nivel_punteros = nRangoBL;                           //el nivel_punteros +alto es el que cuelga del inodo

    while (nivel_punteros > 0)
    { //iterar para cada nivel de indirectos
        if (ptr == 0)
        { //no cuelgan bloques de punteros
            if (reservar == 0)
            {
                fprintf(stderr, "Error in obtener_nRangoBL: lectura bloque inexistente\n");
                return -1;
            }
            else
            { //reservar bloques punteros y crear enlaces desde inodo hasta datos
                salvar_inodo = 1;
                ptr = reservar_bloque(); //de punteros
                inodo.numBloquesOcupados++;
                inodo.ctime = time(NULL); //fecha actual

                if (nivel_punteros == nRangoBL)
                {
                    //el bloque cuelga directamente del inodo
                    inodo.punterosIndirectos[nRangoBL - 1] = ptr; // (imprimirlo para test)
#if DEBUG4
                    printf("[traducir_bloque_inodo()→ inodo.punterosIndirectos[%i] = %i (reservado BF %i para punteros_nivel%i)]\n",
                           nRangoBL - 1, ptr, ptr, nivel_punteros);
#endif
                }
                else
                {
                    buffer[indice] = ptr; // (imprimirlo para test)

#if DEBUG4
                    printf("[traducir_bloque_inodo()→ inodo.punteros_nivel%i[%i] = %i (reservado BF %i para punteros_nivel%i)]\n",
                           nivel_punteros, indice, ptr, ptr, nivel_punteros);
#endif
                    if (bwrite(ptr_ant, buffer) == EXIT_FAILURE)
                    {
                        fprintf(stderr, "Error in obtener_nRangoBL: bwrite(ptr_ant, buffer)\n");
                        return -1;
                    }
                } //el bloque cuelga de otro bloque de punteros
            }
        }
        if (bread(ptr, buffer) == EXIT_FAILURE)
        {
            fprintf(stderr, "Error in obtener_nRangoBL: bread(ptr, buffer)\n");
            return -1;
        }
        indice = obtener_indice(nblogico, nivel_punteros);
        ptr_ant = ptr;        //guardamos el puntero
        ptr = buffer[indice]; // y lo desplazamos al siguiente nivel
        nivel_punteros--;
    } //al salir de este bucle ya estamos al nivel de datos

    if (ptr == 0)
    { //no existe bloque de datos

        if (reservar == 0)
        { //error lectura ∄ bloque
            return -1;
        }
        else
        {

            salvar_inodo = 1;
            ptr = reservar_bloque(); //de datos
            inodo.numBloquesOcupados++;
            inodo.ctime = time(NULL);

            if (nRangoBL == 0)
            {
                inodo.punterosDirectos[nblogico] = ptr; // (imprimirlo para test)
//Caso n1: nblogico = 8, ptr = 3139
#if DEBUG4
                printf("[traducir_bloque_inodo()→ inodo.punterosDirectos[%i] = %i (reservado BF %i para BL %i)]\n",
                       nblogico, ptr, ptr, nblogico);
#endif
            }
            else
            {
                buffer[indice] = ptr; // (imprimirlo para test)
#if DEBUG4
                printf("[traducir_bloque_inodo()→ inodo.punteros_nivel1[%i] = %i (reservado BF %i para BL %i)]\n",
                       indice, ptr, ptr, nblogico);
#endif

                if (bwrite(ptr_ant, buffer) == EXIT_FAILURE)
                {
                    fprintf(stderr, "Error in obtener_nRangoBL: bwrite(ptr_ant, buffer)\n");
                    return -1;
                }
            }
        }
    }

    if (salvar_inodo == 1)
    {
        if (escribir_inodo(ninodo, inodo) == EXIT_FAILURE)
        {
            fprintf(stderr, "Error en salvar inodo: escribir_inodo(ninodo, inodo)\n");
            return -1;
        }
        //sólo si lo hemos actualizado
    }

    return ptr; //nbfisico del bloque de datos
}
