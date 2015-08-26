
SOURCE=$(shell ls *.cc)
OBJECTS=$(SOURCE:.cc=.o)

all: $(ROOT)/lib/libhtmlview.a

$(ROOT)/lib/libhtmlview.a: $(OBJECTS)
	-@mkdir -p $(ROOT)/lib
	$(AR) rcs $(ROOT)/lib/libhtmlview.a $(OBJECTS)

$(OBJECTS): %.o: %.cc
	$(CC) -c $(CFLAGS) $(INCLUDE_PATH) $< -o $@

clean:
	-rm $(ROOT)/lib/libhtmlview.a
	-rm *.o
