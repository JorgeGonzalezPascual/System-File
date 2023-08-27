#include "simulacion.h"

#define DEBUG 1
#define HELP 0
int acabados = 0;

void reaper()
{
    pid_t ended;
    signal(SIGCHLD, reaper);
    while ((ended = waitpid(FAILURE, NULL, WNOHANG)) > 0)
    {
        acabados++;
        //Podemos testear qué procesos van acabando:
#if HELP
        fprintf(stderr, "[simulación.c → Acabado proceso con PID %d, total acabados: %d\n", ended, acabados);
#endif
    }
}

int main(int argc, char **argv)
{

    // Comprueba que la sintaxis sea correcta.
    if (argc != 2)
    {
        fprintf(stderr, RED "Error de sintaxis: ./simulacion <disco>\n" RESET);
        return FAILURE;
    }

    // Montamos el dispositivo virtual
    //Padre
    if (bmount(argv[1]) == FAILURE)
    {
        fprintf(stderr, "./simulacion: Error al  montar el dispositivo\n");
        exit(0);
    }

    // Creamos el directorio de simulacion
    char camino[21] = "/simul_";
    time_t time_now;
    time(&time_now);
    struct tm *tm = localtime(&time_now);
    sprintf(camino + strlen(camino), "%d%02d%02d%02d%02d%02d/",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

    if (mi_creat(camino, 6) == FAILURE)
    {
        fprintf(stderr, "./simulacion: Error al crear el directorio '%s'\n", camino);
        exit(0);
    }

    //Asociar sigchld al reaper()
    signal(SIGCHLD, reaper);

    pid_t pid;
    for (int proceso = 1; proceso <= NUMPROCESOS; proceso++)
    {
        pid = fork();

        if (pid == 0)
        {
            //Hijo
            bmount(argv[1]);
            // Crear directorio del proceso
            char nombredir[38];
            sprintf(nombredir, "%sproceso_%d/", camino, getpid());
            if (mi_creat(nombredir, 6) < 0)
            {
                fprintf(stderr, "./simulación.c: Error al crear el fichero del proceso\n");
                bumount();
                exit(0);
            }

            //Crear el fichero prueba.dat
            char nombrefich[48];
            sprintf(nombrefich, "%sprueba.dat", nombredir);
            if (mi_creat(nombrefich, 6) < 0)
            {
                fprintf(stderr, "./simulación.c: Error al crear el fichero prueba.dat del proceso\n");
                bumount();
                exit(0);
            }
#if HELP
            fprintf(stderr, "\nNombre de la ruta: %s\n", nombrefich);
#endif
            //Numeros random
            srand(time(NULL) + getpid());

            for (int nescritura = 0; nescritura < NUMESCRITURAS; nescritura++)
            {
                struct REGISTRO registro;
                registro.fecha = time(NULL);
                registro.pid = getpid();
                registro.nEscritura = nescritura + 1;
                registro.nRegistro = rand() % REGMAX;
                mi_write(nombrefich, &registro, (registro.nRegistro * sizeof(struct REGISTRO)), sizeof(struct REGISTRO));
#if HELP
                fprintf(stderr, "[simulación.c → Escritura %i en %s]\n", nescritura + 1, nombrefich);
                //fprintf(stderr, BLUE"registro.fecha =  %ld registro.pid = %d, registro.nEscritura = %d, registro.nRegistro = %d\n"RESET, registro.fecha, registro.pid, registro.nEscritura, registro.nRegistro);
#endif
                usleep(50000); //0,05 seg
            }
#if DEBUG
            fprintf(stderr, "[Proceso %d: Completadas %d escrituras en %s]\n", proceso, NUMESCRITURAS, nombrefich);
#endif
            //Desmontar el dispositivo (hijo)
            bumount();
            exit(0);
        }
        usleep(200000); //0,2 seg
    }
    //Permitir que el padre espere por todos los hijos:
    while (acabados < NUMPROCESOS)
    {
        pause();
    }
    if (bumount() == FAILURE)
    {
        fprintf(stderr, "./simulación.c: Error desmontando el dispositivo\n");
        exit(0);
    }
#if DEBUG
    fprintf(stderr, "Total de procesos terminados: %d\n", acabados);
#endif
    return EXIT_SUCCESS;
}