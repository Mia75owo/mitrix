ANSI_BLUE="\e[0;34m"
ANSI_RED="\e[0;31m"
ANSI_RESET="\e[0m"

define PRETTYCC
	@echo -e $(ANSI_BLUE)"CC "$(ANSI_RESET)$(1)
endef
define PRETTYAS
	@echo -e $(ANSI_BLUE)"AS "$(ANSI_RESET)$(1)
endef
define PRETTYLD
	@echo -e $(ANSI_BLUE)"LD "$(ANSI_RESET)$(1)
endef
define PRETTYAR
	@echo -e $(ANSI_BLUE)"AR "$(ANSI_RESET)$(1)
endef
define PRETTYRM
	@echo -e $(ANSI_RED)"RM "$(ANSI_RESET)$(1)
endef

help: # Print help on Makefile
	@grep '^[^.#]\+:\s\+.*#' Makefile | \
	sed "s/\(.\+\):\s*\(.*\) #\s*\(.*\)/`printf "\033[93m"`\1`printf "\033[0m"`	\3 [\2]/" | \
	expand -t20
