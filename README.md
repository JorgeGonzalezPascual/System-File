# Sistema de Ficheros - Sistemas Operativos II
- Autor: 
    * Jorge González Pascual

* **Sintaxis específica**
    * mi_mkfs.c: *mi_mkfs <nombre del fichero> <numero de bloques>*
    * escribir.c: *escribir <nombre_dispositivo> <\"$(cat fichero)\"> <diferentes_inodos>*
    * leer.c: *leer <nombre_dispositivo> <numero_inodo>*
    * permitir.c: *permitir <nombre_dispositivo> <ninodo> <permisos>*
    * leer_sf.c: *leer_sf <nombre_dispositivo>*
    * mi_mkdir.c: *./mi_mkdir <disco> <permisos> </ruta>*
    * mi_cat.c: *./mi_cat <disco> </ruta_fichero>*
    * mi_chmod.c: *./mi_mkdir <disco> <permisos> </ruta>*
    * mi_escribir_varios.c: *mi_escribir <nombre_dispositivo> </ruta_fichero> <texto> <offset>*
    * mi_link.c: *./mi_link disco /ruta_fichero_original /ruta_enlace*
    * mi_ls.c: *./mi_ls <disco></ruta_directorio>*
    * mi_rm.c: *./mi_rm <disco> </ruta_fichero>*
    * mi_stat.c: *./mi_mkdir <disco> <permisos> </ruta>*
    * mi_touch.c: */mi_touch <disco><permisos></ruta>*
    * simulacuon.c: *./simulacion <disco>*
    * verificacion.c: *./verificacion <nombre_dispositivo> <directorio_simulación>*
    * truncar.c: *./truncar <nombre_dispositivo> <ninodo> <nbytes>*

* **Modificaciones:**
    Hemos considerado que en la función _initMB()_ de _ficheros_basico.c_ a la hora de poner 1's los bits que corresponden a los metadatos, podríamos *reservar_bloques()* desde la posición _posSB_ hasta la posición del primer bloque de datos _(posPrimerBloqueDatos)_.

* **Mejoras:**
    * Para las funciones 'mi_write()' y 'mi_read()' de 'direcotrios.c' se ha usado un CACHE de directorios de manera FIFO.
    * Se ha implementado la función mi_dir con colores para una mejor visualización de mi_ls() y también parámetro adicional de tipo, basado en la sintaxis del camino (acabado en ‘/’ para directorios), tipo tendrá que contener: 'd' o 'f'
