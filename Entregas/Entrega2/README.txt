# Sistema de Ficheros
- Autores: 
    * Jorge González Pascual
    * Luis Clar Fiol

* **Sintaxis específica**
    * mi_fks.c : *mi_fks <nombre del fichero> <numero de bloques>*
    * escribir.c: *escribir <nombre_dispositivo> <\"$(cat fichero)\"> <diferentes_inodos>*
    * leer.c: *leer <nombre_dispositivo> <numero_inodo>*
    * permitir.c: *permitir <nombre_dispositivo> <ninodo> <permisos>*
    * leer_sf.c: *leer_sf <nombre_dispositivo>*

* **Modificaciones:**
    Hemos considerado que en la función _initMB()_ de _ficheros_basico.c_ a la hora de poner 1's los bits que corresponden a los metadatos, podríamos *reservar_bloques()* desde la posición _posSB_ hasta la posición del primer bloque de datos _(posPrimerBloqueDatos)_.

* **Mejoras:**
    * mi_touch()
    * Para las funciones 'mi_write()' y 'mi_read()' de 'direcotrios.c' se ha usado un CACHE de directorios de manera FIFO.
    * Se ha implementado la función mi_dir con colores para una mejor visualización de mi_ls(). Y esta en proceso utilizar un parámetro adicional de tipo, basado en la sintaxis del camino (acabado en ‘/’ para directorios) y 