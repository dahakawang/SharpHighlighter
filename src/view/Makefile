TARGET=html

all:
	@for target in $(TARGET); do \
		(cd $$target; make all); \
	done

clean:
	@for target in $(TARGET); do \
		(cd $$target; make clean); \
	done
