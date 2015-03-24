TMLIBS := textmate/build/Frameworks/parse/libparse.a \
				textmate/build/Frameworks/text/libtext.a \
				textmate/build/Frameworks/plist/libplist.a \
				textmate/build/Frameworks/scope/libscope.a \
				textmate/build/Frameworks/bundles/libbundles.a \
				textmate/build/Frameworks/regexp/libregexp.a \
				textmate/build/Frameworks/cf/libcf.a \
				textmate/build/Frameworks/io/libio.a \
				textmate/build/Frameworks/OakSystem/libOakSystem.a \
				textmate/build/Frameworks/crash/libcrash.a

.PHONY: all
all: lib/libtm.a

textmate/build.ninja:
	@(cd textmate; git submodule update --init;)
	@echo 'EXPORT        = src/*.h' >> textmate/Frameworks/parse/target; 
	@(cd textmate; export builddir=./build; ./configure)

$(TMLIBS): textmate/build.ninja
	@(cd textmate; ninja $(subst textmate/,,$@);)
	@(cd textmate; name=`dirname $@`; name=`basename $$name`; ninja $$name/headers)

textmate/build/vendor/Onigmo/libOnigmo.a: textmate/build.ninja
	@(cd textmate; ninja $(subst textmate/,,$@);)
	@(cd textmate; name=`dirname $@`; name=`basename $$name`; ninja $$name/headers)

lib:
	@mkdir lib

lib/libtm.a: $(TMLIBS) textmate/build/vendor/Onigmo/libOnigmo.a | lib
	@-rm lib/*.a
	@for lib in $(TMLIBS); do \
		cp $$lib lib; \
		(cd lib; ar -x `basename $$lib`); \
		(cd lib; rm `basename $$lib`); \
		(cd lib; ar -qcs libtm.a *o); \
	done
	cp textmate/build/vendor/Onigmo/libOnigmo.a lib
	@-rm lib/*.o
	

.PHONY: clean
clean:
	@(cd textmate; git checkout .)
	@-rm -rf textmate/build
	@-rm textmate/build.ninja
	@-(cd textmate; rm $(TMLIBS))
	@-rm -rf lib
