make clean
make
echo "##############################################################"
echo "                          1ª parte"
echo "##############################################################"
rm disco
echo "$ ./mi_mkfs disco 100000"
./mi_mkfs disco 100000
echo "##############################################################"
echo "$ ./mi_mkdir disco 6 /dir1/"
./mi_mkdir disco 6 /dir1/
echo "$ ./mi_mkdir disco 6 /dir1/dir11/"
./mi_mkdir disco 6 /dir1/dir11/
echo "$ ./mi_touch disco 6 /dir1/dir11/fic1"
./mi_touch disco 6 /dir1/dir11/fic1
echo "$ ./mi_escribir disco /dir1/dir11/fic1 hellooooooo 0"
./mi_escribir disco /dir1/dir11/fic1 "hellooooooo" 0
echo "##############################################################"
echo "$ ./mi_mkdir disco 6 /dir2/"
./mi_mkdir disco 6 /dir2/
echo "$ ./mi_mkdir disco 6 /dir2/dir21/"
./mi_mkdir disco 6 /dir2/dir21/
echo "##############################################################"
echo "$ ./mi_link disco /dir1/dir11/fic1 /dir2/dir21/fic2"
./mi_link disco /dir1/dir11/fic1 /dir2/dir21/fic2
echo "##############################################################"
echo "$ ./mi_cat disco /dir2/dir21/fic2 #ha de mostrar mismo contenido que /dir1/dir11/fic1"
./mi_cat disco /dir2/dir21/fic2 #ha de mostrar mismo contenido que dir1/dir11/fic1
echo "$ ./mi_stat disco /dir1/dir11/fic1"
./mi_stat disco /dir1/dir11/fic1 
echo "$ ./mi_stat disco /dir2/dir21/fic2" #han de tener el mismo inodo y 2 enlaces
./mi_stat disco /dir2/dir21/fic2
echo "##############################################################"
echo "$ ./mi_link disco /dir1/dir11/fic3 /di2/dir21/fic4  #camino1 ha de existir"
./mi_link disco /dir1/dir11/fic3 /di2/dir21/fic4  #camino1 ha de existir 
echo "$ ./mi_touch disco 6 /dir1/dir11/fic3"
./mi_touch disco 6 /dir1/dir11/fic3 
echo "$ ./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic4"
./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic4
echo "$ ./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic5"
./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic5
echo "$ ./mi_stat disco /dir1/dir11/fic3"
./mi_stat disco /dir1/dir11/fic3
echo "##############################################################"
echo "$ ./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic2 #camino2 NO ha de existir"
./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic2 #camino2 NO ha de existir 
echo "##############################################################"
echo "                          2ª parte"
echo "##############################################################"
echo "$ ./mi_rm disco /dir2/dir21/ #o mi_rmdir"
./mi_rm disco /dir2/dir21/
echo "$ ./mi_rm disco /dir2/dir21/fic2"
./mi_rm disco /dir2/dir21/fic2 #Borrar enlace
echo "$ ./mi_stat disco /dir1/dir11/fic1 #Hemos borrado 1 enlace"
./mi_stat disco /dir1/dir11/fic1 #Hemos borrado 1 enlace
echo "$ ./mi_rm disco /dir2/dir21/fic2"
./mi_rm disco /dir2/dir21/fic2 #No se puede
echo "$ ./mi_rm disco /dir2/dir21/ #o mi_rmdir"
./mi_rm disco /dir2/dir21/ #No se puede
echo "$ ./mi_ls disco /dir2/dir21/"
./mi_ls disco /dir2/dir21/
echo "$ ./mi_rm disco /dir2/dir21/fic4"
./mi_rm disco /dir2/dir21/fic4
echo "$ ./mi_rm disco /dir2/dir21/fic5"
./mi_rm disco /dir2/dir21/fic5
echo "$ ./mi_rm disco /dir2/dir21/ #o mi_rmdir"
./mi_rm disco /dir2/dir21/
echo "$ ./mi_ls disco /dir2/"
./mi_ls disco /dir2/
echo "##############################################################"

echo "##############################################################"
echo "Comprobamos que al crear 17 subdirectorios los bloques de datos del padre son 2"
echo "(en un bloque caben 16 entradas de directorio),"
echo " y que al eliminar un subdirectorio el directorio padre tiene 1 bloque de datos"

echo "##############################################################"
echo "$ ./mi_mkdir disco /d1/"
./mi_mkdir disco 6 /d1/
echo "##############################################################"
echo "creamos 17 subdirectorios sd0, sd1..., sd16 en d1"
echo "$ for i in \$(seq 0 16)"
echo "> do"
echo "> ./mi_mkdir disco 6 /d1/sd\$i/"
echo "> done"
for i in $(seq 0 16)
do
    ./mi_mkdir disco 6 /d1/sd$i/
done
echo "##############################################################"
echo "Mostramos la metainformacion del directorio para ver que tiene 2 bloques de datos"
echo "$ ./mi_stat disco /d1/"
./mi_stat disco /d1/
echo "##############################################################"
echo "Listamos el directorio para ver sus subdirectorios"
echo "$ ./mi_ls disco /d1/"
./mi_ls disco /d1/
echo "##############################################################"
echo "Eliminamos el subdirectorio sd3 de d1"
echo "$ ./mi_rm disco /d1/sd3/"
./mi_rm disco /d1/sd3/
echo "##############################################################"
echo "Mostramos la metainformacion de d1 para ver que ahora tiene 1 bloque de datos"
echo "$ ./mi_stat disco /d1/"
./mi_stat disco /d1/
echo "##############################################################"
echo "Volvemos a listar el directorio para ver que se ha eliminado un subdirectorio"
echo "$ ./mi_ls disco /d1/"
./mi_ls disco /d1/
echo "Macke clean"
make clean
