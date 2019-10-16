.PHONY: all 
.SILENT:

all :
	cd src; $(MAKE)
test:
	cd src; $(MAKE) test
execute:
	cd src; $(MAKE) execute	
      
