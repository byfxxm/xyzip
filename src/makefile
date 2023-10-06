VPATH=.\..\xyzip
ifeq ($(MAKECMDGOALS),r)
	gcc_config=-std=c++20 -O3 -DNDEBUG
else
	gcc_config=-std=c++20 -O3
endif

d: clean xyzip_imp.o xyzip.o xyzip_tests.o
	g++ xyzip_tests.cpp -o xyzip_tests $(gcc_config)
	@echo debug

r: clean xyzip_imp.o xyzip.o xyzip_tests.o
	g++ xyzip_tests.cpp -o xyzip_tests $(gcc_config)
	@echo debug

xyzip_imp.o: xyzip_imp.cpp
	g++ ../xyzip/xyzip_imp.cpp -c -o xyzip_imp.o $(gcc_config)

xyzip.o: xyzip.cpp
	g++ ../xyzip/xyzip.cpp -c -o xyzip.o $(gcc_config)

xyzip_tests.o: xyzip_tests.cpp
	g++ xyzip_tests.cpp -c -o xyzip_tests.o $(gcc_config)

.PHONY: clean
clean:
	del *.exe
	del *.o