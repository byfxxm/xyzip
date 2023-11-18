VPATH=../xyzip
ifeq ($(MAKECMDGOALS),r)
	gcc_config=-std=c++20 -O3 -DNDEBUG
	show_msg=release
else
	gcc_config=-std=c++20 -O3
	show_msg=debug
endif

d: clean
	g++ xyzip_tests.cpp -o a $(gcc_config)
	@echo $(show_msg)

r: d

.PHONY: clean
clean:
	del *.exe
	del *.o