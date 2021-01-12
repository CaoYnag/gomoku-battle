FLAGS=
ARGS=-lboost_thread -lpthread $(FLAGS)
OUTPUT=out
LD_BOOST=-lboost_thread -lpthread
STATIC=-static

utils.o: utils.h utils.cpp
	g++ -c utils.h utils.cpp
server: utils.o server.cpp
	g++ utils.o server.cpp -o server ${ARGS} ${STATIC}
client: utils.o client.cpp
	g++ utils.o client.cpp -o client ${ARGS} ${STATIC}

serverd: utils.o server.cpp
	g++ utils.o server.cpp -o serverd ${ARGS}
clientd: utils.o client.cpp
	g++ utils.o client.cpp -o clientd ${ARGS}

clean:
	rm *.o server client
debug: serverd clientd
all:  server client
main: server client