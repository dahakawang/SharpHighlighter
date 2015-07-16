
SOURCE=$(shell ls *.cc)
OBJECTS=$(SOURCE:.cc=.o)

all:$(OBJECTS)

$(OBJECTS): %.o: %.cc
	$(CC) -c $(CFLAGS) $(INCLUDE_PATH) $(LIB_PATH) $< -o $@

clean:
	rm *.o
