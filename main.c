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

void readD(int narg, char *arg[], busqueda *datos, int *mask) 
{
    int i = 1;
    while (i < narg)
    {
        if (strcmp(arg[i], "-r") == 0) 
        {
            *mask += 1;
            datos->region = arg[i+1];
            i += 2;
        }
        else if (strcmp(arg[i], "-s") == 0) 
        {
            if (i+1 < narg && ((strcmp(arg[i+1], "pokemon") == 0) || (strcmp(arg[i+1], "trainer") == 0)))
            {
                *mask += (1<<1);
                datos->especie = arg[i+1];
                i += 2;
            } else 
            {
                *mask += (1<<5);
                i++;
            }
        }
        else if (strcmp(arg[i], "-t") == 0)
        {
            *mask += (1<<2);
            datos->tipo = arg[i+1];
            i += 2;
        }
        else if ((strcmp(arg[i], "-c") == 0) || (strcmp(arg[i], "--nocount") == 0))
        {
            *mask += (1<<3);
            i++;
        }
        else if ((strcmp(arg[i], "-l") == 0) || (strcmp(arg[i], "--list") == 0))
        {
            *mask += (1<<4);
            i++;
        }
        else if (strcmp(arg[i], "--size") == 0)
        {
            *mask += (1<<5);
            i++;
        } 
        else
        {
            *mask += (1<<6);
            datos->nombre = arg[i];
            i++;
        }     
    }
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
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("Current working dir: %s\n", cwd);
    else
    {
        perror("getcwd() error");
        return 1;
    }
    int mask = 0;
    busqueda datos;
    readD(argc, argv, &datos, &mask);
    dirwalk(cwd, 0, datos, mask);
    return 0;
}
