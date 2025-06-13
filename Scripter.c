#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// COmentario de mierda
/* CONST VARS */
const int max_line = 1024;
const int max_commands = 10;
#define max_redirections 3 // stdin, stdout, stderr
#define max_args 15

/* VARS GLOBAL */
char *argvv[max_args];
char *filev[max_redirections];
int background = 0;

// Variable de manejo de errores en ejecutar_comando
/*
Como no podemos asegurar que siempre se devuelva algo en ejecutar_comando debido
a la presencia de un execvp, entonces creamos una variable externa que solo se
modificará si hay errores. Usaremos una convención de errores para saber que
tipo de error debemos lanzar. La convención es la siguiente:
-> error management = 0: Todo ha salido bien (se ha ejecutado el execvp)
-> error management = -1: Error al abrir el archivo
-> error management = -2: Error al redirigir
-> error management = -3: Redirección no esperada
-> error management = -4: Error al cerrar el archivo
-> error management = -5: Error al ejecutar el comando
-> error management = -6: Error al crear pipe
-> error management = -6: Error al crear hijo
*/
int error_management = 0;

/**
 * This function splits a char* line into different tokens based on a given
 * character
 * @return Number of tokens
 */
int tokenizar_linea(char *linea, char *delim, char *tokens[], int max_tokens) {
  int i = 0;
  char *token = strtok(linea, delim);
  while (token != NULL && i < max_tokens - 1) {
    tokens[i++] = token;
    token = strtok(NULL, delim);
  }
  tokens[i] = NULL;
  return i;
}

/**
 * This function processes the command line to evaluate if there are
 * redirections. If any redirection is detected, the destination file is
 * indicated in filev[i] array. filev[0] for STDIN filev[1] for STDOUT filev[2]
 * for STDERR
 */
void procesar_redirecciones(char *args[]) {
  // initialization for every command
  filev[0] = NULL;
  filev[1] = NULL;
  filev[2] = NULL;
  // Store the pointer to the filename if needed.
  // args[i] set to NULL once redirection is processed
  for (int i = 0; args[i] != NULL; i++) {
    if (strcmp(args[i], "<") == 0) {
      filev[0] = args[i + 1];
      args[i] = NULL;
      args[i + 1] = NULL;
    } else if (strcmp(args[i], ">") == 0) {
      filev[1] = args[i + 1];
      args[i] = NULL;
      args[i + 1] = NULL;
    } else if (strcmp(args[i], "!>") == 0) {
      filev[2] = args[i + 1];
      args[i] = NULL;
      args[i + 1] = NULL;
    }
  }
}

/**
 * This function processes the input command line and returns in global
 * variables: argvv -- command an args as argv filev -- files for redirections.
 * NULL value means no redirection. background -- 0 means foreground; 1
 * background.
 */
int procesar_linea(char *linea) {
  char *comandos[max_commands];
  int num_comandos = tokenizar_linea(linea, "|", comandos, max_commands);

  // Check if background is indicated
  if (num_comandos > 0 && strchr(comandos[num_comandos - 1], '&')) {
    background = 1;
    char *pos = strchr(comandos[num_comandos - 1], '&');
    // remove character
    *pos = '\0';
  }

  // Finish processing
  for (int i = 0; i < num_comandos; i++) {
    int args_count = tokenizar_linea(comandos[i], " \t\n", argvv, max_args);
    procesar_redirecciones(argvv);
    printf("%d", args_count);

    //**********************************REMOVE
    // THIS****************************************
    printf("Comando = %s\n", argvv[0]);
    for (int arg = 1; arg < max_args; arg++)
      if (argvv[arg] != NULL)
        printf("Args = %s\n", argvv[arg]);

    printf("Background = %d\n", background);
    if (filev[0] != NULL)
      printf("Redir [IN] = %s\n", filev[0]);
    if (filev[1] != NULL)
      printf("Redir [OUT] = %s\n", filev[1]);
    if (filev[2] != NULL)
      printf("Redir [ERR] = %s\n", filev[2]);
    /**********************************************************************************************/
  }

  return num_comandos;
}

void matar_zombies(int signal) {
  while (waitpid(-1, NULL, WNOHANG) > 0) {
    printf("Hijo muerto\n");
  }
}

int manejar_error(int error) {
  switch (error) {
  case -1:
    perror("Error al abrir el archivo");
    break;

  case -2:
    perror("Error al redirigir");
    break;

  case -3:
    perror("Redirección no esperada");
    break;

  case -4:
    perror("Error al cerrar el archivo");
    break;

  case -5:
    perror("Error al ejecutar el comando");
    break;
  }
  return error;
}

int comprobar_redireccion() {
  return filev[0] == NULL && filev[1] == NULL && filev[2] == NULL;
}

void redirigir_target(int target) {
  int fd;

  if ((fd = open(filev[target], O_RDONLY)) < 0) {
    // perror("Error al abrir el archivo");
    error_management = -1;
    return;
  }

  // Redirigimos el stdout al fichero al que queremos redirigir la entrada
  if (target == 0) {
    if (dup2(fd, STDIN_FILENO) < 0) {
      // perror("Error al redirigir la salida");
      error_management = -2;
      close(fd);
      return;
    }
  } else if (target == 1) {
    if (dup2(fd, STDOUT_FILENO) < 0) {
      // perror("Error al redirigir la salida");
      error_management = -2;
      close(fd);
      return;
    }
  } else if (target == 2) {
    if (dup2(fd, STDERR_FILENO) < 0) {
      // perror("Error al redirigir la salida");
      error_management = -2;
      close(fd);
      return;
    }
  } else {
    // perror("Redirección no esperada");
    error_management = -3;
  }
  if (close(fd) < 0) {
    // perror("Error al cerrar el archivo");
    error_management = -4;
  }
}

void ejecutar_comandos(int n_comandos, int (*pipes)[2]) {
  int pids[n_comandos];

  for (int i = 0; i < n_comandos; i++) {
    int pid = fork();
    pids[i] = pid;

    // Error
    if (pid < 0) {
      perror("No se ha podido crear el hijo");
      error_management = -7;
    }

    // Padre
    else if (pid != 0) {
      for (int j = 0; j < n_comandos - 1; j++) {
        /*if (j != i) { // Cerrar todos los pipes que no usa este proceso*/
          close(pipes[j][0]);
          close(pipes[j][1]);
        /*}*/
      }
      if (background == 0) {
        for (int i = 0; i < n_comandos; i++) {
          waitpid(pids[i], NULL, 0);
        }
      } else {
        if (i == n_comandos - 1) {
          printf("El pid del ultimo es %d", pids[n_comandos - 1]);
          for (int i = 0; i < n_comandos; i++) {
            waitpid(pids[i], NULL, WNOHANG);
          }
        }
      }
    }

    // Hijos
    else {

      if (n_comandos > 1) {
        printf("hola\n");

        // Primero
        if (i == 0) {
          for (int j = 0; j < n_comandos - 1; j++) {
            if (j != i) { // Cerrar todos los pipes que no usa este proceso
              close(pipes[j][0]);
              close(pipes[j][1]);
            }
          }
          printf("Primera\n");
          if (close(pipes[i][0]) < 0) {
            error_management = -9;
          }
          printf("Antes\n");
          dup2(pipes[i][1], STDOUT_FILENO);
            
          printf("Depsues\n");
          if (close(pipes[i][1]) < 0) {
            error_management = -9;
          }
        }
        // Ultimo
        else if (i == n_comandos - 1) {
          for (int j = 0; j < n_comandos - 1; j++) {
            if (j != i) { // Cerrar todos los pipes que no usa este proceso
              close(pipes[j][0]);
              close(pipes[j][1]);
            }
          }
          printf("Segunda\n");
          if (close(pipes[i - 1][1]) < 0) {
            error_management = -9;
          }
          dup2(pipes[i - 1][0], STDIN_FILENO);
          /*dup2(STDIN_FILENO, pipes[i - 1][0]);*/
          if (close(pipes[i - 1][0]) < 0) {
            error_management = -9;
          } 
        }
        // Medio
        else {
          for (int j = 0; j < n_comandos - 1; j++) {
            if (j != i) { // Cerrar todos los pipes que no usa este proceso
              close(pipes[j][0]);
              close(pipes[j][1]);
            }
          }
          dup2(pipes[i - 1][0], STDIN_FILENO);
          // close
          dup2(STDOUT_FILENO, pipes[i][1]);
          // close
        }
      }

      for (int i = 0; i < 3; i++) {
        if (filev[i] != NULL) {
          redirigir_target(i);
        }
      }

      printf("Llega a ejecutar");
      execvp(argvv[0], argvv);
      exit(0);
      error_management = -5;
    }
  }
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    perror("Invalid number of arguments");
    return -1;
  }

  char buffer;
  char command[max_line];

  int nlineas = 0;

  int count_command = 0;

  int fd, nread, n_comandos;

  signal(SIGCHLD, matar_zombies);

  if ((fd = open(argv[1], O_RDWR, 0644)) < 0) {
    perror("Error al abrir el archivo\n");
    return -1;
  }

  int leido = 0;

  while ((nread = read(fd, &buffer, 1)) > 0) {
    count_command++;

    if (buffer == '\n') {
      command[count_command - 1] = '\0';
      nlineas++;
      printf("linea: %s\n", command);

      /*if ((count_command != strlen("## Script de SSOO ") ||*/
      /*     strcmp(command, "## Script de SSOO") != 0)) {*/
      /*  perror("Invalid format: Missing title <Script de SSOO>");*/
      /*  return -1;*/
      /*}*/

      if (strcmp(command, "## Script de SSOO") == 0 && leido == 0) {
        memset(command, '\0', max_line);
        leido++;
        count_command = 0;
        continue;
      }

      // Procesar comandos
      n_comandos = procesar_linea(command);

      if (n_comandos > 3) {
        perror("File Error: more than 3 commands in one line\n");
        return -1;
      }

      int pipes[n_comandos - 1][2];

      // Generar pipes
      for (int i = 0; i < n_comandos - 1; i++) {
        if (pipe(pipes[i]) < 0) {
          perror("Error al crear pipe");
          return -6;
        }
      }

      ejecutar_comandos(n_comandos, pipes);

      int e = manejar_error(error_management);

      if (e < 0) {
        return e;
      }

      count_command = 0;

      memset(command, '\0', max_line);

      if (nread == 0)
        break;

    } else {
      command[count_command - 1] = buffer;
    }
  }
}
