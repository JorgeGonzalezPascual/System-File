
echo "$ rm disco"
make clean
make
rm disco
echo "$ ./mi_mkfs disco 100000"
echo "#inicializamos el sistema de ficheros con 100.000 bloques"
./mi_mkfs disco 100000

echo "$ ./leer_sf disco"
./leer_sf disco

echo "$ ./escribir disco "123456789" 0"
./escribir disco "123456789" 0

echo "$ ./leer_sf disco"
./leer_sf disco

echo "$ ./truncar disco 1 409605001"
./truncar disco 1 409605001

echo "$ ./leer_sf disco"
./leer_sf disco

echo "$ ./truncar disco 1 30725003"
./truncar disco 1 30725003

echo "$ ./leer_sf disco"
./leer_sf disco

echo "$ ./truncar disco 1 209008"
./truncar disco 1 209008

echo "$ ./leer_sf disco"
./leer_sf disco

echo "$ ./truncar disco 1 9005"
./truncar disco 1 9005

echo "$ ./leer_sf disco"
./leer_sf disco

echo "$ ./truncar disco 1 0"
./truncar disco 1 0

echo "$ ./leer_sf disco"
./leer_sf disco
