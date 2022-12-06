MF=	Makefile

CC=	mpicc
CFLAGS= -cc=icc -O3 -Wall

LFLAGS= $(CFLAGS)

EXE=	automaton

INC= \
	automaton.h

SRC= \
	automaton.c \


#
# No need to edit below this line
#

.SUFFIXES:
.SUFFIXES: .c .o

OBJ=	$(SRC:.c=.o)

.c.o:
	$(CC) $(CFLAGS) -c $<

all:	$(EXE)

$(OBJ):	$(INC)

$(EXE):	$(OBJ)
	$(CC) $(LFLAGS) -o $@ $(OBJ)

$(OBJ):	$(MF)

clean:
	rm -f $(EXE) $(OBJ) core
