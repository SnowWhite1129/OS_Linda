all:
	g++ linda.cpp main.cpp -std=c++14 -fopenmp -o linda
dbg:
	g++ linda.cpp main.cpp -std=c++14 -fopenmp -o linda -D dbg
    
