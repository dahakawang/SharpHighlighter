
SOURCE=$(shell ls *.cc)
OBJECTS=$(SOURCE:.cc=.o)

all:$(OBJECTS)
	mkdir -p $(ROOT)/lib
	$(AR) rcs $(ROOT)/lib/libshl.a $(OBJECTS)

$(OBJECTS): %.o: %.cc
	$(CC) -c $(CFLAGS) $(INCLUDE_PATH) $< -o $@

clean:
	rm $(ROOT)/lib/libshl.a
	rm *.o
