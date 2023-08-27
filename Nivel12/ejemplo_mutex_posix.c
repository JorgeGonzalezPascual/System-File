#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include "semaforo_mutex_posix.h"

/*
 * Para compilar:
 gcc -pthread ejemplo_mutex_posix.c semaforo_mutex_posix.c -o ejemplo_mutex_posix
 * i.e. usad la librería -pthread
 */

#define CHILDREN 5
int pid_children[CHILDREN];

void child(int id)
{
    sem_t *sem;
    int i;

    sem = initSem();

    sleep(1); // Espera que el padre cree todo los hijos
    for (i = 0; i < 100; i++)
    {
        waitSem(sem);
        printf("Hijo nro: %d, valor: %d\n", id, i);
        signalSem(sem);
    }
    printf("FIN hijo nro: %d\n", id);
}

int main(int argc, char **argv)
{
    sem_t *sem;
    int i;

    sem = initSem();
    if (sem == NULL)
    {
        puts("Error de semáforo");
        exit(1);
    }

    for (i = 0; i < CHILDREN; i++)
    { // Create children
        pid_children[i] = fork();
        if (pid_children[i] == 0)
        {
            child(i);
            exit(0);
        }
    }

    for (i = 0; i < 10; i++)
    {
        waitSem(sem); // Entro a sección crítica
        printf("Paso nr: %d\n", i);
        sleep(1);
        signalSem(sem);
        usleep(1); // Espera un tiempo para ceder paso a los hijos
    }

    for (i = 0; i < CHILDREN; i++)
    {
        kill(pid_children[i], SIGKILL); // Kill every child
    }

    deleteSem();
    return EXIT_SUCCESS;
}
