
SOURCE=$(filter-out catch_main.cc, $(shell ls *.cc))
OBJECTS=$(SOURCE:.cc=.o)
TARGETS=$(SOURCE:.cc=.run)
ALLLIBS=$(shell ls $(ROOT)/lib/*.a)

all:$(TARGETS) shl_test.run

$(TARGETS): %.run: %.o catch_main.o $(ALLLIBS)
	$(CC) $(CFLAGS) -I$(ROOT)/src $(INCLUDE_PATH) -L$(ROOT)/lib $(LIB_PATH) $< catch_main.o $(SHL_LIBS) $(LIBS) -o $@

shl_test.run: $(OBJECTS) catch_main.o $(ALLLIBS)
	$(CC) $(CFLAGS) -I$(ROOT)/src $(INCLUDE_PATH) -L$(ROOT)/lib $(LIB_PATH) $(OBJECTS) catch_main.o $(SHL_LIBS) $(LIBS) -o $@

$(OBJECTS): %.o: %.cc
	$(CC) -c $(CFLAGS) -I$(ROOT)/src $(INCLUDE_PATH) $< -o $@

catch_main.o: catch_main.cc
	$(CC) -c $(CFLAGS) -I$(ROOT)/src $(INCLUDE_PATH) catch_main.cc -o catch_main.o

clean:
	@-rm *.o
	@-rm -rf *.dSYM
	@-rm *.run
