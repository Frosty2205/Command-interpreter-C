#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFERSIZE 1

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: %s <ruta_fichero> <cadena_busqueda>\n", argv[0]);
        return -1;
    }

    int fd, nread;
    char buffer;

    if ((fd = open(argv[1], O_RDONLY)) < 0)
    {
        perror("File Error: could not be open");
        return -1;
    }

    int linea = 1;                  // Nos dice en qué línea se encuentra el buffer
    int i = 0;                      // Nos dice qué iteración del char introducido es la que se está comprobando
    int *lineas_encontradas = NULL; // Guardaremos las líneas en las que haya una coincidencia
    int size_lista = 0;
    int size_word = strlen(argv[2]); // Lo usamos para saber cuando se ha leido la palabra

    while ((nread = read(fd, &buffer, BUFFERSIZE)) > 0)
    {
        if (buffer == argv[2][i])
        {
            i++;
	    // Si llega aquí es que ha leído la palabra
	    if (size_word == i) {
		size_lista++;
		lineas_encontradas = (int *)realloc(lineas_encontradas, size_lista * sizeof(int));
		lineas_encontradas[size_lista - 1] = linea;
		i = 0;
	    }
	 }
	 // \n es carácter especial que solo ocupa 1 byte
	 else if (buffer == '\n') {
	      	linea++;
		i = 0;
	}
	// Ha leído una letra que no está en la palabra
	else {
	  i = 0;
	}
    }
    if (close(fd) < 0)
    {
        perror("Error al cerrar el archivo\n");
        return -1;
    }

    if (size_lista > 0)
    {
        for (i = 0; i < size_lista; i++)
        {
            printf("Se ha encontrado la cadena %s en la línea %d\n", argv[2], lineas_encontradas[i]);
        }
    }
    else
    {
        printf("%s not found\n", argv[2]);
    }

    free(lineas_encontradas);

    return 0;
}
