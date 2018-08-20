main.out:main.cpp RedisServer.o code.o log.o util.o wrap.o
	g++ -Wall -std=c++11 RedisServer.o code.o log.o util.o wrap.o main.cpp -o main.out
RedisServer.o:RedisServer.cpp code.o wrap.o
	g++ -c -Wall -std=c++11 code.o wrap.o RedisServer.cpp -o RedisServer.o
code.o:code.cpp
	g++ -c -Wall -std=c++11 code.cpp -o code.o
wrap.o:wrap.cpp
	g++ -c wrap.cpp -o wrap.o
log.o:log.cpp
	g++ -c -Wall -std=c++11 log.cpp -o log.o
util.o:util.cpp
	g++ -c -Wall -std=c++11 util.cpp -o util.o
