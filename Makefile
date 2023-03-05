CC = g++
FLAGS = -std=c++17 -O1
INCDIR = -I .
OBJS = primitives.o main.o
DEPS = primitives.hpp

main.out: $(OBJS) $(DEPS)
	$(CC) $(FLAGS) -o $@ $(OBJS)

%.o: %.cpp $(DEPS)
	$(CC) $(FLAGS) $(INCDIR) -c -o $@ $<
clean:
	rm -f *.out
	rm -f *.o
	rm -f compile_commands.json
