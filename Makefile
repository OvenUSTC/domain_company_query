dcq:main.o request.o
	g++ -o dcq -std=c++11 main.o request.o -lcurl -ljsoncpp -g
main.o:main.cpp request.h
	g++ -c -std=c++11 main.cpp request.h -lcurl -ljsoncpp -g
request.o:request.cpp
	g++ -c -std=c++11 request.cpp -lcurl -ljsoncpp -g 
clean:
	rm -rf *.o dcq *.gch