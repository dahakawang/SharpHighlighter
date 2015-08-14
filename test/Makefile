
SOURCE=$(filter-out catch_main.cc, $(shell ls *.cc))
TARGETS=$(SOURCE:.cc=.run)

all:$(TARGETS)

$(TARGETS): %.run: %.cc catch_main.o $(ROOT)/lib/libshl.a
	$(CC) $(CFLAGS) -I$(ROOT)/src $(INCLUDE_PATH) -L$(ROOT)/lib $(LIB_PATH) $< -lshl catch_main.o $(LIBS) -o $@

catch_main.o: catch_main.cc
	$(CC) -c $(CFLAGS) -I$(ROOT)/src $(INCLUDE_PATH) catch_main.cc -o catch_main.o

clean:
	-rm *.run
