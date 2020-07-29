lib = libugeneric.a

SRCDIR   := src
INCDIR   := include
BUILDDIR := build
TESTDIR  := tests

CTAGS    := $(shell command -v ctags 2> /dev/null)
VALGRIND := $(shell command -v valgrind 2> /dev/null)
LINT     := $(shell command -v scan-build 2> /dev/null)
FORMAT   := $(shell command -v clang-format 2> /dev/null)
DEBUG    := $(shell ls debug 2> /dev/null)

#CC            := g++ -fpermissive # cries but compiles
SANFLAGS      := -fsanitize=undefined
PFLAGS        := -fprofile-arcs -ftest-coverage
VFLAGS        := -q --child-silent-after-fork=yes --leak-check=full \
                 --error-exitcode=3
CFLAGS_COMMON := -I$(INCDIR) -g -std=c11 -Wall -Wextra -Winline -pedantic \
                 -Wno-missing-field-initializers -Wno-missing-braces

ifdef DEBUG
CFLAGS := $(CFLAGS_COMMON) -O0 -DENABLE_UASSERT_INPUT $(PFLAGS) $(SANFLAGS)
else
CFLAGS := $(CFLAGS_COMMON) -O3
endif

src    := $(shell find $(SRCDIR) -type f -name \*.c)
hdr    := $(shell find $(INCDIR) -type f -name \*.h)
obj    := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(src:.c=.o))
tsrc   := $(shell find $(TESTDIR) -type f -name test\*.c)
texe   := $(patsubst $(TESTDIR)/%.c, %, $(tsrc))
checks := $(patsubst test_%, check_%, $(texe))

all: $(lib)
lib: $(lib)
$(obj): Makefile

$(BUILDDIR):
	mkdir -p $(BUILDDIR) # create build dir if not present

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	@$(CC) $(CFLAGS) -MM $< -MT $(BUILDDIR)/$*.o > $(BUILDDIR)/$*.c.d
	$(CC) $(CFLAGS) -c -o $@ $<


$(BUILDDIR)/ut_utils.o: $(TESTDIR)/ut_utils.c $(TESTDIR)/ut_utils.h
	@$(CC) $(CFLAGS) -MM $< -MT $(BUILDDIR)/ut_utils.o > $(BUILDDIR)/ut_utils.c.d
	$(CC) $(CFLAGS) -c -o $@ $<

test_%: $(TESTDIR)/test_%.c $(lib) $(BUILDDIR)/ut_utils.o $(lib)
	$(CC) $(CFLAGS) $(BUILDDIR)/ut_utils.o $< $(lib) -o $@

$(lib): $(obj) Makefile
	ar rcs $(lib) $(obj)

-include $(obj:.o=.c.d) # drag in all the dependencies for existing obj files


test: $(texe)

check_%: test_%
	@printf "====================[ %-12s ]====================\n"  $*
ifdef VALGRIND
	@$(VALGRIND) $(VFLAGS) ./test_$* && echo "valgrind: OK"
else
	$(warning "valgrind is not found, consider installing it")
	./test_$*
endif
ifeq ($(CC), gcc)
	@gcov $*.c | grep $*.c -A1 | grep Lines | sed 's/Lines executed:/Coverage: /'
endif

check: $(checks)

check_all: $(checks)
	make check_fuzz

# Style checker.
style:
ifdef FORMAT
	find . -regex '.*\.\(c\|h\)' -exec clang-format -style=file -i {} \;
else
	$(warning "clang-format was not found, consider installing it")
endif


# Static analysis.
lint:
ifdef LINT
	scan-build make
else
	$(warning "scan-build was not found, consider installing it")
endif


# Debug helper, 'make print-VAR' to see what is in VAR.
print-%  : ; @echo $* = $($*)


# Clean-up.
.PHONY: clean
clean:
	rm -rf $(BUILDDIR) $(lib) tags core* vgcore.* src/*.gcno *.gcda *.gcov $(texe) callgrind.out.* *.i *.s default.profraw


# Index generation for vim.
.PHONY: tags
tags:
ifdef CTAGS
	$(CTAGS) -R .
else
	$(warning "ctags is not found, index generation is skipped")
endif
