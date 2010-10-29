NAME = multigrid

include makefile.in

OBJS = grid.o scatter.o gather.o subcpy.o overhead.o

AR = ar rcs
CCOMP = gcc -g -std=c99 -O$(OPT_LEVEL) -Iinclude
NVCOMP = nvcc -g -O$(OPT_LEVEL)

TESTS := test1 test3 test4 test5

ifeq ($(HAVE_CUDA), 1)
TESTS += test2
endif

all: core tests

core: $(OBJS)
	$(AR) lib$(NAME).a $(OBJS)

tests: $(TESTS)

test1: tests/test1.c core
	$(CCOMP) -I. tests/test1.c -o tests/bin/test1 -L. -l$(NAME) -lm

test2: tests/test2.c core
	$(NVCOMP) -I. -D$(TYPE) tests/test2.cu -o tests/bin/test2 -L. -l$(NAME) -lm

test3: tests/test3.c core
	$(CCOMP) -I. -D$(TYPE) tests/test3.c -o tests/bin/test3 -L. -l$(NAME) -lm

test4: tests/test4.c core
	$(CCOMP) -I. -D$(TYPE) tests/test4.c -o tests/bin/test4 -L. -l$(NAME) -lm

test5: tests/test5.c core
	$(CCOMP) -I. -D$(TYPE) tests/test5.c -o tests/bin/test5 -L. -l$(NAME) -lm

grid.o: src/grid.c include/grid.h
	$(CCOMP) -c $<

scatter.o: src/scatter.c include/scatter.h
	$(CCOMP) -c $<

gather.o: src/gather.c include/gather.h
	$(CCOMP) -c $<

subcpy.o: src/subcpy.c include/subcpy.h
	$(CCOMP) -c $<

overhead.o: src/overhead.c include/overhead.h
	$(CCOMP) -c $<

clean:
	rm -rf *.o lib$(NAME).a tests/bin/* 

snap:
	tar -cvzf ../$(NAME)_`date +%y%m%d%H%M%S`.tar.gz ../$(NAME)

