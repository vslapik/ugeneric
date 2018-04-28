lib = libugeneric.a
#CC = g++ -fpermissive
PFLAGS = -fprofile-arcs -ftest-coverage
CFLAGS_COMMON=-I. -g -std=c11 -Wall -Wextra -Winline -pedantic -Wno-missing-field-initializers -Wno-missing-braces $(PFLAGS)
#CFLAGS = $(CFLAGS_COMMON) -O0 -DENABLE_UASSERT_INPUT $(PFLAGS)
CFLAGS = $(CFLAGS_COMMON) -Ofast
VFLAGS = -q --child-silent-after-fork=yes --leak-check=full --error-exitcode=3

src = generic.c stack.c vector.c queue.c heap.c list.c graph.c bitmap.c sort.c string_utils.c file_utils.c bst.c mem.c dsu.c dict.c htbl.c struct.c set.c
tsrc = $(patsubst %.c, test_%.c, $(src))
texe = $(patsubst %.c, %, $(tsrc))
checks = $(patsubst test_%, check_%, $(texe))

hdr = ${src:.c=.h}
obj = ${src:.c=.o}

all: $(lib) tags test test_fuzz

lib: $(lib)

test: $(texe)
test_%: test_%.c $(lib)
	$(CC) $(CFLAGS) test_$*.c $(lib) -o $@ -lgcov

$(lib): $(obj) tags $(hdr) Makefile backtrace.c
	$(CC) $(CFLAGS) -c backtrace.c -o backtrace.o
	ar rcs $(lib) $(obj) backtrace.o

tags: $(src) $(hdr)
	ctags -R .

test_fuzz: $(lib) ut_utils.c test_fuzz.c
	$(CC) $(CFLAGS) -c ut_utils.c -o ut_utils.o
	$(CC) $(CFLAGS) -c test_fuzz.c -o test_fuzz.o
	$(CC) $(CFLAGS) ut_utils.o test_fuzz.o $(lib) -o $@ -lgcov

.PHONY: clean
clean:
	$(RM) *.o $(lib) tags core* vgcore.* *.gcno *.gcda *.gcov $(texe) callgrind.out.* *.i *.s test_fuzz default.profraw

check_%: test_%
	@printf "====================[ %-12s ]====================\n"  $*
	@valgrind $(VFLAGS) ./test_$* && echo "valgrind: OK"
ifeq ($(CC), gcc)
	@gcov $*.c | grep $*.c -A1 | grep Lines | sed 's/Lines executed:/Coverage: /'
endif

check: $(checks)

print-%  : ; @echo $* = $($*)
