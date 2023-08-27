echo "################################################################################"
echo "$ rm disco"
rm disco
echo "$ ./mi_mkfs disco 100000"
./mi_mkfs disco 100000
echo "################################################################################"
echo "$ ./escribir disco "$(cat texto2.txt)" 1"
echo "#escribimos el texto contenido en text2.txt en los offsets  9000, 209000, 30725000, "
echo "#409605000 y 480000000 de inodos diferentes"
./escribir disco "$(cat texto2.txt)" 1
echo "################################################################################"
echo "$ ./leer disco 2 > ext4.txt"
echo "#leemos el contenido del inodo 2 (escrito en el offset 209000) y lo direccionamos"
echo "#al fichero externo ext4.txt"
./leer disco 2 > ext4.txt
echo "################################################################################"
echo "$ ls -l ext4.txt"
echo "#comprobamos cuánto ocupa el fichero externo ext4.txt"
echo "#(ha de coincidir con el tamaño en bytes lógico del inodo 2 y con total_leidos)"
ls -l ext4.txt
echo "################################################################################"
echo "$ cat ext4.txt"
echo "#usamos el cat del sistema para leer el contenido de nuestro fichero direccionado"
echo "#No hay que mostrar basura"
cat ext4.txt
echo "################################################################################"
echo "$ ./permitir"
echo "#mostramos sintaxis de permitir"
./permitir
echo "################################################################################"
echo "$ ./permitir disco 2 0"
echo "#cambiamos permisos del inodo 2 a 0"
./permitir disco 2 0
echo "################################################################################"
echo "$ ./leer disco 2"
echo "#intentamos leer inodo 2 con permisos=0"
./leer disco 2