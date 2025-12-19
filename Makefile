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
		  $(OBJDIR)/lisp_built_in_subrs.o \
		  $(OBJDIR)/genericlisp.o


### Build Rules

.SUFFIXES: .c .o

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<


### Primary Targets

all: $(OBJDIR) genericlisp


clean:
	$(RM) genericlisp
	$(RM) $(OBJECTS)


genericlisp: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)


### Utility Targets

.PHONY: $(OBJDIR)
$(OBJDIR):
	@mkdir -p $(OBJDIR)


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
