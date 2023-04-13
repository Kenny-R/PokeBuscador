#ifndef UTILIDADES_LIB
    #define UTILIDADES_LIB

    #define TRUE (1 == 1)
    #define FALSE !TRUE
    #define PATH_MAX 4096
    #define STRING_MAX 100

    /*
    Funcion que imprime el manual de uso del programa y luego
    termina la ejecución del mismo. Se usa cuando se ha encontrado
    un error en los flags introducidos en la línea de comandos.
    */
    void printMan();

    /*
    Función que verifica si la cadena dada corresponde con
    un flag.
    Argumentos:
        arg: Cadena que será verificada.
    Retorna:
        Un entero, 1 si la cadena dada es un flag, 0 en caso
        contrario.
    */
    int verifF(char *arg);
    /*
    Función que lee un flag compuesto con más de un guión
    como prefijo y que su contenido es una cadena.
    Argumentos:
        mask: Apuntador a un entero, este entero corresponde
            a la máscara de bits relacionada con los flags
            que no tienen argumentos seguidos.
        flag: Cadena que corresponde con el flag.
    */
    void readFSA(int *mask, char *flag);

    /*
    Función que lee un flag compuesto de un solo guión como prefijo
    y que su contenido es un caracter.
    También verifica si a ese flag le corresponde un argumento seguido
    y si efectivamente, se ha introducido en consola.
    Argumentos:
        narg: Número de argumentos introducidos en consola.
        pos: Apuntador a la posición del flag en el arreglo de argumentos
        introducidos en consola.
        mask: Apuntador a un entero, este entero corresponde a la máscara
            de bits relacionada con los flags que tienen argumentos seguidos.
        maska: Apuntador a un entero, este entero corresponde a la máscara
            de bits relacionada con los argumentos correspondientes a los
            flags que tienen argumentos seguidos.
        masksa: Apuntador a un entero, este entero corresponde a la máscara
            de bits relacionada con los flags que tienen no argumentos seguidos.
        arg: Arreglo que contiene todos los argumentos introducidos en consola.
        datos: Struct que almacena los datos necesario para iniciar la búsqueda
            requerida del progama.
    */
    void readFA(int narg, int *pos, int *mask, int *maska, int *masksa, char *arg[], busqueda *datos);

    /*
    Función que lee los datos introducidos por consola, verifica
    que estén correctos y almacena lo necesario para iniciar la
    búsqueda requerida.
    Argumentos:
        narg: Entero, corresponde al número de argumentos introducidos
            por consola.
        arg: Arreglo que contiene los argumentos introducidos por
            consola.
        datos: Estructura en la que se almacenan todos los datos de la
            búsqueda.
        mask: Apuntador a un entero, este entero corresponde a la máscara
            de bits relacionada con los flags que tienen argumentos seguidos.
        maska: Apuntador a un entero, este entero corresponde a la máscara
            de bits relacionada con los argumentos correspondientes a los
            flags que tienen argumentos seguidos.
        masksa: Apuntador a un entero, este entero corresponde a la máscara
            de bits relacionada con los flags que tienen no argumentos seguidos.
    */
    void readD(int narg, char *arg[], busqueda *datos, int *mask, int *maska, int *masksa);

    /*
    Funcion que verifica si el bit no esta prendido o si hemos encontrado la carpeta
    o el archivo requerido.
    Argumentos:
        depth: Entero, corresponde al nivel del directorio en que se encuentra
            el directorio actual, por cada nivel se suma 4
            0: directorio raiz (se encuentran las carpetas de las regiones)
            4: donde estan las carpetas trainers/pokemon
            8: donde se encuentran las carpetas del tipo de apariciones
            12: finalmente, llegamos a una carpeta que contiene una lista de archivos html
        mask: Entero, corresponde a la máscara de bits relacionada con los
            flags que tienen argumentos seguidos
        mask2: Entero, corresponde a la máscara de bits relacionada con los flags no tienen
            argumentos seguidos.
        filename: String, contiene el nombre del archivo o la carpeta en que estamos
        buscar: String, contiene el nombre del archivo o la carpeta que se requiere
    Retorna:
        TRUE: si el bit no esta prendido o si hemos encontrado la carpeta/archivo que se requiere
        FALSE: en caso contrario
    */
    int search(int depth, int mask1, int mask2, char filename[], char buscar[]);

    /*
    Funcion que imprime en la consola los datos del archivo que corresponde segun una mascara de bit
    Argumentos:
        mask: Entero, representa una mascara de bit que corresponde a los flags --list, --nocount, --size
        name: String, contiene el nombre del archivo
        size: Entero, contiene el tamaño en Kilobytes del archivo
    */
    void printdata(int mask, char name[], int size);

#endif
