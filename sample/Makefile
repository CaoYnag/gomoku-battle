FLAGS=-static
ARGS=-lboost_thread -lpthread $(FLAGS)
OUTPUT=out
LD_BOOST=-lboost_thread -lpthread
LD_GRPC=-L/usr/local/lib -lgrpc -lgrpc++ -lgrpc++_reflection -ldl -L/usr/lib -lprotobuf -lprotoc
STATIC=-static

utils.o: utils.h utils.cpp
	g++ -c utils.h utils.cpp
server: utils.o server.cpp
	g++ utils.o server.cpp -o server ${ARGS}
client: utils.o client.cpp
	g++ utils.o client.cpp -o client ${ARGS}
msg: testmsg.cpp utils.o
	g++ utils.o testmsg.cpp -o msg
httptest: httptest.cpp
	g++ httptest.cpp -o httptest -lcurl -lboost_thread -lpthread
mcc: utils.o mcc.cpp
	g++ utils.o mcc.cpp -o mcc ${FLAGS}
mcs: utils.o mcs.cpp
	g++ utils.o mcs.cpp -o mcs ${FLAGS}

clean:
	rm *.o server client httptest mcc mcs 
all:  server client msg httptest mcc mcs 
main: server client httptest mcc mcs