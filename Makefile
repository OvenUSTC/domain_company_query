dcq:main.o request.o dcq_conf.o dcq_get_info.o
	g++ -o dcq -std=c++11 main.o request.o dcq_conf.o dcq_get_info.o -lcurl -ljsoncpp -g
main.o:main.cpp request.h dcq_conf.h
	g++ -c -std=c++11 main.cpp request.h dcq_conf.h
request.o:request.cpp
	g++ -c -std=c++11 request.cpp
dcq_conf.o:dcq_conf.cpp
	g++ -c -std=c++11 dcq_conf.cpp
dcq_get_info.o:dcq_get_info.cpp
	g++ -c -std=c++11 dcq_get_info.cpp
clean:
	rm -rf *.o dcq *.gch