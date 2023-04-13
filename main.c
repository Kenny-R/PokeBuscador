#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

#define PATH_MAX 4096

typedef struct
{
    char *region;
    char *especie;
    char *tipo;
    char *nombre;
} busqueda;

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
                if (*mask & (1<<0)) printMan();
                *mask |= (1<<0);
                break;
            case 's':
                if (*mask & (1<<2)) printMan();
                *mask |= (1<<2);
                break;
            default:
                if (*mask & (1<<1)) printMan();
                *mask |= (1<<1);
                break;
        }
    }
    else printMan();
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
    if (arg[*pos][2] != '\0') printMan();
    
    switch (arg[*pos][1]) 
    {
        case 'r':
            if (*mask & (1<<0)) printMan();
            *mask |= (1<<0);
            if (*pos+1 < narg) 
                if (!verifF(arg[*pos+1])) 
                {
                    *maska |= (1<<0);
                    datos->region = arg[*pos+1];
                }
            *pos += 2;
            break;
        case 's':
            if (*mask & (1<<1)) printMan();
            *mask |= (1<<1);
            if (*pos+1 < narg) 
                if (!verifF(arg[*pos+1])) 
                {
                    *maska |= (1<<1);
                    datos->especie = arg[*pos+1];
                }
            *pos += 2;
            break;
        case 't':
            if (*mask & (1<<2)) printMan();
            *mask |= (1<<2);
            if (*pos+1 < narg) 
                if (!verifF(arg[*pos+1])) 
                {
                    *maska |= (1<<2);
                    datos->tipo = arg[*pos+1];
                }
            *pos += 2;
            break;
        case 'l':
            if (*masksa & (1<<0)) printMan();
            *masksa |= (1<<0);
            *pos += 1;
            break;
        case 'c':
            if (*masksa & (1<<1)) printMan();
            *masksa |= (1<<1);
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
            *masksa |= (1<<3);
            datos->nombre = arg[i];
            i++;
        }
    }

    if (*mask != *maska) printMan();
}

void dirwalk(char *dir, int depth, busqueda datos, int mask)
{
    DIR *dp;
    struct dirent *entry;
    struct stat stbuf;
    int fd;
    /* abrir el directorio dir */
    if ((dp = opendir(dir)) == NULL)
    {
        printf("No se pudo abrir el directorio: %s\n", dir);
        return;
    }
    /* cambiar el directorio del trabajo actual a dir */
    chdir(dir);

    char buscar[100];
    if ((mask & (1 << (depth / 4))) != 0)
        switch (depth)
        {
        case 0:
        {
            strcpy(buscar, datos.region);
            break;
        }
        case 4:
        {
            strcpy(buscar, datos.especie);
            break;
        }
        case 8:
        {
            strcpy(buscar, datos.tipo);
            break;
        }
        case 12:
        {
            strcpy(buscar, datos.nombre);
            break;
        }
        default:
            break;
        }

    /* leemos cada entrada del directorio abierto */
    while ((entry = readdir(dp)) != NULL)
    {
        /* si el bit no esta prendido o encontramos la carpeta especifica */
        if (!(mask & (1 << (depth / 4))) || (strcmp(entry->d_name, buscar) == 0))
        {
            /* abrimos el archivo para usar fstat */
            if ((fd = open(entry->d_name, O_RDONLY)) < 1)
            {
                printf("No se pudo abrir el archivo %s\n", entry->d_name);
                return;
            }

            if (fstat(fd, &stbuf) == -1)
            {
                printf("Se encontro un error al obtener informacion de la entrada %s\n", entry->d_name);
                return;
            }

            if (S_ISDIR(stbuf.st_mode))
            {
                /* Si es un directorio, se ignora a si mismo y al padre */
                if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                    continue;
                printf("%*s%s/\n", depth, "", entry->d_name);
                /* lanzamos recursivamente a dirwalk */
                dirwalk(entry->d_name, depth + 4, datos, mask);
            }
            else
                printf("%*s%s %ld KB\n", depth, "", entry->d_name, (stbuf.st_size) / 1024);
            /* cerramos el archivo */
            close(fd);
        }
    }
    /* retornamos al directorio padre */
    chdir("..");
    /* cerramos el directorio */
    closedir(dp);
}

int main(int argc, char *argv[])
{
    /* obtenemos el directorio actual */
    /*char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("Current working dir: %s\n", cwd);
    else
    {
        perror("getcwd() error");
        return 1;
    }*/
    int maskConArg = 0;
    int maskSinArg = 0;
    int maskArg = 0;
    busqueda datos;
    readD(argc, argv, &datos, &maskConArg, &maskArg, &maskSinArg);
   /* dirwalk(cwd, 0, datos, mask);*/
    return 0;
}
