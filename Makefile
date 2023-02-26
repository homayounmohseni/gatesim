a.out: main.cpp
	g++ -std=c++17 main.cpp

clean:
	rm -f a.out
