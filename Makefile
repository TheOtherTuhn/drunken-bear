 Declaration of variables
 CC = cc 
 CC_FLAGS = -Wall -Werror -std=c99 -D_GNU_SOURCE
 LDADD?=-pthread

 # File names
 EXEC = run
 SOURCES = $(wildcard *.c)
 OBJECTS = $(SOURCES:.c=.o)

 # Main target
 $(EXEC): $(OBJECTS)
 $(CC) $(OBJECTS) -o $(EXEC)

 # To obtain object files
 %.o: %.c
 $(CC) -c $(CC_FLAGS) $< -o $@

 # To remove generated files
 clean:
 rm -f $(EXEC) $(OBJECTS)
