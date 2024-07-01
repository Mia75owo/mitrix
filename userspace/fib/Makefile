OBJ = $(patsubst %.c, %.o, $(wildcard *.c))
EXE = fib.elf

include ../../util.mk

.DEFAULT_GOAL := all
all: $(EXE)

$(EXE): $(OBJ)
	@$(LD) -o $(EXE) $(OBJ) $(LDFLAGS)
	@$(call PRETTYLD,"$<")

%.o: %.c
	@$(CC) -c $< -o $@ $(CFLAGS)
	@$(call PRETTYCC,"$<")

gen_cc_json: clean
	bear -- make all

clean:
	@rm -rf *.o $(EXE)
	@$(call PRETTYRM,"*.o $(EXE)")
