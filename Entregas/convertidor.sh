#!/bin/bash
#Script para comprimir los archivos y dejarlos listos para entregar
echo $'\n'"-- CONVERTIDOR .tar.gz --" $'\n'
echo "Escribe el nombre de la capeta a comprimir:"
read dir
echo $'\n'"Escribe el nombre del archivo de salida:"
read nombre
cp -r $dir $nombre                  #Copia una carpeta con el nombre deseado
tar -czvf $nombre.tar.gz $nombre    #Comprimimos
rm -rf $nombre                      #Eliminamos la carpeta creada
echo $'\n'"Conversión realizada!"$'\n'