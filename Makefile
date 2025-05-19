BINARY=bin/pain
SOURCEDIR=source
INCDIRS=include
LIBDIR=lib


CC=gcc
OPT=-O0 

DEPFLAGS=-MP -MD 

CFLAGS=-Wall -Wextra -g $(foreach D, $(INCDIRS), -I$(D)) $(OPT) $(DEPFLAGS) 


CFILES=$(foreach D, $(SOURCEDIR), $(wildcard $(D)/*.c))
LDLIBS =  -lglfw3dll -lopengl32 -lmingw32

OBJECTS=$(patsubst %.c,%.o,$(CFILES))
DEPFILES=$(patsubst %.c,%.d,$(CFILES))

 ifdef ComSpec
    RM=del /F /Q
else
    RM=rm -f
endif


all: $(BINARY)

$(BINARY): $(OBJECTS)
		$(CC) -L ./lib -o $@ $^ $(LDLIBS)

%.o:%.c
		$(CC) $(CFLAGS) -c -o $@ $< 

run:
	./$(BINARY)

clean:
	$(RM) $(BINARY) $(OBJECTS) $(DEPFILES) 


-include $(DEPFILES)
