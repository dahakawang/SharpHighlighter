MODULES = vendor src
export ROOT = $(shell pwd)

.PHONY: all clean

all:
	@for proj in $(MODULES); do \
		(cd $$proj; make all); \
	done

clean:
	@for proj in $(MODULES); do \
		(cd $$proj; make clean); \
	done
