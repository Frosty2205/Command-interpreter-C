CC=gcc
FLAGS=-Wno-implicit-function-declaration
CFLAGS=-I.

# Para make scripter y make mygrep
OBJ_SCRIPTER = scripter.o 
OBJ_MYGREP = mygrep.o 

# Para make
all: scripter mygrep

# Para compilar scripter

scripter: $(OBJ_SCRIPTER)
	$(CC) $(FLAGS) -L. -o $@ $< $(LIBS)

# Para compilar mygrep

mygrep: $(OBJ_MYGREP)
	$(CC) $(FLAGS) -L. -o $@ $< $(LIBS)

%.o: %.c
	$(CC) $(FLAGS) -c -o $@ $< $(CFLAGS)

clean:
	rm -f ./scripter.o ./scripter
	rm -f ./mygrep.o ./mygrep
