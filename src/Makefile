all: main.out

main.out: main.o RedisServer.o code.o log.o util.o wrap.o
	g++ -Wall -std=c++11 -g $^ -o $@

wrap.o: wrap.cpp
	g++ -c wrap.cpp

clean:
	-rm main.out *.o *.d

.PHONY: clean

sources = main.cpp RedisServer.cpp code.cpp log.cpp util.cpp 

include $(sources:.cpp=.d)

%.d: %.cpp
	set -e; \
	rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

CXXFLAGS = -Wall -std=c++11 -c
