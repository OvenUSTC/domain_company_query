libdcq.so:libdcq.o request.o dcq_conf.o dcq_get_info.o dcq_domain_list.o dcq_log.o dcq_db.o
	g++ -shared -o libdcq.so -std=c++11 request.o dcq_conf.o dcq_get_info.o dcq_domain_list.o dcq_log.o dcq_db.o -lcurl -ljsoncpp -lglog -ltdb -g
libdcq.o:libdcq.cpp
	g++ -fPIC -Wall -g -c libdcq.cpp
dcq:main.o request.o dcq_conf.o dcq_get_info.o dcq_domain_list.o dcq_log.o dcq_db.o
	g++ -o -fPIC -Wall dcq -std=c++11 main.o request.o dcq_conf.o dcq_get_info.o dcq_domain_list.o dcq_log.o dcq_db.o -lcurl -ljsoncpp -lglog -ltdb -g
main.o:main.cpp request.h dcq_conf.h
	g++ -c -fPIC -Wall -std=c++11 main.cpp request.h dcq_conf.h -g
request.o:request.cpp
	g++ -c -fPIC -Wall -std=c++11 request.cpp -g
dcq_conf.o:dcq_conf.cpp
	g++ -c -fPIC -Wall -std=c++11 dcq_conf.cpp -g
dcq_get_info.o:dcq_get_info.cpp
	g++ -c -fPIC -Wall -std=c++11 dcq_get_info.cpp -g
dcq_domain_list.o:dcq_domain_list.cpp
	g++ -c -fPIC -Wall -std=c++11 dcq_domain_list.cpp -g
dcq_log.o:dcq_log.cpp
	g++ -c -fPIC -Wall -std=c++11 dcq_log.cpp -g
dcq_db.o:dcq_db.cpp
	g++ -c -fPIC -Wall -std=c++11 dcq_db.cpp -g
clean:
	rm -rf *.o dcq *.gch *.so
