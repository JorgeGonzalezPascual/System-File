echo "Test nivel 8"
echo "################################################################################"
echo "$ ./rm disco"
./rm disco
echo "################################################################################"
echo "$ ./mi_mkfs disco 100000"
./mi_mkfs disco 100000
echo "################################################################################"
echo "$ ./mi_mkdir #comprobar sintaxis"
./mi_mkdir 
echo "################################################################################"
echo "$ ./mi_mkdir disco 7 / #no ha de dejar crear la raíz al usuario"
./mi_mkdir disco 7 /
echo "################################################################################"
echo "$ ./mi_mkdir disco 6 dir1/"
./mi_mkdir disco 6 dir1/
echo "################################################################################"
echo "$ ./mi_mkdir disco 6 /dir1/   #permiso lectura/escritura"
./mi_mkdir disco 6 /dir1/   
echo "################################################################################"
echo "$ ./mi_mkdir disco 6 /dir1/dir11/   #permiso lectura/escritura"
./mi_mkdir disco 6 /dir1/dir11/  
echo "################################################################################"
echo "$ ./mi_chmod #comprobar sintaxis"
./mi_chmod 
echo "################################################################################"
echo "$ ./mi_chmod disco 1 /dir1/dir11/   #permiso ejecución"
./mi_chmod disco 1 /dir1/dir11/  
echo "################################################################################"
echo "$ ./mi_mkdir disco 6 /dir1/dir11/fic111   #Error: Permiso denegado de lectura."
./mi_mkdir disco 6 /dir1/dir11/fic111  
echo "################################################################################"
echo "./mi_chmod disco 2 /dir1/dir11/  #permiso escritura"
./mi_chmod disco 2 /dir1/dir11/  
echo "################################################################################"
echo "$ ./mi_mkdir disco 6 /dir1/dir11/fic111   #Error: Permiso denegado de lectura."
./mi_mkdir disco 6 /dir1/dir11/fic111  
echo "################################################################################"
echo "$ ./mi_chmod disco 6 /dir1/dir11/   #permiso lectura/escritura"
./mi_chmod disco 6 /dir1/dir11/   
echo "################################################################################"
echo "$ ./mi_mkdir disco 6 /dir1/dir11/fic111   #permiso lectura/escritura"
./mi_mkdir disco 6 /dir1/dir11/fic111  
echo "################################################################################"
echo "$ ./mi_mkdir disco 6 /dir1/dir11/fic112   #permiso lectura/escritura"
./mi_mkdir disco 6 /dir1/dir11/fic112  
echo "################################################################################"
echo "$ ./mi_ls disco /"
./mi_ls disco /
echo "################################################################################"
echo "$ ./mi_stat disco /dir1/"
./mi_stat disco /dir1/
echo "################################################################################"
echo "$ ./mi_ls disco /dir1/ "
./mi_ls disco /dir1/ 
echo "################################################################################"
echo "$ ./mi_stat disco /dir1/dir11/"
./mi_stat disco /dir1/dir11/
echo "################################################################################"
echo "$ ./mi_ls disco /dir1/dir11/"
./mi_ls disco /dir1/dir11/
echo "################################################################################"
echo "./mi_ls disco /dir1/dir12/ #Error: No existe el archivo o el directorio."
./mi_ls disco /dir1/dir12/
echo "################################################################################"
echo "$ ./mi_mkdir disco 6 /dir1/dir11/fic111 #Error: El archivo ya existe."
./mi_mkdir disco 6 /dir1/dir11/fic111  
echo "################################################################################"
echo "$ ./mi_mkdir disco 6 /dir1/dir11/fic111/dir12/ #Error: No es un directorio."
./mi_mkdir disco 6 /dir1/dir11/fic111/dir12/
echo "################################################################################"
echo "$ ./mi_mkdir disco 6 /dir1/dir11/dir12/fic111 #Error: No existe algún directorio intermedio."
./mi_mkdir disco 6 /dir1/dir11/dir12/fic111
echo "################################################################################"
echo "$ ./mi_mkdir disco 9 /dir2/ #Error: modo inválido: <<9>>"
./mi_mkdir disco 9 /dir2/
echo "################################################################################"