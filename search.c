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

int main()
{
    /* obtenemos el directorio actual */
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("Current working dir: %s\n", cwd);
    else
    {
        perror("getcwd() error");
        return 1;
    }
    int mask = 6;
    busqueda datos;
    datos.especie = "trainers";
    datos.tipo = "main";
    dirwalk(cwd, 0, datos, mask);
    return 0;
}