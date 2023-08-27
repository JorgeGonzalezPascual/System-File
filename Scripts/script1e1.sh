echo "################################################################################"
echo "$ rm disco"
rm disco
echo "$ ./mi_mkfs disco 100000"
echo "#inicializamos el sistema de ficheros con 100.000 bloques"
./mi_mkfs disco 100000
echo "################################################################################"
echo "$ ./leer_sf disco"
echo "#mostramos solo el SB"
./leer_sf disco
echo "################################################################################"
echo "$ ./escribir"
echo "#consultamos sintaxis comando"
./escribir
echo "################################################################################"
echo "$ ./escribir disco "123456789" 0"
echo "#escribimos el texto “123456789” en los offsets  9000, 209000, 30725000, "
echo "#409605000 y 480000000 de un mismo inodo"
./escribir disco "123456789" 0
echo "################################################################################"
echo "$ ./leer disco 1 > ext1.txt"
echo "#leemos el contenido del inodo 1 y lo direccionamos al fichero externo ext1.txt"
./leer disco 1 > ext1.txt
echo "################################################################################"
echo "$ ls -l ext1.txt"
echo "#comprobamos cuánto ocupa el fichero externo"
echo "#(ha de coincidir con el tamaño en bytes lógico del inodo y con los bytes leídos)"
ls -l ext1.txt
echo "################################################################################"
echo "$ ./escribir disco "123456789" 1"
echo "#escribimos el texto “123456789” en los offsets 9000, 209000, 30725000, "
echo "#409605000 y 480000000, de inodos diferentes"
./escribir disco "123456789" 1
echo "################################################################################"
echo "$ ./leer disco 2 > ext2.txt"  
echo "#leemos el contenido del inodo 2 (escrito en el offset 9000) y lo direccionamos"
echo "#al fichero externo ext2.txt"
./leer disco 2 > ext2.txt
echo "################################################################################"
echo "$ ls -l ext2.txt"
echo "#comprobamos cuánto ocupa el fichero externo ext2.txt"
echo "#(ha de coincidir con el tamaño en bytes lógico del inodo 2 y con total_leidos)"
ls -l ext2.txt
echo "################################################################################"
echo "$ cat ext2.txt"
echo "#usamos el comando cat del sistema para leer el contenido del fichero externo"
cat ext2.txt
echo
echo "################################################################################"
echo "$ ./leer disco 2"
echo "#leemos el contenido de nuestro inodo 2"
echo "#(ha de contener lo mismo que el fichero externo ext2.txt)"
./leer disco 2
echo "################################################################################"
echo "$ ./leer disco 5 > ext3.txt"
echo "#leemos todo el contenido del inodo 5 (escrito en el offset 409605000) y lo "
echo "#direccionamos al fichero externo ext3.txt"
./leer disco 5 > ext3.txt
echo "################################################################################"
echo "$ ls -l ext3.txt"
echo "#comprobamos cuánto ocupa el fichero externo ext3.txt"
echo "#(ha de coincidir con el tamaño en bytes lógico del inodo 5 y con total_leidos)"
ls -l ext3.txt
echo "################################################################################"
echo "$ cat ext3.txt"
echo "#usamos el comando cat del sistema para leer el contenido del fichero externo"
cat ext3.txt
echo
echo "################################################################################"
echo "$ ./leer disco 5"
echo "#leemos el contenido de nuestro inodo 5"
echo "#(ha de contener lo mismo que el fichero externo ext3.txt)"
./leer disco 5
