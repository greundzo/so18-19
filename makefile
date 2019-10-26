.PHONY: all 
.SILENT:

all :
	cd src; $(MAKE)
execute:
	cd src; $(MAKE) execute	
clean:
	ipcrm -S 555 
	ipcrm -M 300
	ipcrm -Q 777