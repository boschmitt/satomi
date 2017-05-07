#===--- Makefile ------------------------------------------------------------===
#
#                     satomi: Satisfiability solver
#
# This file is distributed under the BSD 2-Clause License.
# See LICENSE for details.
#
#===-------------------------------------------------------------------------===
CC=cc

# Default settings
STD=-std=c11
OPT=-O3
WARN=
DEBUG=
FINAL_CFLAGS=$(STD) $(WARN) $(OPT) $(DEBUG) $(CFLAGS)
FINAL_LDFLAGS=$(LDFLAGS) $(DEBUG)

TARGET=satomi
SATOMI_INCLUDE= -I./include -I./src
SATOMI_SOURCES= src/main.c src/cnf_reader.c src/solver.c src/solver_api.c
SATOMI_OBJECTS= $(patsubst %.c, %.o, $(SATOMI_SOURCES))

FINAL_CFLAGS+=$(SATOMI_INCLUDE)

# Terminal output
CCCOLOR="\033[34m"
LINKCOLOR="\033[34;1m"
SRCCOLOR="\033[33m"
BINCOLOR="\033[37;1m"
MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"

MSG_CC = @printf '    %b %b\n' $(CCCOLOR)CC$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR) 1>&2;
MSG_LINK = @printf '    %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
MSG_INSTALL = @printf '    %b %b\n' $(LINKCOLOR)INSTALL$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;

# Commands
SATOMI_CC=$(MSG_CC)$(CC) $(FINAL_CFLAGS)
SATOMI_LD=$(MSG_LINK)$(CC) $(FINAL_LDFLAGS)
SATOMI_INSTALL=$(MSG_INSTALL)$(INSTALL)

all: $(TARGET)
	@echo ""
	@echo "Hint: It's a good idea to run 'make test' =)"
	@echo ""

.PHONY: all

dep:
	$(SATOMI_CC) -MM $(SATOMI_SOURCES) > Makefile.dep

.PHONY: dep

debug: OPT=
debug: DEBUG:=-g -DDEBUG
debug: WARN:=-Weverything -Wno-gnu-zero-variadic-macro-arguments
debug: $(TARGET)

.PHONY: debug

$(TARGET): $(SATOMI_OBJECTS)
	$(SATOMI_LD) -o $@ $^ $(FINAL_LIBS)

%.o: %.c
	$(SATOMI_CC) -c $< -o $@

# The Cleaner
clean:
	rm -rf $(SATOMI_OBJECTS)
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

.PHONY: clean
