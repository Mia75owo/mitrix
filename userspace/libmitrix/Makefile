OBJ = $(patsubst %.c, %.o, $(wildcard src/*.c))

CFLAGS =
CFLAGS += -O3
CFLAGS += -ffreestanding
CFLAGS += -fno-builtin
CFLAGS += -nostdlib
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -I include

all: libmitrix.a

libmitrix.a: $(OBJ)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

gen_cc_json: clean
	bear -- make all

clean:
	rm -f *.o libmitrix.a
