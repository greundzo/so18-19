CC=gcc
CFLAGS=-Wall
DEPS=shared.h
ODIR =../bin

.PHONY: all

all: student gestore clean

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -std=gnu99 -c -o $@ $< $(CFLAGS)

student: $(ODIR)/student.o $(ODIR)/shared.o
	$(CC) -std=gnu99 -o $(ODIR)/$@ $^ $(CFLAGS)

gestore: $(ODIR)/gestore.o $(ODIR)/shared.o
	$(CC) -std=gnu99 -o $(ODIR)/$@ $^ $(CFLAGS)

shared: shared.o
	$(CC) shared.o -o shared

test:
	cd $(ODIR);
	./shared

execute:
	cd $(ODIR);
	./gestore

clean:
	rm -f $(ODIR)/*.o
	rm -f shared

#clear
