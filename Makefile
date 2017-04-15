lib = libugeneric.a
CC = gcc
#CC = clang
PFALGS = -fprofile-arcs -ftest-coverage
CFLAGS_COMMON=-I. -g -Wall -Wextra -Winline -pedantic -rdynamic -std=c99
CFLAGS = $(CFLAGS_COMMON) -O0 -DENABLE_ASSERT_INPUT
#CFLAGS = $(CFLAGS_COMMON) -O3
VFLAGS = -q --child-silent-after-fork=yes --leak-check=full --error-exitcode=3

src = generic.c stack.c vector.c queue.c heap.c list.c graph.c bitmap.c sort.c string_utils.c file_utils.c bst.c mem.c dsu.c buffer.c dict.c htbl.c deque.c
tsrc = $(patsubst %.c, test_%.c, $(src))
texe = $(patsubst %.c, %, $(tsrc))
checks = $(patsubst test_%, check_%, $(texe))

hdr = ${src:.c=.h}
obj = ${src:.c=.o}

all: $(lib) tags test

lib: $(lib)

test: $(texe)
test_%: test_%.c $(lib)
	$(CC) $(CFLAGS) test_$*.c $(lib) -o $@ -lgcov

$(lib): $(obj) tags $(hdr)
	$(CC) $(CFLAGS) -c backtrace.c -o backtrace.o
	ar rcs $(lib) $(obj) backtrace.o

tags: $(src) $(hdr)
	ctags -R .

.PHONY: clean
clean:
	$(RM) *.o $(lib) tags core.* vgcore.* *.gcno *.gcda *.gcov $(texe) callgrind.out.*

check_%: test_%
	@echo "========================================== checking $*"
#	@gcov $*.c | grep Lines | sed 's/Lines executed:/Coverage: /'
	@valgrind $(VFLAGS) ./test_$* && echo "OK"

check: $(checks)

print-%  : ; @echo $* = $($*)
