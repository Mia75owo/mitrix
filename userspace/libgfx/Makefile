OBJ = $(patsubst %.c, %.o, $(wildcard src/*.c))

CFLAGS += -O3
CFLAGS += -I include

all: libgfx.a

libgfx.a: $(OBJ)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

gen_cc_json: clean
	bear -- make all

clean:
	rm -f *.o libgfx.a
