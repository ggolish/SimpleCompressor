CC 	= gcc
CFLAGS 	= -O3
OBJS	= main.o compress.o mtf.o huffman.o bits.o bwt.o

compress456: $(OBJS)
	$(CC) $(CFLAGS) -o compress456 $(OBJS)

compress.o: compress.c compress.h mtf.h huffman.h 
	$(CC) $(CFLAGS) -c compress.c

main.o: main.c compress.h
	$(CC) $(CFLAGS) -c main.c

mtf.o: mtf.c mtf.h bwt.h
	$(CC) $(CFLAGS) -c mtf.c

huffman.o: huffman.c huffman.h bits.h
	$(CC) $(CFLAGS) -c huffman.c

bits.o: bits.c bits.h
	$(CC) $(CFLAGS) -c bits.c

bwt.o: bwt.c bwt.h
	$(CC) $(CFLAGS) -c bwt.c

clean:
	rm -f $(OBJS) compress456
