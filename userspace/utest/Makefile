OBJ = $(patsubst %.c, %.o, $(wildcard *.c))
EXE = utest.elf

CC_flags += -I../libgfx/include
CC_flags += -O3
LD_flags += -L../libgfx -l:libgfx.a

all: $(EXE)

$(EXE): $(OBJ)
	$(LD) -o $(EXE) $(OBJ) $(LD_flags)

%.o: %.c
	$(CC) -c $< -o $@ $(CC_flags)

gen_cc_json: clean
	bear -- make all

clean:
	rm -f *.o $(EXE)
