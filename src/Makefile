SHELL = /bin/sh

CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wpedantic -std=c11
LDFLAGS = -lpthread -ldl -lncurses
ADDFLAG = -D_GNU_SOURCE

#Intall dir.
EXEC_PREFIX = ./game

# Other directories.
SRCDIR_TETRIS = ./brick_game/tetris
SRCDIR_GUI = ./gui/cli
BUILDDIR = ./build
LIBDIR = $(EXEC_PREFIX)/gamefiles
STATEDIR_GCOV = $(BUILDDIR)/gcov_report

# Source files
SRC_TETRISCORE = $(SRCDIR_TETRIS)/tetris.c
SRC_GUI = $(SRCDIR_GUI)/gui.c
SRC_TEXI = ./dvi.texi
SRC_TEST = ./test.c

# Output files
TETRIS_LIB = $(LIBDIR)/tetris
GUI_EXECUTABLE = $(EXEC_PREFIX)/brick_game
GCOV_EXECUTABLE = $(STATEDIR_GCOV)/test_gcov
DVI_OUT = $(EXEC_PREFIX)/desc.dvi
TEST_OUT = $(BUILDDIR)/test.o
TEST_EXECUTABLE = $(BUILDDIR)/test_run

#Distribution variables
PORTNAME = brick_game
PORTVERSION = 0.0.9
DISTNAME = $(PORTNAME)_$(PORTVERSION)-stilwool
DISTFILES = brick_game gui headers scrshts dvi.texi README.MD Makefile

OS = $(shell uname -s)
ifeq ($(OS), Linux)
	CHECK_FLAGS = -pthread -lcheck_pic -lrt -lm -lsubunit
else
	CHECK_FLAGS = -lcheck
endif

SRC_C_FILES := $(shell find $(SRCDIR_TETRIS) -name "*.c")

all: install

create_dir:
	[ -d $(EXEC_PREFIX) ] || mkdir $(EXEC_PREFIX)
	[ -d $(LIBDIR) ] || mkdir $(LIBDIR)
	[ -d $(BUILDDIR) ] || mkdir $(BUILDDIR)

lib_build:
	$(CC) $(CFLAGS) -c -fPIC $(ADDFLAG) $(SRC_TETRISCORE) -o $(BUILDDIR)/tetris.o
	$(CC) $(CFLAGS) $(LDFLAGS) -shared $(BUILDDIR)/tetris.o -o $(TETRIS_LIB)

gui_build:
	$(CC) $(CFLAGS) $(SRC_GUI) $(ADDFLAG) $(LDFLAGS) -o $(GUI_EXECUTABLE)

clean:
	rm -rf $(BUILDDIR)

test:
	[ -d $(BUILDDIR) ] || mkdir $(BUILDDIR)
	$(CC) $(CFLAGS) $(ADDFLAG) -I./headers -c $(SRC_TEST) -o $(TEST_OUT)
	$(CC) $(CFLAGS) $(ADDFLAG) test.o $(SRC_TETRISCORE) -I./headers $(CHECK_FLAGS) -o $(TEST_EXECUTABLE)

gcov_report:
	[ -d $(BUILDDIR) ] || mkdir $(BUILDDIR)
	[ -d $(STATEDIR_GCOV) ] || mkdir $(STATEDIR_GCOV)
	$(CC) $(C_FLAGS) --coverage $(SRC_TEST) $(SRC_C_FILES) ${CHECK_FLAGS} -o $(GCOV_EXECUTABLE)
	$(GCOV_EXECUTABLE)
	lcov -t "Report" -c -d $(STATEDIR_GCOV) --output-file $(STATEDIR_GCOV)/coverage.info
	genhtml $(STATEDIR_GCOV)/coverage.info --output-directory $(STATEDIR_GCOV)/report/

gcov_report_open: gcov_report
	open $(STATEDIR_GCOV)/report/index.html

dvi:
	[ -d $(EXEC_PREFIX) ] || mkdir $(EXEC_PREFIX)
	texi2dvi $(SRC_TEXI) -o $(DVI_OUT)
	rm dvi.aux
	rm dvi.log
	rm dvi.toc

dvi_pdf:
	[ -d $(EXEC_PREFIX) ] || mkdir $(EXEC_PREFIX)
	texi2pdf $(SRC_TEXI) -p -o $(EXEC_PREFIX)/desc.pdf
	rm dvi.aux
	rm dvi.log
	rm dvi.toc

install: create_dir lib_build gui_build dvi clean

uninstall:
	rm -rf $(EXEC_PREFIX)

dist: 
	[ -d $(DISTNAME) ] || mkdir $(DISTNAME)
	$(foreach var,$(DISTFILES), cp -R $(var) $(DISTNAME);)
	tar -czvf $(DISTNAME).tar.gz $(DISTNAME)
	rm -rf $(DISTNAME)

.PHONY: test gcov_report dist