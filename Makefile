dcq:main.o request.o dcq_conf.o dcq_get_info.o dcq_domain_list.o dcq_log.o dcq_db.o
	g++ -o dcq -std=c++11 main.o request.o dcq_conf.o dcq_get_info.o dcq_domain_list.o dcq_log.o dcq_db.o -lcurl -ljsoncpp -lglog -ltalloc -ltdb -g
main.o:main.cpp request.h dcq_conf.h
	g++ -c -std=c++11 main.cpp request.h dcq_conf.h -g
request.o:request.cpp
	g++ -c -std=c++11 request.cpp -g
dcq_conf.o:dcq_conf.cpp
	g++ -c -std=c++11 dcq_conf.cpp -g
dcq_get_info.o:dcq_get_info.cpp
	g++ -c -std=c++11 dcq_get_info.cpp -g
dcq_domain_list.o:dcq_domain_list.cpp
	g++ -c -std=c++11 dcq_domain_list.cpp -g
dcq_log.o:dcq_log.cpp
	g++ -c -std=c++11 dcq_log.cpp -g
dcq_db.o:dcq_db.cpp
	g++ -c -std=c++11 dcq_db.cpp -g
clean:
	rm -rf *.o dcq *.gch