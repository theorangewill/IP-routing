compile = g++

program1 = emissor.cpp
program2 = roteador.cpp

cppsources = $(wildcard *.cpp)
cppobjects1 = $(program1:.cpp=)
cppobjects2 = $(program2:.cpp=)

all:
	$(compile) -o $(cppobjects1) $(program1)
	$(compile) -o $(cppobjects2) $(program2)

clean:
	rm emissor roteador
