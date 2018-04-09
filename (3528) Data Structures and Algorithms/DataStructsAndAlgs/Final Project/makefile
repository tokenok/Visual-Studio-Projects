CXX = g++

CC = g++


FLAGS = -std=c++11

all: 		finalproj

finalproj: firstclassseat.o economyplusseat.o economyseat.o seat.o flight.o passenger.o plane.o main.o
	$(CXX) $(FLAGS) firstclassseat.o economyplusseat.o economyseat.o seat.o flight.o passenger.o plane.o main.o -o finalproj

firstclassseat.o: firstclassseat.cpp
	$(CXX) $(FLAGS) -c firstclassseat.cpp

economyplusseat.o: economyplusseat.cpp
	$(CXX) $(FLAGS) -c economyplusseat.cpp

economyseat.o: economyseat.cpp
	$(CXX) $(FLAGS) -c economyseat.cpp

seat.o: seat.cpp
	$(CXX) $(FLAGS) -c seat.cpp

flight.o: flight.cpp
	$(CXX) $(FLAGS) -c flight.cpp

passenger.o: passenger.cpp
	$(CXX) $(FLAGS) -c passenger.cpp

plane.o: plane.cpp
	$(CXX) $(FLAGS) -c plane.cpp

main.o: main.cpp
	$(CXX) $(FLAGS) -c main.cpp


clobber:
	rm -f *.o *.exe core finalproj