OBJ = $(patsubst %.c, %.o, $(wildcard *.c))
EXE = utest.elf

CFLAGS += -I../libgfx/include
CFLAGS += -O3
LDFLAGS += -L../libgfx -l:libgfx.a

all: $(EXE)

$(EXE): $(OBJ)
	$(LD) -o $(EXE) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

gen_cc_json: clean
	bear -- make all

clean:
	rm -f *.o $(EXE)
