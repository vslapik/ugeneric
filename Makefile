lib = libugeneric.a

CTAGS    := $(shell command -v ctags 2> /dev/null)
LINT     := $(shell command -v cland-tidy 2> /dev/null)
ASTYLE   := $(shell command -v astyle 2> /dev/null)
VALGRIND := $(shell command -v valgrind 2> /dev/null)
DEBUG    := $(shell ls debug 2> /dev/null)

#CC           := g++ -fpermissive
SANFLAGS      := -fsanitize=undefined
PFLAGS        := -fprofile-arcs -ftest-coverage
VFLAGS        := -q --child-silent-after-fork=yes --leak-check=full \
                 --error-exitcode=3
CFLAGS_COMMON := -I. -g -std=c11 -Wall -Wextra -Winline -pedantic \
                 -Wno-missing-field-initializers -Wno-missing-braces

ifdef DEBUG
CFLAGS := $(CFLAGS_COMMON) -O0 -DENABLE_UASSERT_INPUT $(PFLAGS) $(SANFLAGS)
else
CFLAGS := $(CFLAGS_COMMON) -O3
endif

src    := generic.c stack.c vector.c queue.c heap.c list.c graph.c bitmap.c \
          sort.c string_utils.c file_utils.c bst.c mem.c dsu.c dict.c htbl.c \
          struct.c set.c
tsrc   := $(patsubst %.c, test_%.c, $(src))
texe   := $(patsubst %.c, %, $(tsrc))
checks := $(patsubst test_%, check_%, $(texe))
hdr    := ${src:.c=.h}
obj    := ${src:.c=.o}

all: $(lib) tags test test_fuzz

$(obj): Makefile

lib: $(lib)

test: $(texe)
test_%: test_%.c $(lib)
	$(CC) $(CFLAGS) test_$*.c $(lib) -o $@

$(lib): $(obj) tags $(hdr) Makefile backtrace.c
	$(CC) $(CFLAGS) -c backtrace.c -o backtrace.o
	ar rcs $(lib) $(obj) backtrace.o

tags: $(src) $(hdr)
ifdef CTAGS
	$(CTAGS) -R .
else
	$(warning "ctags is not found, index generation is skipped")
endif

test_fuzz: $(lib) ut_utils.c test_fuzz.c
	$(CC) $(CFLAGS) -c ut_utils.c -o ut_utils.o
	$(CC) $(CFLAGS) -c test_fuzz.c -o test_fuzz.o
	$(CC) $(CFLAGS) ut_utils.o test_fuzz.o $(lib) -o $@

.PHONY: clean
clean:
	$(RM) *.o $(lib) tags core* vgcore.* *.gcno *.gcda *.gcov $(texe) callgrind.out.* *.i *.s test_fuzz default.profraw

check_%: test_%
	@printf "====================[ %-12s ]====================\n"  $*
ifdef VALGRIND
	@$(VALGRIND) $(VFLAGS) ./test_$* && echo "valgrind: OK"
else
	$(warning "valgrind is not found, consider to install it")
	./test_$*
endif
ifeq ($(CC), gcc)
	@gcov $*.c | grep $*.c -A1 | grep Lines | sed 's/Lines executed:/Coverage: /'
endif

check: $(checks)

check_all: $(checks)
	make check_fuzz

style:
	astyle --options=astyle.cfg *.[ch]

LINT_CHECKS = -checks=*,-llvm-header-guard,-hicpp-braces-around-statements,-readability-braces-around-statements,-google-readability-todo,-hicpp-signed-bitwise,-readability-else-after-return

lint:
	clang-tidy *.[ch] -header-filter=.* $(LINT_CHECKS) -- -std=c11

print-%  : ; @echo $* = $($*)
