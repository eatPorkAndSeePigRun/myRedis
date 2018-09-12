all: main.out

main.out: main.o redisServer.o log.o wrap.o
	g++ -Wall -std=c++11 -g $^ -o $@

clean:
	-rm main.out *.o *.d

.PHONY: clean

sources = main.cpp redisServer.cpp log.cpp wrap.cpp 

include $(sources:.cpp=.d)

%.d: %.cpp
	set -e; \
	rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

CXXFLAGS = -Wall -std=c++11 -c -g
