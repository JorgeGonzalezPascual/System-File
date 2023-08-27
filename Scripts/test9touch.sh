make clean
make
#SCRIPT NIVEL 9
rm disco
echo "######################################################################"
echo "$ ./mi_mkfs disco 100000"
./mi_mkfs disco 100000
echo "######################################################################"
echo "$ ./mi_touch disco 6 /fichero"
./mi_touch disco 6 /fichero
echo "######################################################################"
echo "$ ./mi_ls disco /"
./mi_ls disco /
echo "######################################################################"
echo "$ ./mi_escribir  disco /fichero "hola que tal" 5120"
./mi_escribir  disco /fichero "hola que tal" 5120
echo "######################################################################"
echo "$ ./mi_ls disco /"
./mi_ls disco /
echo "######################################################################"
echo "$ ./leer_sf disco"
./leer_sf disco
echo "######################################################################"
echo "$ ./mi_chmod disco 4 /fichero"
./mi_chmod disco 4 /fichero
echo "######################################################################"
echo "$ ./mi_escribir  disco /fichero "estoy estupendamente" 256000"
./mi_escribir  disco /fichero "estoy estupendamente" 256000
echo "######################################################################"
echo "$ ./mi_ls disco /fichero"
./mi_ls disco /fichero

echo "######################################################################"
echo "$ ./mi_mkdir disco 6 /dir1/"
 ./mi_mkdir disco 6 /dir1/
echo "$ ./mi_touch disco 6 /dir1/fic1"
./mi_touch disco 6 /dir1/fic1
echo "######################################################################"
echo "./mi_escribir disco /dir1/fic1 hola1  256000"
./mi_escribir disco /dir1/fic1 hola1  256000
echo "######################################################################"
echo "$ ./mi_stat disco /dir1/fic1"
./mi_stat disco /dir1/fic1
echo "######################################################################"
echo "$ sleep 2 #esperamos un poco para observar los sellos de tiempo" 
sleep 2 
echo "######################################################################"
echo "$ ./mi_escribir disco /dir1/fic1 hola2  5120 #no cambia tamenBytesLog pero sí mtime y ctime (ocupamos 1 bloque más)"
./mi_escribir disco /dir1/fic1 hola2  5120 
echo "######################################################################"
echo "$ ./mi_stat disco /dir1/fic1"
./mi_stat disco /dir1/fic1
echo "######################################################################"
echo "$ sleep 2 #esperamos un poco para observar los sellos de tiempo" 
sleep 2 
echo "######################################################################"
echo "$ ./mi_escribir disco /dir1/fic1 hola3  5200 #mismo bloque que offset 5120, cambia mtime pero no ctime"
./mi_escribir disco /dir1/fic1 hola3  5200 
echo "######################################################################"
echo "$ ./mi_stat disco /dir1/fic1"
./mi_stat disco /dir1/fic1
echo "######################################################################"
echo "sleep 2 #esperamos un poco para observar los sellos de tiempo"
sleep 2
echo "######################################################################"
echo "./mi_escribir disco /dir1/fic1 hola4 256010 #cambia tamEnBytesLog, mtime y ctime"
./mi_escribir disco /dir1/fic1 hola4 256010
echo "######################################################################"
echo "$ ./mi_stat disco /dir1/fic1"
./mi_stat disco /dir1/fic1
echo
echo "# Comprobación de la caché de directorios"
echo "######################################################################"
echo "$ ./mi_touch disco 6 /dir1/fic2"
./mi_touch disco 6 /dir1/fic2
echo "$ ./mi_escribir disco /dir1/fic2 "$(cat texto2.txt)" 1000"
./mi_escribir disco /dir1/fic2 "$(cat texto2.txt)" 1000
echo "######################################################################"
echo "$ ./mi_cat disco /dir1/fic2 #tambuffer=BLOCKSIZE * 4"
./mi_cat disco /dir1/fic2 
echo "######################################################################"
echo "$ ./mi_escribir disco /dir1/fic2 \"******************************\" 10000"
./mi_escribir disco /dir1/fic2 "******************************" 10000
echo "######################################################################"
echo "$ ./mi_ls disco /dir1/"
./mi_ls disco /dir1/
echo "######################################################################"
echo "$ ./mi_cat disco /dir1/fic2 #tambuffer=BLOCKSIZE * 4"
 ./mi_cat disco /dir1/fic2 

echo "######################################################################"
echo "$ ./mi_touch disco 6 /dir1/fic3"
./mi_touch disco 6 /dir1/fic3
echo "$ ./mi_escribir_varios disco /dir1/fic3 \"--texto repetido en 10 bloques--\" 0"
./mi_escribir_varios disco /dir1/fic3 "--texto repetido en 10 bloques--" 0
echo "$ ./mi_stat disco /dir1/fic3"
./mi_stat disco /dir1/fic3
echo "######################################################################"
echo "$ ./mi_cat disco /dir1/fic3 #tambuffer=BLOCKSIZE * 4"
./mi_cat disco /dir1/fic3 
make clean
echo "make clean"
echo "################################################################################"
