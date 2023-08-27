//Autores: Jorge González Pascual, Luis Clar Fiol
#include "verificacion.h"

#define DEBUG 1

int main(int argc, char const *argv[])
{

    if (argc != 3)
    {
        fprintf(stderr, RED "Error de sintaxis: ./verificacion <nombre_dispositivo> <directorio_simulación>\n" RESET);
        return FAILURE;
    }

    if (bmount(argv[1]) == FAILURE)
    {
        return FAILURE;
    }

    struct STAT st;
    mi_stat(argv[2], &st);

#if DEBUG
    fprintf(stderr, "Directorio de simulación: %s\n", argv[2]);
#endif

    int numentradas = (st.tamEnBytesLog / sizeof(struct entrada));
    if (numentradas != NUMPROCESOS)
    {
        printf(RED "verificacion.c: Error en el número de entradas.\n" RESET);
        bumount();
        return -1;
    }

#if DEBUG
    fprintf(stderr, "numentradas: %i, NUMPROCESOS: %i\n", numentradas, NUMPROCESOS);
#endif

    //directorio/informe.txt
    char nfichero[100];
    sprintf(nfichero, "%s%s", argv[2], "informe.txt");
    if (mi_creat(nfichero, 7) < 0)
    {
        bumount(argv[1]);
        exit(0);
    }

    //Cargamos las entradas
    struct entrada entrs[numentradas];
    int error = mi_read(argv[2], entrs, 0, sizeof(entrs));
    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
        return FAILURE;
    }

    int nbytes_info_f = 0;
    for (int nentr = 0; nentr < numentradas; nentr++)
    {

        //Leemos la entrada de directorio y extraemos el pid a partir del nombre
        // de la entrada
        pid_t pid = atoi(strchr(entrs[nentr].nombre, '_') + 1); //sacamos el pid a traves del nombre
        struct INFORMACION info;
        info.pid = pid;
        info.nEscrituras = 0;

        char f_prueba[128]; //camino fichero
        sprintf(f_prueba, "%s%s/%s", argv[2], entrs[nentr].nombre, "prueba.dat");

        //Buffer de N registros de escrituras
        int cant_registros_buffer_escrituras = 256 * 24; //Un multiple de BLOCKSIZE, en plataforma de 64bits
        struct REGISTRO buffer_escrituras[cant_registros_buffer_escrituras];
        memset(buffer_escrituras, 0, sizeof(buffer_escrituras));

        int offset = 0;
        //Mientras haya escrituras en prueba.dat
        while (mi_read(f_prueba, buffer_escrituras, offset, sizeof(buffer_escrituras)) > 0)
        {

            //iterador escrituras buffer
            int nregistro = 0;
            while (nregistro < cant_registros_buffer_escrituras)
            {
                //Si es valida
                if (buffer_escrituras[nregistro].pid == info.pid)
                {
                    //Si es la Primera escritura validada
                    if (!info.nEscrituras)
                    {
                        info.MenorPosicion = buffer_escrituras[nregistro];
                        info.MayorPosicion = buffer_escrituras[nregistro];
                        info.PrimeraEscritura = buffer_escrituras[nregistro];
                        info.UltimaEscritura = buffer_escrituras[nregistro];
                        info.nEscrituras++;
                    }
                    else
                    {
                        //Actualizamos los datos de las fechas la primera y la última escritura si se necesita
                        if ((difftime(buffer_escrituras[nregistro].fecha, info.PrimeraEscritura.fecha)) <= 0 &&
                            buffer_escrituras[nregistro].nEscritura < info.PrimeraEscritura.nEscritura)
                        {
                            info.PrimeraEscritura = buffer_escrituras[nregistro];
                        }
                        if ((difftime(buffer_escrituras[nregistro].fecha, info.UltimaEscritura.fecha)) >= 0 &&
                            buffer_escrituras[nregistro].nEscritura > info.UltimaEscritura.nEscritura)
                        {
                            info.UltimaEscritura = buffer_escrituras[nregistro];
                        }
                        if (buffer_escrituras[nregistro].nRegistro < info.MenorPosicion.nRegistro)
                        {
                            info.MenorPosicion = buffer_escrituras[nregistro];
                        }
                        if (buffer_escrituras[nregistro].nRegistro > info.MayorPosicion.nRegistro)
                        {
                            info.MayorPosicion = buffer_escrituras[nregistro];
                        }
                        info.nEscrituras++;
                    }
                }
                nregistro++;
            }
            memset(&buffer_escrituras, 0, sizeof(buffer_escrituras));
            offset += sizeof(buffer_escrituras);
        }

#if DEBUG
        fprintf(stderr, "[%i) %i escrituras validadas en %s]\n", nentr + 1, info.nEscrituras, f_prueba);
#endif
        //Añadimos la informacion del struct info en el fichero
        char tiempoPrimero[100];
        char tiempoUltimo[100];
        char tiempoMenor[100];
        char tiempoMayor[100];
        struct tm *tm;

        tm = localtime(&info.PrimeraEscritura.fecha);
        strftime(tiempoPrimero, sizeof(tiempoPrimero), "%a %Y-%m-%d %H:%M:%S", tm);
        tm = localtime(&info.UltimaEscritura.fecha);
        strftime(tiempoUltimo, sizeof(tiempoUltimo), "%a %Y-%m-%d %H:%M:%S", tm);
        tm = localtime(&info.MenorPosicion.fecha);
        strftime(tiempoMenor, sizeof(tiempoMenor), "%a %Y-%m-%d %H:%M:%S", tm);
        tm = localtime(&info.MayorPosicion.fecha);
        strftime(tiempoMayor, sizeof(tiempoMayor), "%a %Y-%m-%d %H:%M:%S", tm);

        char buffer[BLOCKSIZE];
        memset(buffer, 0, BLOCKSIZE);

        sprintf(buffer, "PID: %i\nNumero de escrituras: %i\n", pid, info.nEscrituras);
        sprintf(buffer + strlen(buffer), "%s %i %i %s",
                "Primera escritura",
                info.PrimeraEscritura.nEscritura,
                info.PrimeraEscritura.nRegistro,
                asctime(localtime(&info.PrimeraEscritura.fecha)));

        sprintf(buffer + strlen(buffer), "%s %i %i %s",
                "Ultima escritura",
                info.UltimaEscritura.nEscritura,
                info.UltimaEscritura.nRegistro,
                asctime(localtime(&info.UltimaEscritura.fecha)));

        sprintf(buffer + strlen(buffer), "%s %i %i %s",
                "Menor posicion",
                info.MenorPosicion.nEscritura,
                info.MenorPosicion.nRegistro,
                asctime(localtime(&info.MenorPosicion.fecha)));

        sprintf(buffer + strlen(buffer), "%s %i %i %s",
                "Mayor posicion",
                info.MayorPosicion.nEscritura,
                info.MayorPosicion.nRegistro,
                asctime(localtime(&info.MayorPosicion.fecha)));

        sprintf(buffer,
                "PID: %d\nNumero de escrituras:\t%d\nPrimera escritura:"
                "\t%d\t%d\t%s\nUltima escritura:\t%d\t%d\t%s\nMayor po"
                "sición:\t\t%d\t%d\t%s\nMenor posición:\t\t%d\t%d\t%s\n\n",
                info.pid, info.nEscrituras,
                info.PrimeraEscritura.nEscritura,
                info.PrimeraEscritura.nRegistro,
                tiempoPrimero,
                info.UltimaEscritura.nEscritura,
                info.UltimaEscritura.nRegistro,
                tiempoUltimo,
                info.MenorPosicion.nEscritura,
                info.MenorPosicion.nRegistro,
                tiempoMenor,
                info.MayorPosicion.nEscritura,
                info.MayorPosicion.nRegistro,
                tiempoMayor);
        //Escribimos en prueba.dat y actualizamos offset
        if ((nbytes_info_f += mi_write(nfichero, &buffer, nbytes_info_f, strlen(buffer))) < 0)
        {
            printf("verifiacion.c: Error al escribir el fichero: '%s'\n", nfichero);
            bumount();
            return FAILURE;
        }
    }
    bumount();
}