PROJECT = lssd
PLATFORM := $(shell uname -s)

CC = gcc
RM = rm -f
GDB = gdb
MKDIR = mkdir -p

SRCDIR = src
INCDIR = include
BUILDDIR := build
TARGET = $(BUILDDIR)/bin/$(PROJECT)

ifeq ($(PLATFORM), Linux)
	SOURCES := $(SRCDIR)/linux.c
else ifeq ($(PLATFORM), NetBSD)
	SOURCES := $(SRCDIR)/netbsd.c
endif
SRCEXT := c
SOURCES += $(SRCDIR)/main.c $(SRCDIR)/device.c
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/obj/%,$(SOURCES:.$(SRCEXT)=.o))

CFLAGS = -Wall -I $(INCDIR)
ifeq ($(PLATFORM), Linux)
	LDFLAGS = -lblkid
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILDDIR)/obj/%.o: $(SRCDIR)/%.$(SRCEXT)
	@$(MKDIR) $(BUILDDIR)/bin
	@$(MKDIR) $(BUILDDIR)/obj
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	@./$(TARGET)

debug: CFLAGS += -g3 -DDEBUG
debug: clean $(TARGET)
	$(GDB) $(TARGET)

memcheck: CFLAGS += -g3 -DDEBUG -DMEMCHECK
memcheck: clean $(TARGET)
	valgrind --tool=memcheck --leak-check=yes --show-leak-kinds=all --track-origins=yes --log-file=valgrind.log ./$(TARGET)
	cat valgrind.log

clean:
	$(RM) -r $(BUILDDIR)
	$(RM) valgrind.log

