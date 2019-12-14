CC=gcc
MKDIR=mkdir

LIB=libuobject.so
DESTDIR=/usr
INCDIR=$(DESTDIR)/include/uobject
LIBDIR=$(DESTDIR)/lib
LDFLAGS=-fPIC
CFLAGS=-fPIC $(WARN_FLAGS) -DBUILDING_UOBJ_CORE $(EXTRA_CFLAGS)
WARN_FLAGS=-Wall -Wextra -pedantic
EXTRA_CFLAGS=
INSTALL=install

SRC=$(shell find src -name '*.c')
HDR=$(shell find src -name '*.h')

OBJ=$(patsubst src/%,obj/%,$(SRC:.c=.o))
IHDR=$(addprefix $(INCDIR)/,$(patsubst src/%,%,$(HDR)))

.PHONY: clean install
.SILENT:

$(LIB): $(OBJ)
	echo linking $(LIB)
	$(LD) -shared -soname $(LIB) $(OBJ) -o $(LIB)

obj/%.o: src/%.c
	echo compiling $(patsubst src/%,%,$<)
	$(MKDIR) -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	echo cleaning
	$(RM) -r $(LIB) obj

install: $(LIB) $(IHDR)
	echo installing $(LIBDIR)/$(LIB)
	$(INSTALL) -m755 $(LIB) $(LIBDIR)/$(LIB)

$(INCDIR)/%.h: src/%.h
	echo installing $@
	$(INSTALL) -d -m755 $(dir $@)
	$(INSTALL) -m644 $< $@
