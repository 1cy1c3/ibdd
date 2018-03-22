CPP		= $(wildcard *.cpp)
OBJECT	= $(addsuffix .o, $(basename $(CPP) ) )
PROG	= ibdd_test
OUT		= /bin/echo
CC		= g++ -std=c++11
FLAGS	= -g -Wall

$(PROG): $(OBJECT)
	@$(OUT) "- Linking $@"
	@$(CC) -o $@ $(FLAGS) $(OBJECT)
%.o: %.cpp
	@$(OUT) "- Compiling $<"
	@$(CC) $(FLAGS) -c -o $@ $<
clean:
	@rm -f $(OBJECT) $(PROG)
