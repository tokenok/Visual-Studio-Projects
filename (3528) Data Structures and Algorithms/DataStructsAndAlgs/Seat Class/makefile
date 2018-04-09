CXX = g++

CC = g++


FLAGS = -std=c++11

all: 		seatdemo

firstclassseat.o: firstclassseat.cpp
	$(CXX) $(FLAGS) -c firstclassseat.cpp

economyplusseat.o: economyplusseat.cpp
	$(CXX) $(FLAGS) -c economyplusseat.cpp

economyseat.o: economyseat.cpp
	$(CXX) $(FLAGS) -c economyseat.cpp

seat.o: seat.cpp
	$(CXX) $(FLAGS) -c seat.cpp

main.o: main.cpp
	$(CXX) $(FLAGS) -c main.cpp

seatdemo: firstclassseat.o economyplusseat.o economyseat.o seat.o main.o
	$(CXX) $(FLAGS) firstclassseat.o economyplusseat.o economyseat.o seat.o main.o -o seatdemo



clobber:
	rm -f *.o *.exe core seatdemo