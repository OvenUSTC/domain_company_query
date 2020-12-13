unit_test_libdcq:unit_test_libdcq.o libdcq.so
	g++ -o unit_test_libdcq unit_test_libdcq.o -lcurl -ljsoncpp -lglog -ltdb -lgtest -lpthread -fprofile-arcs -ftest-coverage -L. -ldcq 
unit_test_libdcq.o:unit_test_libdcq.cpp
	g++ -fPIC -Wall -g -c unit_test_libdcq.cpp -fprofile-arcs -ftest-coverage
libdcq.so:libdcq.o request.o dcq_conf.o dcq_get_info.o dcq_domain_list.o dcq_log.o dcq_db.o
	g++ -shared -o libdcq.so -std=c++11 request.o dcq_conf.o dcq_get_info.o dcq_domain_list.o dcq_log.o dcq_db.o libdcq.o -lcurl -ljsoncpp -lglog -ltdb -fprofile-arcs -ftest-coverage -g
libdcq.o:libdcq.cpp
	g++ -fPIC -Wall -g -c libdcq.cpp -fprofile-arcs -ftest-coverage
dcq:main.o request.o dcq_conf.o dcq_get_info.o dcq_domain_list.o dcq_log.o dcq_db.o
	g++ -fPIC -Wall -o dcq -std=c++11 main.o request.o dcq_conf.o dcq_get_info.o dcq_domain_list.o dcq_log.o dcq_db.o -lcurl -ljsoncpp -lglog -ltdb -g -fprofile-arcs -ftest-coverage
main.o:main.cpp request.h dcq_conf.h
	g++ -c -fPIC -Wall -std=c++11 main.cpp request.h dcq_conf.h -g -fprofile-arcs -ftest-coverage
request.o:request.cpp
	g++ -c -fPIC -Wall -std=c++11 request.cpp -g -fprofile-arcs -ftest-coverage
dcq_conf.o:dcq_conf.cpp
	g++ -c -fPIC -Wall -std=c++11 dcq_conf.cpp -g -fprofile-arcs -ftest-coverage
dcq_get_info.o:dcq_get_info.cpp
	g++ -c -fPIC -Wall -std=c++11 dcq_get_info.cpp -g -fprofile-arcs -ftest-coverage
dcq_domain_list.o:dcq_domain_list.cpp
	g++ -c -fPIC -Wall -std=c++11 dcq_domain_list.cpp -g -fprofile-arcs -ftest-coverage
dcq_log.o:dcq_log.cpp
	g++ -c -fPIC -Wall -std=c++11 dcq_log.cpp -g -fprofile-arcs -ftest-coverage
dcq_db.o:dcq_db.cpp
	g++ -c -fPIC -Wall -std=c++11 dcq_db.cpp -g -fprofile-arcs -ftest-coverage
clean:
	rm -rf *.o dcq *.gch *.so *.gcno *.info *gcda unit_test_libdcq
install:
	sudo cp ./libdcq.so /usr/lib/x86_64-linux-gnu/libdcq.so
	lcov -d ./ -t 'unit_test_libdcq' -o 'test.info' -b . -c