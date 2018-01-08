
# ---------------------------------------------------------
# makefile permettant la compilation du programme principal
# ---------------------------------------------------------
.KEEP_STATE:

# definition des variables
CC = gcc
OBJ = ../OBJ
EXE = ../EXE
CCFLAGS = -std=c11 -Wall -g `pkg-config --cflags gtk+-2.0`
LDFLAGS = `pkg-config --libs gtk+-2.0`

# compilation
OBJ_FILES = compute.o 

all: ImaProjet.exe ImaProjet_auto.exe
	mv *.o ${OBJ}

.c.o:
	${CC} ${CCFLAGS} -o $@ -c $<

ImaProjet.exe: ${OBJ_FILES} main.o
	${CC} -o ${EXE}/$@ ${OBJ_FILES} main.o ${LDFLAGS}

ImaProjet_auto.exe: ${OBJ_FILES} main_auto.o
	${CC} -o ${EXE}/$@ ${OBJ_FILES} main_auto.o ${LDFLAGS}

clean:
	\rm -f ${OBJ}/*.o ../*/core ${EXE}/*.exe

install:
	cp *.h ${INC}

.SUFFIXES: .exe .o .c .h
