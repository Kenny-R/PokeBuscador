#include "utilidades.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 Funcion que imprime el manual de uso del programa y luego
 termina la ejecución del mismo. Se usa cuando se ha encontrado
 un error en los flags introducidos en la línea de comandos.
 */
void printMan()
{
    printf("Console input error.\n");
    printf("Allowed commands:\n");
    printf("[-r <region>]\n[-s <species>]\n");
    printf("[-t <type>]\n[-c|--nocount]\n");
    printf("[-l|--list]\n[--size]\n");
    printf("[name]\n");
    printf("Remember not to repeat flags and not to omit required arguments.\n");
    exit(0);
}

/*
 Función que verifica si la cadena dada corresponde con
 un flag.
 Argumentos:
    arg: Cadena que será verificada.
 Retorna:
    Un entero, 1 si la cadena dada es un flag, 0 en caso
    contrario.
 */
int verifF(char *arg)
{
    if (arg[0] == '-')
        return (1);
    return (0);
}

/*
 Función que lee un flag compuesto con más de un guión
 como prefijo y que su contenido es una cadena.
 Argumentos:
    mask: Apuntador a un entero, este entero corresponde
        a la máscara de bits relacionada con los flags
        que no tienen argumentos seguidos.
    flag: Cadena que corresponde con el flag.
 */
void readFSA(int *mask, char *flag)
{
    if (strcmp(flag, "--list") == 0 || strcmp(flag, "--nocount") || strcmp(flag, "--size"))
    {
        switch (flag[2])
        {
        case 'l':
            if (*mask & (1 << 0))
                printMan();
            *mask |= (1 << 0);
            break;
        case 's':
            if (*mask & (1 << 2))
                printMan();
            *mask |= (1 << 2);
            break;
        default:
            if (*mask & (1 << 1))
                printMan();
            *mask |= (1 << 1);
            break;
        }
    }
    else
        printMan();
}

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
void readFA(int narg, int *pos, int *mask, int *maska, int *masksa, char *arg[], busqueda *datos)
{
    if (arg[*pos][2] != '\0')
        printMan();

    switch (arg[*pos][1])
    {
    case 'r':
        if (*mask & (1 << 0))
            printMan();
        *mask |= (1 << 0);
        if (*pos + 1 < narg)
            if (!verifF(arg[*pos + 1]))
            {
                *maska |= (1 << 0);
                datos->region = arg[*pos + 1];
            }
        *pos += 2;
        break;
    case 's':
        if (*mask & (1 << 1))
            printMan();
        *mask |= (1 << 1);
        if (*pos + 1 < narg)
            if (!verifF(arg[*pos + 1]))
            {
                *maska |= (1 << 1);
                datos->especie = arg[*pos + 1];
            }
        *pos += 2;
        break;
    case 't':
        if (*mask & (1 << 2))
            printMan();
        *mask |= (1 << 2);
        if (*pos + 1 < narg)
            if (!verifF(arg[*pos + 1]))
            {
                *maska |= (1 << 2);
                datos->tipo = arg[*pos + 1];
            }
        *pos += 2;
        break;
    case 'l':
        if (*masksa & (1 << 0))
            printMan();
        *masksa |= (1 << 0);
        *pos += 1;
        break;
    case 'c':
        if (*masksa & (1 << 1))
            printMan();
        *masksa |= (1 << 1);
        *pos += 1;
        break;
    default:
        printMan();
        break;
    }
}

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
void readD(int narg, char *arg[], busqueda *datos, int *mask, int *maska, int *masksa)
{
    int i = 1;
    while (i < narg)
    {
        if (verifF(arg[i]))
        {
            if (arg[i][1] == '-')
            {
                readFSA(masksa, arg[i]);
                i++;
            }
            else
                readFA(narg, &i, mask, maska, masksa, arg, datos);
        }
        else
        {
            *masksa |= (1 << 3);
            datos->nombre = arg[i];
            i++;
        }
    }

    if (*mask != *maska)
        printMan();
}

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
int search(int depth, int mask1, int mask2, char filename[], char buscar[])
{
    if (depth <= 8)
    {
        if (!(mask1 & (1 << (depth / 4))) || strcmp(filename, buscar) == 0)
            return TRUE;
    }
    else
    {
        if (!(mask2 & (1 << 3)))
            return TRUE;
        /* si estamos en el ultimo nivel del directorio y el bit esta prendido */
        /* copiamos una parte del nombre del archivo y comparamos con el string buscar */
        char *tmp = calloc((int)strlen(buscar), sizeof(char));
        if (tmp == NULL)
        {
            printf("Memory not allocated\n");
            exit(1);
        }
        strncpy(tmp, filename, strlen(buscar));
        if (strcmp(tmp, buscar) == 0)
        {
            free(tmp);
            return TRUE;
        }
        free(tmp);
    }
    return FALSE;
}

/*
 Funcion que imprime en la consola los datos del archivo que corresponde segun una mascara de bit
 Argumentos:
    mask: Entero, representa una mascara de bit que corresponde a los flags --list, --nocount, --size
    name: String, contiene el nombre del archivo
    size: Entero, contiene el tamaño en Kilobytes del archivo
*/
void printdata(int mask, char name[], int size)
{
    /* si el primer y tercer flag estan prendidos, significa que estan presentes los flags -l o --list y --size */
    if ((mask & (1 << 0)) && (mask & (1 << 2)))
    {
        /* se imprime el nombre del archivo encontrado y su tamaño en kilobytes*/
        printf("%s %d KB\n", name, size);
    }
    else if (mask & (1 << 0))
    {
        /* si solo esta presente el flag -l o --list, simplemente imprimimos el nombre del archivo encontrado */
        printf("%s\n", name);
    }
}
