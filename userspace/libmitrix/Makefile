OBJ = $(patsubst %.c, %.o, $(wildcard src/*.c))

CC_flags =
CC_flags += -ffreestanding
CC_flags += -fno-builtin
CC_flags += -nostdlib
CC_flags += -Wall
CC_flags += -Wextra
CC_flags += -I include

all: libmitrix.a

libmitrix.a: $(OBJ)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) -c $< -o $@ $(CC_flags)

gen_cc_json: clean
	bear -- make all

clean:
	rm -f *.o libmitrix.a
