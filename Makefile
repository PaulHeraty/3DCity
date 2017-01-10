default: all

all: way x11

clean:
	$(MAKE) -f Makefile_Way clean
	$(MAKE) -f Makefile_X11 clean

way: 
	$(MAKE) -f Makefile_Way

x11:
	$(MAKE) -f Makefile_X11

