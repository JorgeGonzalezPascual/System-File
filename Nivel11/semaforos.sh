#Script para reiniciar disco. Perfecto para pruebas
echo "################################################################################"
make
echo "Make realizado"
echo "################################################################################"
./mi_mkfs disco 100000
echo "./mi_mkfs disco 100000"
echo "################################################################################"
gcc -pthread ejemplo_mutex_posix.c semaforo_mutex_posix.c -o ejemplo_mutex_posix
./ejemplo_mutex_posix
echo "./ejemplo_mutex_posix"
echo "################################################################################"
make clean
echo "Limpio"
echo "################################################################################"