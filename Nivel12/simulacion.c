//Autores: Jorge González Pascual, Luis Clar Fiol
#include "simulacion.h"

#define DEBUG 1

int acabados = 0;

void reaper()
{
    pid_t ended;
    signal(SIGCHLD, reaper);
    while ((ended = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        acabados++;
#if DEBUG
        fprintf(stderr, "[simulación.c → Acabado proceso con PID %d, total acabados: %d\n", ended, acabados);
#endif
    }
}

int main(int argc, char const *argv[])
{
    //Asociar sigchld al reaper()
    signal(SIGCHLD, reaper);

    // Comprueba que la sintaxis sea correcta.
    if (argc != 2)
    {
        fprintf(stderr, RED "Error de sintaxis: ./simulacion <disco>\n" RESET);
        return FAILURE;
    }

    if (bmount(argv[1]) == FAILURE)
    {
        return FAILURE;
    }


    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char *tiempo = malloc(14);
    sprintf(tiempo, "%d%02d%02d%02d%02d%02d", tm.tm_year + 1900,
            tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    char *camino = malloc(14 + 8);
    strcat(strcpy(camino, "/simul_"), tiempo);
    strcat(camino, "/");
    fprintf(stderr, "Directorio de simulación:%s \n", camino);

    char buf[80];
    strcpy(buf, camino);

    fprintf(stderr, "Buffer de simulación:%s \n", buf);

    if ((mi_creat(camino, 6)) < 0)
    {
        //mostrar_error_buscar_entrada(error);
        return EXIT_FAILURE;
    }
    fprintf(stderr, "Directorio creado\n");

    for (int proceso = 1; proceso <= NUMPROCESOS; proceso++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            bmount(argv[1]); //Motar el disco para el proceso hijo
            char nombredir[80];
            sprintf(nombredir, "%sproceso_%d/", buf, getpid());
            if (mi_creat(nombredir, 6) < 0)
            {
                //mostrar_error_buscar_entrada(error);
                bumount();
                exit(0);
            }

            //Creamos el fichero
            char nfichero[100];
            sprintf(nfichero, "%sprueba.dat", nombredir);
            if (mi_creat(nfichero, 4) < 0)
            {
                //mostrar_error_buscar_entrada(error);
                bumount();
                exit(0);
            }
            fprintf(stderr, "Fichero del proceso %i creado\n", proceso);

            srand(time(NULL) + getpid()); //Valor random
            for (int escritura = 1; escritura <= NUMESCRITURAS; escritura++)
            {
                struct REGISTRO r;

                r.fecha = time(NULL);
                r.pid = getpid();
                r.nEscritura = escritura;
                r.nRegistro = rand() % REGMAX;

                mi_write(nfichero, &r, r.nRegistro * sizeof(struct REGISTRO), sizeof(struct REGISTRO));
#if DEBUG
                fprintf(stderr, "[simulación.c → Escritura %i en %s]\n", escritura, buf);
#endif
                usleep(50000);
            }

            bumount();
            exit(0);
        }

        usleep(200000);
    }

    while (acabados < NUMPROCESOS)
    {
        pause();
    }

    if (bumount() < 0)
    {
        fprintf(stderr, "Error al desmontar el dispositivo\n");
        exit(0);
    }

    fprintf(stderr, "Total de procesos terminados: %d\n", acabados);
}