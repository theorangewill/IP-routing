compile = g++

program1 = emissor.cpp
program2 = roteador.cpp

cppsources = $(wildcard *.cpp)
cppobjects1 = $(program1:.cpp=)
cppobjects2 = $(program2:.cpp=)

all: #$(cppsources:.cpp=.o)
	$(compile) -o $(cppobjects1) $(program1)
	$(compile) -o $(cppobjects2) $(program2)

#%.o: %.cpp
#	$(compile) -o $@ $^

#$(cppobjects2): $(program2)
#	$(compile) -o $@ $^

clean:
	rm *.o
