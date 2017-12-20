.PHONY: all clean

NAME = splitXY.nes

CFLAGS = -C nes.cfg -t nes -Oisr
CC = cl65

SRC = $(wildcard *.c)
SRC += $(wildcard *.s)

OBJ = $(SRC:.c=.o)
OBJ := $(OBJ:.s=.o)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(CFLAGS) $(OBJ)

$(OBJ): $(wildcard *.h *.sinc)

clean:
	rm -f $(NAME) *.o
