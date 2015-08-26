
SOURCE=$(filter-out catch_main.cc, $(shell ls *.cc))
TARGETS=$(SOURCE:.cc=.run)
ALLLIBS=$(shell ls $(ROOT)/lib/*.a)

all:$(TARGETS)

$(TARGETS): %.run: %.cc catch_main.o $(ALLLIBS)
	$(CC) $(CFLAGS) -I$(ROOT)/src $(INCLUDE_PATH) -L$(ROOT)/lib $(LIB_PATH) $< catch_main.o $(SHL_LIBS) $(LIBS) -o $@

catch_main.o: catch_main.cc
	$(CC) -c $(CFLAGS) -I$(ROOT)/src $(INCLUDE_PATH) catch_main.cc -o catch_main.o

clean:
	@-rm *.o
	@-rm -rf *.dSYM
	@-rm *.run
