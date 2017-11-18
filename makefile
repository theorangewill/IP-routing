compile = g++

program = emissor

csources = $(program).cpp

cobjects = $(csources:.cpp=.o)


$(program): $(cobjects)
	$(compile) -std=c++11 -o $(program) $(cobjects)

clean:
	rm *.o
