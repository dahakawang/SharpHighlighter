
SOURCE=$(shell ls *.cc)
OBJECTS=$(SOURCE:.cc=.o)

all: $(ROOT)/lib/libscope.a

$(ROOT)/lib/libscope.a: $(OBJECTS)
	-@mkdir -p $(ROOT)/lib
	$(AR) rcs $(ROOT)/lib/libscope.a $(OBJECTS)

$(OBJECTS): %.o: %.cc
	$(CC) -c $(CFLAGS) $(INCLUDE_PATH) $< -o $@

clean:
	-rm $(ROOT)/lib/libscope.a
	-rm *.o
