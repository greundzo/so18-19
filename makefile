.PHONY: all 
.SILENT:

all :
	cd src; $(MAKE)
execute:
	cd src; $(MAKE) execute	
clean:
	ipcrm -S 5 
	ipcrm -M 30
	ipcrm -Q 7
	ipcrm -Q 8