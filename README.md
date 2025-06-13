# Batch Command Interpreter in C

This project is a simple command interpreter written in C that reads a list of commands from a file and executes them sequentially. It mimics a basic shell-like behavior in batch mode, using system-level calls such as `fork()` and `execvp()`.

## 📄 Description

Instead of interacting with the user via terminal input, this program takes a file as an argument. The file must contain first a line with the text (## Script) and then in the next line among one or three commands per line. The interpreter reads each line, parses the command and its arguments, and executes it as a separate process.

## ✅ Features

- Reads commands from a text file
- Executes standard shell commands using `execvp()`
- Handles command arguments
- Uses `fork()` to create a child process for each command
- Waits for each command to complete before moving to the next

## 🛠️ Compilation

To compile the program there is a file called makefile in charge of it:

```bash
make
```
## 🚀 Usage

Run the program with a file containing commands:

```bash
./scripter commands.txt
```

Where `commands.txt` must look like this:

```commands.txt
## Script
ls -l | grep ".c"
cat fichero > fich1
echo "Hola"
```

Each command will be executed in the order listed in the file.

## 🔧 Possible improvements

- Add better memory control
- Command history
- Better control pipes

## 👨‍💻 Author

Daniel López-Antona Pesquera - Computer Engineering Student

## 📄 License

This proyect is licensed under the [MIT License](https://github.com/Frosty2205/Command-interpreter-C/blob/main/License.md)
