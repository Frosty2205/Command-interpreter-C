# Intérprete de Comandos por Lote en C

Este proyecto es un intérprete de comandos sencillo escrito en C que lee una lista de comandos desde un archivo y los ejecuta secuencialmente. Simula un comportamiento básico de una shell en modo por lotes, utilizando llamadas de sistema como `fork()` y `execvp()`.

## 📄 Descripción

En lugar de interactuar con el usuario a través de la terminal, este programa toma un archivo como argumento. El archivo debe contener un comando por línea. El intérprete lee cada línea, analiza el comando y sus argumentos, y lo ejecuta como un proceso separado.

## ✅ Funcionalidades

- Lee comandos desde un archivo de texto
- Ejecuta comandos estándar de shell usando `execvp()`
- Soporta argumentos en los comandos
- Usa `fork()` para crear un proceso hijo por comando
- Espera a que cada comando termine antes de ejecutar el siguiente

## 🛠️ Compilación

Para compilar este programa hay un archivo Makefile encargado:

```bash
make
```
## 🚀 Uso

Ejecuta el programa con un archivo que contenga comandos:

```bash
./scripter commands.txt
```

Donde `commands.txt` debe verse como:

```commands.txt
## Script
ls -l | grep ".c"
cat fichero > fich1
echo "Hola"
```

Cada comando será ejecutado en el orden en que aparece en el archivo.

## 🔧 Posibles Mejoras

- Añadir mejor control de la memoria
- Historia de comandos
- Mejor control de las tuberías

## 👨‍💻 Autor

Daniel López-Antona Pesquera - Estudiante de Ingeniería Informática

## 📄 Licencia

Este proyecto está licenciado bajo la [Licencia MIT](https://github.com/Frosty2205/Command-interpreter-C/blob/main/License.md)
