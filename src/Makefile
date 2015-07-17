
SOURCE=$(shell ls *.cc)
OBJECTS=$(SOURCE:.cc=.o)

all:$(OBJECTS)
	$(CC) $(CFLAGS) $(LIB_PATH) $(OBJECTS) $(LIBS) -o run

$(OBJECTS): %.o: %.cc
	$(CC) -c $(CFLAGS) $(INCLUDE_PATH) $< -o $@

clean:
	rm *.o
