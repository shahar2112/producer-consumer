
# make clean/all CONFIG=Debug/Release ARCH=linux64/raspi ECHO=0/1

# the parrent dir of "arch" dir
ROOT_DIR = ../..

#PROJ_SRCS = $(shell xmlstarlet select -t -v //Unit/@filename xx.cbp | grep -e '\.c')
PROJ_SRCS = prod_cons.c

# list of directories containing header files
PROJ_INC_DIR = $(ROOT_DIR)/ds/linkedList $(ROOT_DIR)/ds/queue $(ROOT_DIR)/ds/cbuf

# the executable file
PROJ_OUT = prod_cons.out

# list of compiler flags (prepended with "-" sign)
#PROJ_CFLAGS = -Wunused-but-set-variable
PROJ_CFLAGS = -D_XOPEN_SOURCE=700

#PROJ_MACROS = _BSD_SOURCE
PROJ_MACROS =

# additional object files (created by other projects)
PROJ_ADD_OBJS = linked_list.o queue.o cbuffer.o

# additional libraries
PROJ_LIBS = pthread

# linker flags
PROJ_LDFLAGS =

# list of projects (makefiles) this project depends on
PROJ_DEPENDS  = $(ROOT_DIR)/ds/linkedList/Makefile $(ROOT_DIR)/ds/queue/Makefile $(ROOT_DIR)/ds/cbuf/Makefile

include $(ROOT_DIR)/Make/Project.mak
