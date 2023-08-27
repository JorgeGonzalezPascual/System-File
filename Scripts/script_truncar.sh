echo "################################################################################"
echo "$ rm disco"
make clean
make
rm disco
echo "$ ./mi_mkfs disco 100000"
echo "#inicializamos el sistema de ficheros con 100.000 bloques"
./mi_mkfs disco 100000
echo "################################################################################"
echo "$ ./leer_sf disco"
echo "#mostramos solo el SB"
./leer_sf disco
echo "################################################################################"
echo "$ ./escribir disco "123456789" 0"
./escribir disco "123456789" 0
echo "################################################################################"
echo "$ ./leer_sf disco"
echo "#mostramos solo el SB"
./leer_sf disco
echo "################################################################################"
echo "$ ./truncar disco 1 0"
time ./truncar disco 1 0

