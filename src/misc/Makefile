
SOURCE=$(shell ls *.cc)
OBJECTS=$(SOURCE:.cc=.o)

all: $(ROOT)/lib/libmisc.a

$(ROOT)/lib/libmisc.a: $(OBJECTS)
	-@mkdir -p $(ROOT)/lib
	$(AR) rcs $(ROOT)/lib/libmisc.a $(OBJECTS)

$(OBJECTS): %.o: %.cc
	$(CC) -c $(CFLAGS) $(INCLUDE_PATH) $< -o $@

clean:
	-rm $(ROOT)/lib/libmisc.a
	-rm *.o
