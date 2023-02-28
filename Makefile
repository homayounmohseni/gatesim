CC = g++
FLAGS = -std=c++17
INCDIR = -I .
OBJS = primitives.o main.o

main.out: $(OBJS)
	$(CC) -o $@ $(OBJS) $(FLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(FLAGS) $(INCDIR)


clean:
	rm -f *.out
	rm -f *.o
	rm -f compile_commands.json
