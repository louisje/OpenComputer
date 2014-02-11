CC   = gcc$(EXESUF) -DDEBUG
OBJ  = Parser.o Tree.o Lib.o Scanner.o Array.o Compiler.o HashTable.o Generator.o Assembler.o Cpu.o OpTable.o OS.o PCode.o SymTable.o Interpreter.o Semantic.o
LINKOBJ = $(OBJ)
LIBS = 
INCS = 
BIN  = test$(EXESUF) cc1$(EXESUF) as1$(EXESUF) vm1$(EXESUF) os1$(EXESUF) ci1$(EXESUF)
CFLAGS = $(INCS) -g3
RM = rm -f

.PHONY: all clean

all: $(OBJ) test cc1 as1 vm1 os1 ci1

test: $(OBJ)
	$(CC) main.c $(LINKOBJ) -DTARGET=TEST -o test $(LIBS)

cc1: $(OBJ)
	$(CC) main.c $(LINKOBJ) -DTARGET=CC -o cc1 $(LIBS)

as1: $(OBJ)
	$(CC) main.c $(LINKOBJ) -DTARGET=AS -o as1 $(LIBS)

vm1: $(OBJ)
	$(CC) main.c $(LINKOBJ) -DTARGET=VM -o vm1 $(LIBS)

os1: $(OBJ)
	$(CC) main.c $(LINKOBJ) -DTARGET=OS -o os1 $(LIBS)

ci1: $(OBJ)
	$(CC) main.c $(LINKOBJ) -DTARGET=CI -o ci1 $(LIBS)

clean: 
	${RM} $(OBJ) $(BIN)

%.o:  %.c
	$(CC) -c -o $*.o $< $(CFLAGS)
