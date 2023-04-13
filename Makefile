COMP = gcc -c
LINK = gcc
OBJDIR = etc
OBJECTS = $(OBJDIR)/main.o

PROGRAM = PokeBuscador.out

.PHONY: all clean

all: $(PROGRAM)

clean: 
	rm $(PROGRAM)
	rm $(OBJECTS)
	rm -r ./$(OBJDIR)

$(PROGRAM): $(OBJDIR) $(OBJECTS)
	$(LINK) $(OBJECTS) -o $(PROGRAM) -ansi

$(OBJDIR)/main.o: main.c
	$(COMP) main.c -o $(OBJDIR)/main.o -ansi

$(OBJDIR):
	mkdir $(OBJDIR)
