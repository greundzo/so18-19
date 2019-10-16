.PHONY: all 
.SILENT:

all :
	cd src; $(MAKE)
execute:
	cd src; $(MAKE) execute	
clean:
	ipcrm -S 5 -M 30