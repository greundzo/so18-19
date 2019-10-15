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

clean:
	rm -f $(ODIR)/*.o

#clear
