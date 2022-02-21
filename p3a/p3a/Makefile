#
# 'make'        build executable file 'main'
# 'make clean'  removes all .o and executable files
#

# define the C compiler to use
CC = gcc

# define any compile-time flags
CFLAGS	:= -Wall -Wextra -g -std=c99 -pedantic -ggdb -D_XOPEN_SOURCE=700 
# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS =

# define output directory
OUTPUT	:= output

# define source directory
SRC		:= src

# define include directory
INCLUDE	:= include

# define lib directory
LIB		:= lib

ifeq ($(OS),Windows_NT)
MAIN	:= wis-untar.exe
SOURCEDIRS	:= $(SRC)
INCLUDEDIRS	:= $(INCLUDE)
LIBDIRS		:= $(LIB)
FIXPATH = $(subst /,\,$1)
RM			:= del /q /f
MD	:= mkdir
else
MAIN1	:= wis-grep
MAIN2	:= wis-tar
MAIN3	:= wis-untar
SOURCEDIRS	:= $(shell find $(SRC) -type d)
INCLUDEDIRS	:= $(shell find $(INCLUDE) -type d)
LIBDIRS		:= $(shell find $(LIB) -type d)
FIXPATH = $1
RM = rm -f
MD	:= mkdir -p
endif

# define any directories containing header files other than /usr/include
INCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))

# define the C libs
LIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))

# define the C source files
SOURCES		:= $(wildcard $(patsubst %,%/*.c, $(SOURCEDIRS)))

# define the C object files 
OBJECTS		:= $(SOURCES:.c=.o)

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

OUTPUTMAIN1	:= $(call FIXPATH,$(OUTPUT)/$(MAIN1))
OUTPUTMAIN2	:= $(call FIXPATH,$(OUTPUT)/$(MAIN2))
OUTPUTMAIN3	:= $(call FIXPATH,$(OUTPUT)/$(MAIN2))

all: $(OUTPUT) $(MAIN1) $(MAIN2) $(MAIN3)
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)

$(MAIN1): wis-grep.o fileutil.o
	$(CC) $(CFLAGS) -o $(OUTPUTMAIN1) $(SOURCEDIRS)/wis-grep.o $(SOURCEDIRS)/fileutil.o
$(MAIN2): wis-tar.o fileutil.o
	$(CC) $(CFLAGS) -o $(OUTPUTMAIN2) $(SOURCEDIRS)/wis-tar.o $(SOURCEDIRS)/fileutil.o
$(MAIN3): wis-untar.o fileutil.o
	$(CC) $(CFLAGS) -o $(OUTPUTMAIN3) $(SOURCEDIRS)/wis-untar.o $(SOURCEDIRS)/fileutil.o

fileutil.o:
	gcc -Wall -Wextra -g -std=c99 -pedantic -ggdb -D_XOPEN_SOURCE=700 -Iinclude -c src/fileutil.c  -o src/fileutil.o

wis-grep.o:
	gcc -Wall -Wextra -g -std=c99 -pedantic -ggdb -D_XOPEN_SOURCE=700 -Iinclude -c src/wis-grep.c  -o src/wis-grep.o

wis-tar.o:
	gcc -Wall -Wextra -g -std=c99 -pedantic -ggdb -D_XOPEN_SOURCE=700 -Iinclude -c src/wis-tar.c  -o src/wis-tar.o

wis-untar.o:
	gcc -Wall -Wextra -g -std=c99 -pedantic -ggdb -D_XOPEN_SOURCE=700 -Iinclude -c src/wis-untar.c  -o src/wis-untar.o
# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

.PHONY: clean
clean:
	$(RM) $(OUTPUTMAIN1)
	$(RM) $(OUTPUTMAIN2)
	$(RM) $(OUTPUTMAIN3)
	$(RM) $(call FIXPATH,$(OBJECTS))
	rm -f tarfile.txt
	@echo Cleanup complete!

run: all
	./$(OUTPUTMAIN1)
	./$(OUTPUTMAIN2)
	./$(OUTPUTMAIN3)
	@echo Executing 'run: all' complete!
