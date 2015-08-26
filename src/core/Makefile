
SOURCE=$(shell ls *.cc)
OBJECTS=$(SOURCE:.cc=.o)

all: $(ROOT)/lib/libcore.a

$(ROOT)/lib/libcore.a: $(OBJECTS)
	-@mkdir -p $(ROOT)/lib
	$(AR) rcs $(ROOT)/lib/libcore.a $(OBJECTS)

$(OBJECTS): %.o: %.cc
	$(CC) -c $(CFLAGS) $(INCLUDE_PATH) $< -o $@

clean:
	-rm $(ROOT)/lib/libcore.a
	-rm *.o
