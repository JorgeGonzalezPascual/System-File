#Script para reiniciar disco. Perfecto para pruebas
echo "################################################################################"
make
echo "Make realizado"
echo "################################################################################"
./mi_mkfs disco 100000
echo "./mi_mkfs disco 100000"
echo "################################################################################"
./leer_sf disco
echo "./leer_sf disco"
echo "################################################################################"
make clean
echo "Limpio"
echo "################################################################################"