CC=gcc
CFLAGS=-Wall
DEPS=shared.h
ODIR =../build
DIR = build

.PHONY: all

all: build student gestore clean

build:
	cd ..; ./mkbuild

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -std=c99 -c -o $@ $< $(CFLAGS)

student: $(ODIR)/student.o $(ODIR)/shared.o
	$(CC) -std=c99 -o $(ODIR)/$@ $^ $(CFLAGS)

gestore: $(ODIR)/gestore.o $(ODIR)/shared.o
	$(CC) -std=c99 -o $(ODIR)/$@ $^ $(CFLAGS)

execute:
	cd $(ODIR); ./gestore

clean:
	rm -f $(ODIR)/*.o