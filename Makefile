CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = 
INCLUDES = -Iinclude

SRCDIR = src
INCDIR = include

# Fichiers sources communs
COMMON_SRCS = $(SRCDIR)/serialization.c $(SRCDIR)/task_tree.c $(SRCDIR)/execution.c $(SRCDIR)/protocol.c
COMMON_OBJS = $(COMMON_SRCS:.c=.o)

# Fichiers sources du démon
ERRAID_SRCS = $(SRCDIR)/erraid.c $(COMMON_SRCS)
ERRAID_OBJS = $(ERRAID_SRCS:.c=.o)

# Fichiers sources du client
TADMOR_SRCS = $(SRCDIR)/tadmor.c $(COMMON_SRCS)
TADMOR_OBJS = $(TADMOR_SRCS:.c=.o)

# Cibles principales
all: erraid tadmor

erraid: $(ERRAID_OBJS)
	$(CC) $(LDFLAGS) -o erraid $(ERRAID_OBJS)

tadmor: $(TADMOR_OBJS)
	$(CC) $(LDFLAGS) -o tadmor $(TADMOR_OBJS)

# Règle générique pour compiler les fichiers .c
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Nettoyage
clean:
	rm -f $(SRCDIR)/*.o erraid tadmor

distclean: clean
	rm -rf /tmp/$$USER/erraid

.PHONY: all clean distclean

