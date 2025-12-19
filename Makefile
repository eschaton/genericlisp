#
#	File:		Makefile
#
#	Copyright:	© 2025 Christopher M. Hanson. All rights reserved.
#				See file COPYING for details.
#

CONFIGURATION = Release

CC = clang
RM = rm -f

OBJDIR = obj
SRCDIR = src
TSTDIR = tests

CFLAGS_Debug   = -O0 -g
CFLAGS_Release = -Os
CFLAGS += $(CFLAGS_$(CONFIGURATION))

PREPROCESSOR_MACROS = -DLISP_USE_STDLIB=1

CPPFLAGS_Debug	 = -DDEBUG=1
CPPFLAGS_Release = -DNDEBUG=1
CPPFLAGS += -I$(SRCDIR) \
			$(CPPFLAGS_$(CONFIGURATION)) \
			$(PREPROCESSOR_MACROS)

OBJECTS = \
		  $(OBJDIR)/lisp_types.o \
		  $(OBJDIR)/lisp_utilities.o \
		  $(OBJDIR)/lisp_atom.o \
		  $(OBJDIR)/lisp_cell.o \
		  $(OBJDIR)/lisp_environment.o \
		  $(OBJDIR)/lisp_evaluation.o \
		  $(OBJDIR)/lisp_fixnum.o \
		  $(OBJDIR)/lisp_interior.o \
		  $(OBJDIR)/lisp_memory.o \
		  $(OBJDIR)/lisp_plist.o \
		  $(OBJDIR)/lisp_printing.o \
		  $(OBJDIR)/lisp_reading.o \
		  $(OBJDIR)/lisp_stream.o \
		  $(OBJDIR)/lisp_string.o \
		  $(OBJDIR)/lisp_struct.o \
		  $(OBJDIR)/lisp_subr.o \
		  $(OBJDIR)/lisp_vector.o \
		  $(OBJDIR)/lisp_built_in_sforms.o \
		  $(OBJDIR)/lisp_built_in_streams.o \
		  $(OBJDIR)/lisp_built_in_subrs.o

CHECK_PREFIX = /opt/local
CHECK_INCLUDE_PATH = $(CHECK_PREFIX)/include
CHECK_LIB_PATH = $(CHECK_PREFIX)/lib

CHECK_CFLAGS = -I$(CHECK_INCLUDE_PATH)
CHECK_LDFLAGS = -L$(CHECK_LIB_PATH) -lcheck

TESTS = \
		do_check_atom \
		do_check_cell \
		do_check_char \
		do_check_environment \
		do_check_evaluation \
		do_check_fixnum \
		do_check_plist \
		do_check_stream \
		do_check_string


### Build Rules

.SUFFIXES: .c .o

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<


### Primary Targets

all: $(OBJDIR) genericlisp


clean:
	$(RM) genericlisp $(OBJDIR)/genericlisp.o
	$(RM) $(OBJECTS)
	$(RM) $(OBJDIR)/tests_support.o
	$(RM) $(TESTS)
	$(RM) -r *.dSYM


check: $(TESTS)
	./do_check_atom
	./do_check_cell
	./do_check_char
	./do_check_environment
	./do_check_evaluation
	./do_check_fixnum
	./do_check_plist
	./do_check_stream
	./do_check_string


genericlisp: $(OBJECTS) $(OBJDIR)/genericlisp.o
	$(CC) -o $@ $(OBJECTS) $(OBJDIR)/genericlisp.o


### Test Targets

do_check_atom: $(TSTDIR)/check_atom.c $(OBJDIR)/tests_support.o $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) \
		$(CHECK_CFLAGS) $(CHECK_LDFLAGS) \
		$(OBJDIR)/tests_support.o $(OBJECTS) \
		-o $@ $<

do_check_cell: $(TSTDIR)/check_cell.c $(OBJDIR)/tests_support.o $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) \
		$(CHECK_CFLAGS) $(CHECK_LDFLAGS) \
		$(OBJDIR)/tests_support.o $(OBJECTS) \
		-o $@ $<

do_check_char: $(TSTDIR)/check_char.c $(OBJDIR)/tests_support.o $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) \
		$(CHECK_CFLAGS) $(CHECK_LDFLAGS) \
		$(OBJDIR)/tests_support.o $(OBJECTS) \
		-o $@ $<

do_check_environment: $(TSTDIR)/check_environment.c $(OBJDIR)/tests_support.o $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) \
		$(CHECK_CFLAGS) $(CHECK_LDFLAGS) \
		$(OBJDIR)/tests_support.o $(OBJECTS) \
		-o $@ $<

do_check_evaluation: $(TSTDIR)/check_evaluation.c $(OBJDIR)/tests_support.o $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) \
		$(CHECK_CFLAGS) $(CHECK_LDFLAGS) \
		$(OBJDIR)/tests_support.o $(OBJECTS) \
		-o $@ $<

do_check_fixnum: $(TSTDIR)/check_fixnum.c $(OBJDIR)/tests_support.o $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) \
		$(CHECK_CFLAGS) $(CHECK_LDFLAGS) \
		$(OBJDIR)/tests_support.o $(OBJECTS) \
		-o $@ $<

do_check_plist: $(TSTDIR)/check_plist.c $(OBJDIR)/tests_support.o $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) \
		$(CHECK_CFLAGS) $(CHECK_LDFLAGS) \
		$(OBJDIR)/tests_support.o $(OBJECTS) \
		-o $@ $<

do_check_stream: $(TSTDIR)/check_stream.c $(OBJDIR)/tests_support.o $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) \
		$(CHECK_CFLAGS) $(CHECK_LDFLAGS) \
		$(OBJDIR)/tests_support.o $(OBJECTS) \
		-o $@ $<

do_check_string: $(TSTDIR)/check_string.c $(OBJDIR)/tests_support.o $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) \
		$(CHECK_CFLAGS) $(CHECK_LDFLAGS) \
		$(OBJDIR)/tests_support.o $(OBJECTS) \
		-o $@ $<


### Utility Targets

.PHONY: $(OBJDIR)
$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(OBJDIR)/tests_support.o: $(TSTDIR)/tests_support.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(CHECK_CFLAGS) -c -o $@ $<


### File Dependencies

src/lisp_base.h:

src/lisp_atom.c: src/lisp_atom.h \
				 src/lisp_environment.h \
				 src/lisp_interior.h \
				 src/lisp_memory.h \
				 src/lisp_string.h

src/lisp_atom.h: src/lisp_types.h

src/lisp_built_in_sforms.c: src/lisp_built_in_sforms.h \
							src/lisp_atom.h \
							src/lisp_cell.h \
							src/lisp_environment.h \
							src/lisp_evaluation.h \
							src/lisp_memory.h \
							src/lisp_plist.h \
							src/lisp_subr.h

src/lisp_built_in_sforms.h: src/lisp_types.h

src/lisp_built_in_streams.c: src/lisp_built_in_streams.h \
							 src/lisp_atom.h \
							 src/lisp_environment.h \
							 src/lisp_interior.h \
							 src/lisp_string.h

src/lisp_built_in_streams.h: src/lisp_stream.h

src/lisp_built_in_subrs.c: src/lisp_built_in_subrs.h \
						   src/lisp_atom.h \
						   src/lisp_cell.h \
						   src/lisp_environment.h \
						   src/lisp_evaluation.h \
						   src/lisp_fixnum.h \
						   src/lisp_printing.h \
						   src/lisp_reading.h \
						   src/lisp_stream.h \
						   src/lisp_string.h \
						   src/lisp_subr.h

src/lisp_built_in_subrs.h: src/lisp_types.h

src/lisp_cell.c: src/lisp_cell.h \
				 src/lisp_environment.h \
				 src/lisp_memory.h \
				 src/lisp_printing.h \
				 src/lisp_string.h

src/lisp_cell.h: src/lisp_types.h

src/lisp_environment.c: src/lisp_environment.h \
						src/lisp_atom.h \
						src/lisp_built_in_subrs.h \
						src/lisp_cell.h \
						src/lisp_evaluation.h \
						src/lisp_memory.h \
						src/lisp_plist.h \
						src/lisp_stream.h \
						src/lisp_string.h \
						src/lisp_subr.h \
						src/lisp_built_in_streams.h

src/lisp_environment.h: src/lisp_types.h

src/lisp_evaluation.c: src/lisp_evaluation.h \
					   src/lisp_atom.h \
					   src/lisp_cell.h \
					   src/lisp_environment.h \
					   src/lisp_memory.h \
					   src/lisp_plist.h \
					   src/lisp_subr.h

src/lisp_evaluation.h: src/lisp_types.h

src/lisp_fixnum.c: src/lisp_fixnum.h \
				   src/lisp_printing.h \
				   src/lisp_string.h

src/lisp_fixnum.h: src/lisp_types.h

src/lisp_interior.h: src/lisp_types.h

src/lisp_interior.c: src/lisp_interior.h \
					 src/lisp_environment.h \
					 src/lisp_memory.h \
					 src/lisp_string.h

src/lisp_memory.c: src/lisp_memory.h \
				   src/lisp_atom.h \
				   src/lisp_string.h \
				   src/lisp_utilities.h

src/lisp_memory.h: src/lisp_types.h

src/lisp_plist.c: src/lisp_plist.h \
				  src/lisp_atom.h \
				  src/lisp_cell.h \
				  src/lisp_environment.h \
				  src/lisp_printing.h

src/lisp_plist.h: src/lisp_types.h

src/lisp_printing.c: src/lisp_printing.h \
					 src/lisp_atom.h \
					 src/lisp_cell.h \
					 src/lisp_environment.h \
					 src/lisp_fixnum.h \
					 src/lisp_interior.h \
					 src/lisp_memory.h \
					 src/lisp_stream.h \
					 src/lisp_string.h \
					 src/lisp_struct.h \
					 src/lisp_subr.h \
					 src/lisp_vector.h

src/lisp_printing.h: src/lisp_types.h

src/lisp_reading.c: src/lisp_reading.h \
					src/lisp_atom.h \
					src/lisp_cell.h \
					src/lisp_environment.h \
					src/lisp_evaluation.h \
					src/lisp_fixnum.h \
					src/lisp_stream.h \
					src/lisp_string.h

src/lisp_reading.h: src/lisp_types.h

src/lisp_stream.c: src/lisp_stream.h \
				   src/lisp_environment.h \
				   src/lisp_interior.h \
				   src/lisp_memory.h \
				   src/lisp_string.h

src/lisp_stream.h: src/lisp_types.h

src/lisp_string.c: src/lisp_string.h \
				   src/lisp_environment.h \
				   src/lisp_interior.h \
				   src/lisp_memory.h \
				   src/lisp_printing.h \
				   src/lisp_stream.h \
				   src/lisp_utilities.h

src/lisp_string.h: src/lisp_types.h

src/lisp_struct.c: src/lisp_struct.h \
				   src/lisp_environment.h \
				   src/lisp_memory.h \
				   src/lisp_string.h

src/lisp_struct.h: src/lisp_types.h

src/lisp_subr.c: src/lisp_subr.h \
				 src/lisp_environment.h \
				 src/lisp_memory.h \
				 src/lisp_printing.h \
				 src/lisp_string.h

src/lisp_subr.h: src/lisp_types.h

src/lisp_types.c: src/lisp_types.h \
				  src/lisp_atom.h \
				  src/lisp_cell.h \
				  src/lisp_environment.h \
				  src/lisp_fixnum.h \
				  src/lisp_interior.h \
				  src/lisp_stream.h \
				  src/lisp_string.h \
				  src/lisp_struct.h \
				  src/lisp_subr.h \
				  src/lisp_vector.h

src/lisp_types.h: src/lisp_base.h

src/lisp_utilities.c: src/lisp_utilities.h

src/lisp_utilities.h: src/lisp_types.h

src/lisp_vector.c: src/lisp_vector.h \
				   src/lisp_environment.h \
				   src/lisp_printing.h \
				   src/lisp_string.h

src/lisp_vector.h: src/lisp_types.h

src/genericlisp.c: src/genericlisp.h

src/genericlisp.h: src/lisp_base.h \
				   src/lisp_types.h \
				   src/lisp_utilities.h \
				   src/lisp_atom.h \
				   src/lisp_cell.h \
				   src/lisp_environment.h \
				   src/lisp_evaluation.h \
				   src/lisp_fixnum.h \
				   src/lisp_interior.h \
				   src/lisp_memory.h \
				   src/lisp_plist.h \
				   src/lisp_printing.h \
				   src/lisp_reading.h \
				   src/lisp_stream.h \
				   src/lisp_string.h \
				   src/lisp_struct.h \
				   src/lisp_subr.h \
				   src/lisp_vector.h


$(TSTDIR)/check_atom.c: $(SRCDIR)/genericlisp.h \
						$(TSTDIR)/tests_support.h

$(TSTDIR)/check_cell.c: $(SRCDIR)/genericlisp.h \
						$(SRCDIR)/lisp_built_in_sforms.h \
						$(TSTDIR)/tests_support.h

$(TSTDIR)/check_char.c: $(SRCDIR)/genericlisp.h \
						$(TSTDIR)/tests_support.h

$(TSTDIR)/check_environment.c: $(SRCDIR)/genericlisp.h \
							   $(TSTDIR)/tests_support.h

$(TSTDIR)check_evaluation.c: $(SRCDIR)/genericlisp.h \
							 $(SRCDIR)/lisp_built_in_sforms.h \
							 $(TSTDIR)/tests_support.h

$(TSTDIR)/check_fixnum.c: $(SRCDIR)/genericlisp.h \
						  $(TSTDIR)/tests_support.h

$(TSTDIR)/check_plist.c: $(SRCDIR)/genericlisp.h \
						 $(TSTDIR)/tests_support.h

$(TSTDIR)/check_stream.c: $(SRCDIR)/genericlisp.h \
						  $(TSTDIR)/tests_support.h

$(TSTDIR)/check_string.c: $(SRCDIR)/genericlisp.h \
						  $(TSTDIR)/tests_support.h

$(TSTDIR)/tests_support.c: $(TSTDIR)/tests_support.h \
						   $(SRCDIR)/genericlisp.h
