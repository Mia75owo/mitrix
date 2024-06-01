OBJ = $(patsubst %.c, %.o, $(wildcard src/*.c))

CC_flags += -O3
CC_flags += -I include

all: libgfx.a

libgfx.a: $(OBJ)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) -c $< -o $@ $(CC_flags)

gen_cc_json: clean
	bear -- make all

clean:
	rm -f *.o libgfx.a
