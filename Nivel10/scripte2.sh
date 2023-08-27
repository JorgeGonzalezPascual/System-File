make clean
make
echo "$ rm disco"
rm disco
echo "$ ./mi_mkfs disco 100000"
./mi_mkfs disco 100000
echo "######################################################################"
echo "$ ./leer_sf disco"
./leer_sf disco
echo
echo "######################################################################"
echo "NIVEL 8: CREAR DIRECTORIOS Y FICHEROS, PERMISOS, STAT, LISTAR"
echo "######################################################################"
echo "$ ./mi_mkdir #comprobar sintaxis"
./mi_mkdir 
echo "######################################################################"
echo "$ ./mi_mkdir disco 7 / #no ha de dejar crear la raíz al usuario"
./mi_mkdir disco 7 /
echo "######################################################################"
echo "$ ./mi_mkdir disco 6 dir1/"
./mi_mkdir disco 6 dir1/
echo "######################################################################"
echo "$ ./mi_mkdir disco 6 /dir1/"  
./mi_mkdir disco 6 /dir1/   
echo "$ ./mi_mkdir disco 6 /dir1/dir11/"
./mi_mkdir disco 6 /dir1/dir11/  
echo "######################################################################"
echo "$ ./mi_chmod #comprobar sintaxis"
./mi_chmod 
echo "######################################################################"
echo "$ ./mi_chmod disco 1 /dir1/dir11/ #cambio a permiso ejecución"
./mi_chmod disco 1 /dir1/dir11/  
echo "$ ./mi_touch disco 6 /dir1/dir11/fic111 #Error: Permiso denegado de lectura."
./mi_touch disco 6 /dir1/dir11/fic111  
echo "######################################################################"
echo "$ ./mi_chmod disco 2 /dir1/dir11/ #cambio a permiso escritura"
./mi_chmod disco 2 /dir1/dir11/  
echo "$ ./mi_touch disco 6 /dir1/dir11/fic111 #Error: Permiso denegado de lectura."
./mi_touch disco 6 /dir1/dir11/fic111  
echo "######################################################################"
echo "$ ./mi_chmod disco 6 /dir1/dir11/ #cambio a permiso lectura/escritura"
./mi_chmod disco 6 /dir1/dir11/   
echo "$ ./mi_touch disco 6 /dir1/dir11/fic111"
./mi_touch disco 6 /dir1/dir11/fic111  
echo "$ ./mi_touch disco 6 /dir1/dir11/fic112"
./mi_touch disco 6 /dir1/dir11/fic112  
echo "######################################################################"
echo
echo
echo "                  /"
echo "                  |"
echo "                dir1"
echo "                  |"
echo "                dir11"
echo "               /     \\"
echo "           fic111   fic112"
echo
echo "######################################################################"
echo "$ ./mi_ls disco /"
./mi_ls disco /
echo "$ ./mi_stat disco /dir1/"
./mi_stat disco /dir1/
echo "$ ./mi_ls disco /dir1/"
./mi_ls disco /dir1/ 
echo "$ ./mi_stat disco /dir1/dir11/"
./mi_stat disco /dir1/dir11/
echo "$ ./mi_ls disco /dir1/dir11/"
./mi_ls disco /dir1/dir11/
echo "$ ./mi_ls disco /dir1/dir12/ #Error: No existe el archivo o el directorio."
./mi_ls disco /dir1/dir12/
echo "######################################################################"
echo "$ ./mi_touch disco 6 /dir1/dir11/fic111 #Error: El archivo ya existe."
./mi_touch disco 6 /dir1/dir11/fic111  
echo "$ ./mi_mkdir disco 6 /dir1/dir11/fic111/dir12/ #Error: No es un directorio."
./mi_mkdir disco 6 /dir1/dir11/fic111/dir12/
echo "$ ./mi_touch disco 6 /dir1/dir11/dir12/fic111 #Error: No existe algún directorio intermedio."
./mi_touch disco 6 /dir1/dir11/dir12/fic111
echo "$ ./mi_mkdir disco 9 /dir2/ #Error: modo inválido: <<9>>"
./mi_mkdir disco 9 /dir2/
echo

echo "######################################################################"
echo "NIVEL 9: LEER Y ESCRIBIR"
echo "######################################################################"
echo "$ ./leer_sf disco"
./leer_sf disco
echo "######################################################################"
echo "$ ./mi_escribir  disco /dir1/dir11/fic111 "$(cat texto2.txt)" 0 #⊂ BL 0 ∈ D0"
echo "#reservaremos 5 bloques de datos (3 escritos completos y 2 parciales)"
./mi_escribir  disco /dir1/dir11/fic111 "$(cat texto2.txt)" 0
echo "$ ./leer_sf disco"
./leer_sf disco
echo "$ ./mi_cat  disco /dir1/dir11/fic111>ext1.txt"
./mi_cat  disco /dir1/dir11/fic111>ext1.txt
echo "$ ls -l ext1.txt #comprobamos el tamaño del fichero externo" 
ls -l ext1.txt
echo "######################################################################"
echo "$ ./mi_chmod disco 4 /dir1/dir11/fic111 #cambio a permiso lectura"
./mi_chmod disco 4 /dir1/dir11/fic111
echo "$ ./mi_escribir  disco /dir1/dir11/fic111 "lo que sea" 209000 #⊂ BL 204 ∈ I0"
./mi_escribir  disco /dir1/dir11/fic111 "lo que sea" 209000
echo "######################################################################"
echo "$ ./mi_ls disco /dir1/dir11/"
./mi_ls disco /dir1/dir11/
echo "######################################################################"
echo "$ ./mi_escribir disco /dir1/dir11/fic112 "hola1" 209000 #⊂ BL 204 ∈ I0"
echo "#modifica tamEnBytesLog, mtime y ctime, y reservamos 1 bloque datos y 1 de punteros"
./mi_escribir disco /dir1/dir11/fic112 "hola1"  209000
echo "######################################################################"
echo "$ ./mi_stat disco /dir1/dir11/fic112"
./mi_stat disco /dir1/dir11/fic112
echo "######################################################################"
echo "$ sleep 2 #esperamos para distanciar los sellos de tiempo"
sleep 2 
echo "######################################################################"
echo "$ ./mi_escribir disco /dir1/dir11/fic112 "hola2" 9000 #⊂ BL 8 ∈ D8"
echo "#no cambia tamenBytesLog pero sí mtime y ctime (ocupamos 1 bloque más de datos)"
./mi_escribir disco /dir1/dir11/fic112 "hola2" 9000 
echo "######################################################################"
echo "$ ./mi_stat disco /dir1/dir11/fic112"
./mi_stat disco /dir1/dir11/fic112
echo "######################################################################"
echo "$ sleep 2 #esperamos para distanciar los sellos de tiempo"
sleep 2 
echo "######################################################################"
echo "$ ./mi_escribir disco /dir1/dir11/fic112 "hola3" 9100 #⊂ BL 8 ∈ D8"
echo "#mismo bloque que offset 9000, cambia mtime pero no ctime"
./mi_escribir disco /dir1/dir11/fic112 "hola3"  9100
echo "######################################################################"
echo "$ ./mi_stat disco /dir1/dir11/fic112"
./mi_stat disco /dir1/dir11/fic112
echo "######################################################################"
echo "$ sleep 2 #esperamos para distanciar los sellos de tiempo"
sleep 2
echo "######################################################################"
echo "$ ./mi_escribir disco /dir1/dir11/fic112 "hola4" 275000 #⊂ BL 268 ∈ I1" 
echo " #cambia tamEnBytesLog, mtime y ctime, y reservamos 1 bloque datos y 2 de punteros"
./mi_escribir disco /dir1/dir11/fic112 "hola4" 275000
echo "######################################################################"
echo "$ ./mi_stat /dir1/dir11/fic112 "
./mi_stat disco /dir1/dir11/fic112 
echo "$ ./mi_cat /dir1/dir11/fic112 "
./mi_cat disco /dir1/dir11/fic112 
echo

echo "######################################################################"
echo "NIVEL 10: ENLACES Y BORRADO"
echo "######################################################################"
echo
echo "                  /"
echo "                  |"
echo "                dir1"
echo "                  |"
echo "                dir11"
echo "               /     \\"
echo "           fic111   fic112"
echo
echo "######################################################################"
echo "$ ./mi_ls disco /dir1/dir11/"
./mi_ls disco /dir1/dir11/
echo "$ ./mi_touch disco 6 /dir1/dir11/fic113"
./mi_touch disco 6 /dir1/dir11/fic113
echo "$ ./mi_escribir disco /dir1/dir11/fic113 hellooooooo 409605000 #⊂ BL 400.004 ∈ I2"
./mi_escribir disco /dir1/dir11/fic113 "hellooooooo" 409605000
echo "######################################################################"
echo "$ ./mi_mkdir disco 6 /dir2/"
./mi_mkdir disco 6 /dir2/
echo "$ ./mi_mkdir disco 6 /dir2/dir21/"
./mi_mkdir disco 6 /dir2/dir21/
echo "######################################################################"
echo "$ ./mi_link disco /dir1/dir11/fic113 /dir2/dir21/fic211"
./mi_link disco /dir1/dir11/fic113 /dir2/dir21/fic211
echo "######################################################################"
echo
echo "                                 /"
echo "                      ___________|__________" 
echo "                     /                      \\"
echo "                   dir1                   dir2"
echo "                     |                      |"
echo "                   dir11                  dir21"
echo "                  /  |  \                   |"
echo "                 /   |   \                  |"
echo "           fic111 fic112 fic113 <-------- fic211"
echo
echo "######################################################################"
echo "$ ./mi_cat disco /dir2/dir21/fic211" 
echo "#ha de mostrar mismo contenido que /dir1/dir11/fic113"
./mi_cat disco /dir2/dir21/fic211 #ha de mostrar mismo contenido que dir1/dir11/fic113
echo "$ ./mi_stat disco /dir1/dir11/fic113"
./mi_stat disco /dir1/dir11/fic113 
echo "$ ./mi_stat disco /dir2/dir21/fic211" #han de tener el mismo inodo y 2 enlaces
./mi_stat disco /dir2/dir21/fic211
echo "######################################################################"
echo "$ ./mi_link disco /dir1/dir11/fic114 /di2/dir21/fic212" 
echo "#camino1 ha de existir"
./mi_link disco /dir1/dir11/fic14 /di2/dir21/fic212  #camino1 ha de existir 
echo "######################################################################"
echo "$ ./mi_link disco /dir1/dir11/fic113 /dir2/dir21/fic211"
echo "#camino2 NO ha de existir"
./mi_link disco /dir1/dir11/fic113 /dir2/dir21/fic211 #camino2 NO ha de existir 
echo "######################################################################"
echo "$ ./mi_rm disco /dir2/dir21/ #o mi_rmdir"
echo "#Error: El directorio /dir2/dir21/ no está vacío"
./mi_rm disco /dir2/dir21/ 
echo "$ ./mi_rm disco /dir2/dir21/fic211"
./mi_rm disco /dir2/dir21/fic211
echo "$ ./mi_stat disco /dir1/dir11/fic113 #Hemos borrado 1 enlace"
./mi_stat disco /dir1/dir11/fic113 #Hemos borrado 1 enlace
echo "$ ./leer_sf disco"
./leer_sf disco
echo "$ ./mi_rm disco /dir1/dir11/fic113 #Ahora sí se liberará el inodo y sus bloques"
./mi_rm disco /dir1/dir11/fic113
echo "$ ./leer_sf disco"
./leer_sf disco
echo "$ ./mi_rm disco /dir2/dir21/fic211 #ya no existe"
./mi_rm disco /dir2/dir21/fic211
echo "$ ./mi_rm disco /dir2/dir21/" #o mi_rmdir
./mi_rm disco /dir2/dir21/
echo "$ ./mi_ls disco /dir2/"
./mi_ls disco /dir2/

echo "######################################################################"
echo
echo "                                 /"
echo "                      ___________|_________" 
echo "                     /                     \\"
echo "                   dir1                   dir2"
echo "                     |"
echo "                   dir11"
echo "                  /     \\"
echo "              fic111  fic112"
echo
echo "######################################################################"
echo "Creamos más directorios de los que caben en 1 bloque"
echo "######################################################################"
echo "$ ./mi_mkdir disco /dir3/"
./mi_mkdir disco 6 /dir3/
echo "creamos 17 subdirectorios sd0, sd1..., sd16 en /dir3/"
echo "$ for i in \$(seq 0 16)"
echo "> do"
echo "> ./mi_mkdir disco 6 /dir3/sd\$i/"
echo "> done"
for i in $(seq 0 16)
do
    ./mi_mkdir disco 6 /dir3/sd$i/
done
echo "######################################################################"
echo
echo "             /"
echo "        _____|_____" 
echo "       /     |     \\"
echo "     dir1  dir2    dir3"
echo "      |        ______|_____________________________________________________________"
echo "      |       /  |   |   |   |   |   |   |   |   |   |    |    |    |    |    |    \\"
echo "    dir11   sd0 sd1 sd2 sd3 sd4 sd5 sd6 sd7 sd8 sd9 sd10 sd11 sd12 sd13 sd14 sd15 sd16"
echo "   /     \\"
echo "fic111  fic112"
echo
echo "######################################################################"
echo "Mostramos la metainformacion del directorio para ver que tiene 2 bloques de datos"
echo "$ ./mi_stat disco /dir3/"
./mi_stat disco /dir3/
echo "######################################################################"
echo "Listamos el directorio para ver sus subdirectorios"
echo "$ ./mi_ls disco /dir3/"
./mi_ls disco /dir3/
echo "######################################################################"
echo "Eliminamos el subdirectorio sd3 de dir3"
echo "$ ./mi_rm disco /dir3/sd3/"
./mi_rm disco /dir3/sd3/
echo "######################################################################"
echo
echo "             /"
echo "        _____|_____" 
echo "       /     |     \\"
echo "     dir1  dir2    dir3"
echo "      |         ____|__________________________________________________________"
echo "      |        /  |   |   |   |   |   |   |   |   |   |    |    |    |    |    \\"
echo "    dir11   sd0 sd1 sd2 sd16 sd4 sd5 sd6 sd7 sd8 sd9 sd10 sd11 sd12 sd13 sd14 sd15"
echo "   /     \\"
echo "fic111  fic112"
echo "######################################################################"
echo "Mostramos la metainformacion de dir3 para ver que ahora tiene 1 bloque de datos"
echo "$ ./mi_stat disco /dir3/"
./mi_stat disco /dir3/
echo "######################################################################"
echo "Volvemos a listar el directorio para ver que se ha eliminado un subdirectorio"
echo "$ ./mi_ls disco /dir3/"
./mi_ls disco /dir3/

make clean