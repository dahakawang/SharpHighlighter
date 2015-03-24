include $(ROOT)/MacroRule

.PHONY: all clean
all: main

main: main.cc
	clang++ $(CXXFLAGS) $(LIB) $(INCLUDE) $(LINKFLAG)  -o $@  main.cc

clean:
	@-rm *.o
	@-rm main
