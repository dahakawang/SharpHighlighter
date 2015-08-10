
SOURCE=$(shell ls *.cc)
TARGETS=$(SOURCE:.cc=)

all:$(TARGETS)

$(TARGETS): %: %.cc
	$(CC) $(CFLAGS) -I$(ROOT)/src $(INCLUDE_PATH) -L$(ROOT)/lib $(LIB_PATH) $< -lshl $(LIBS) -o $@.run

clean:
	-rm *.run
