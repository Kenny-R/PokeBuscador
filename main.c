#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include "utilidades.h"
#include "busqueda.h"

/* variables globales */
char ext[6] = ".html";
/*
 Función que navega entre directorios para encontrar una lista
 determinada de archivos requeridos
 Argumentos:
    dir: String del directorio actual
    depth: Entero, corresponde al nivel del directorio en que se encuentra
        el directorio actual, por cada nivel se suma 4
        0: directorio raiz (se encuentran las carpetas de las regiones)
        4: donde estan las carpetas trainers/pokemon
        8: donde se encuentran las carpetas del tipo de apariciones
        12: finalmente, llegamos a una carpeta que contiene una lista de archivos html
    datos: Struct busqueda, contiene los datos que debemos buscar entre las carpetas
    mask: Entero, corresponde a la máscara de bits relacionada con los
          flags que tienen argumentos seguidos
    mask2: Entero, corresponde a la máscara de bits relacionada con los flags no tienen
          argumentos seguidos.
    count: Apuntador a un entero, un acumulador que representa la cantidad de archivos requeridos
          y encontrados hasta el momento
    totalsize: Apuntador a un entero, un acumulador del tamaño total de los archivos requeridos
          y encontrados hasta el momento (en kilobytes)
 */
void dirwalk(char *dir, int depth, busqueda datos, int mask1, int mask2, int *count, int *totalsize)
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

    char *buscar = calloc(STRING_MAX, sizeof(char));
    if (buscar == NULL)
    {
        printf("Memory not allocated\n");
        exit(1);
    }
    /* si aun no hemos llegado al ultimo nivel del directorio */
    if (depth <= 8)
    {
        if (mask1 & (1 << (depth / 4)))
        {
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
            default:
                break;
            }
        }
    }
    else
    {
        if ((mask2 & (1 << 3)) != 0)
            strcpy(buscar, datos.nombre);
    }

    /* leemos cada entrada del directorio abierto */
    while ((entry = readdir(dp)) != NULL)
    {
        /* si el bit no esta prendido o encontramos la carpeta especifica */
        if (search(depth, mask1, mask2, entry->d_name, buscar) == TRUE)
        {
            /* abrimos el archivo para usar fstat */
            if ((fd = open(entry->d_name, O_RDONLY)) < 1)
            {
                printf("Cannot open the file %s\n", entry->d_name);
                return;
            }

            if (fstat(fd, &stbuf) == -1)
            {
                printf("Encountered an error while getting the file information of %s\n", entry->d_name);
                return;
            }

            if (S_ISDIR(stbuf.st_mode))
            {
                /* Si es un directorio, se ignora a si mismo y al padre */
                if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                    continue;
                /* lanzamos recursivamente a dirwalk */
                dirwalk(entry->d_name, depth + 4, datos, mask1, mask2, count, totalsize);
            }
            else
            {
                /* verificar que el archivo sea un .html */
                if (depth == 12 && strcmp(ext, strrchr(entry->d_name, '.')) == 0)
                {
                    printdata(mask2, entry->d_name, (stbuf.st_size) / 1024);
                    *(count) += 1;
                    *(totalsize) += (stbuf.st_size) / 1024;
                }
            }
            /* cerramos el archivo */
            close(fd);
        }
    }
    free(buscar);
    /* retornamos al directorio padre */
    chdir("..");
    /* cerramos el directorio */
    closedir(dp);
}

int main(int argc, char *argv[])
{
    int maskConArg = 0;
    int maskSinArg = 0;
    int maskArg = 0;
    busqueda datos;
    readD(argc, argv, &datos, &maskConArg, &maskArg, &maskSinArg);

    /* obtenemos el directorio actual */
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd() error");
        return 1;
    }

    int count = 0, totalsize = 0;

    dirwalk(cwd, 0, datos, maskConArg, maskSinArg, &count, &totalsize);

    if ((maskSinArg & (1 << 1)) == 0)
        printf("Number of files found: %d\n", count);
    if ((maskSinArg & (1 << 2)) != 0)
        printf("The total amount of information you are looking for is %d KB\n", totalsize);
    return EXIT_SUCCESS;
}
