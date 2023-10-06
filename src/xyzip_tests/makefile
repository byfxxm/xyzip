VPATH=.\..\xyzip
ifeq ($(MAKECMDGOALS),r)
	gcc_config=-std=c++20 -O3 -DNDEBUG
	show_msg=release
else
	gcc_config=-std=c++20 -O3
	show_msg=debug
endif

d: clean xyzip_imp.o xyzip.o
	g++ xyzip_imp.o xyzip.o xyzip_tests.cpp -o tests $(gcc_config)
	@echo $(show_msg)

r: d

xyzip_imp.o: xyzip_imp.cpp
	g++ ../xyzip/xyzip_imp.cpp -c -o xyzip_imp.o $(gcc_config)

xyzip.o: xyzip.cpp
	g++ ../xyzip/xyzip.cpp -c -o xyzip.o $(gcc_config)

.PHONY: clean
clean:
	del *.exe
	del *.o