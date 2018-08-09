CC := gcc
CFLAGS := -O2
LDFLAGS += -lncurses
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))
PROG_NAME = cross_zeros

.PHONY: all clear

#.o:.c
#	${CC} ${CFLAGS} -o $$

$(PROG_NAME):$(OBJECTS)
	$(CC) $(LDFLAGS) -o $(PROG_NAME) $^

all: $(PROG_NAME)

clean:
	-rm $(OBJECTS) $(PROG_NAME)
