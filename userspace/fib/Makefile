OBJ = $(patsubst %.c, %.o, $(wildcard *.c))
EXE = fib.elf

all: $(EXE)

$(EXE): $(OBJ)
	$(LD) -o $(EXE) $(OBJ) $(LD_flags)

%.o: %.c
	$(CC) -c $< -o $@ $(CC_flags)

gen_cc_json: clean
	bear -- make all

clean:
	rm -f *.o $(EXE)
