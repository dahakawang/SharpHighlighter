
SOURCE=$(shell ls *.cc)
OBJECTS=$(SOURCE:.cc=.o)

all:$(OBJECTS)

$(OBJECTS): %.o: %.cc
	$(CC) $(CFLAGS) -I$(ROOT)/src $(INCLUDE_PATH) -L$(ROOT)/lib $(LIB_PATH) $< $(LIBS) -lshl -o $@

clean:
	rm *.o
