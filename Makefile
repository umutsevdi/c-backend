# gcc router.c  -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -o router -lglib-2.0

# dependencies :
# * microhttpd
# * glib-2.0

# Directories
TYPE   ?= BUILD
#BUILD | TEST | RELEASE
SRCDIR := src
INCDIR := include
OBJDIR := obj
BUILDDIR := bin

# Compiler and flags
CC := gcc
# -g for debug
CFLAGS := -Wall -Wextra -I$(INCDIR) -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lmicrohttpd

ifeq ($(TYPE), RELEASE)
    CFLAGS += -O2
else
	CFLAGS += -g
endif

ifeq ($(TYPE), TEST)
	EXCLUDE := $(SRCDIR)/main.c
else
    EXCLUDE := $(SRCDIR)/test.c
endif

# Source files (add more if necessary)
SRCS      := $(filter-out $(EXCLUDE), $(wildcard $(SRCDIR)/*.c))
# Object files derived from source files
OBJS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

# Main build target
all: $(BUILDDIR)/build

# Rule to build the executable from object files
$(BUILDDIR)/build: $(OBJS)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $^ -o $@

# Rule to build object files from source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove object files and the output executable
clean:
	rm -rf $(OBJDIR) $(BUILDDIR)

# Phony targets to prevent conflicts with file names
.PHONY: all clean
