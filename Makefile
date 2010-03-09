.PHONY: all clean install uninstall distclean

all: 
	./waf

install:
	./waf install

uninstall:
	./waf uninstall

clean:
	./waf clean


distclean:
	./waf distclean
