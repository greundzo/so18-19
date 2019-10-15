CC=gcc
CFLAGS=-Wall
DEPS=shared.h
ODIR =../bin

.PHONY: all

all: student gestore clean

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

student: $(ODIR)/student.o $(ODIR)/shared.o
	$(CC) -o $(ODIR)/$@ $^ $(CFLAGS)

gestore: $(ODIR)/gestore.o $(ODIR)/shared.o
	$(CC) -o $(ODIR)/$@ $^ $(CFLAGS)

clean:
	rm -f $(ODIR)/*.o
#clear
