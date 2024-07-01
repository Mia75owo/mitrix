# You can override the CFLAGS and C compiler externally,
# e.g. make PLATFORM=cortex-m3
CFLAGS += -Wno-all -Wno-extra -I include -O3

# With this, the makefile should work on Windows also.
ifdef windir
  RM = del
endif

# Just include all the source files in the build.
CSRC = $(wildcard src/*.c)
OBJS = $(CSRC:.c=.o)

# And the files for the test suite
TESTS_CSRC = $(wildcard tests/*_tests.c)
TESTS_OBJS = $(TESTS_CSRC:.c=)

# Some of the files uses "templates", i.e. common pieces
# of code included from multiple files.
CFLAGS += -Isrc/templates

include ../../util.mk

.DEFAULT_GOAL := all
all: libc.a

gen_cc_json: clean # Genereate compile_commands.json for LSP
	bear -- make all

clean: # Clean generated files
	@$(RM) $(OBJS) $(TESTS_OBJS) libc.a
	@$(call PRETTYRM,"$(OBJS) $(TESTS_OBJS) libc.a")

libc.a: $(OBJS)
	@$(RM) $@
	@$(call PRETTYRM,"$@")
	@$(AR) ru $@ $^
	@$(call PRETTYAR,"$@")

run_tests: $(TESTS_OBJS)
	$(foreach f,$^,$f)

tests/%: tests/%.c tests/tests_glue.c libc.a
	@$(CC) $(CFLAGS) -o $@ $^
	@$(call PRETTYCC,"$^")

%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $<
	@$(call PRETTYCC,"$<")
