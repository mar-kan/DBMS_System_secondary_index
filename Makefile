CC = gcc
CFLAFS = -g -Wall -Iinclude

all : SHT move

SHT: main_SHT.o SHT.o HtList.o utilities.o HT.o SHTList.o
	$(CC) $(CFLAFS) -o SHT main_SHT.o SHT.o HtList.o SHTList.o utilities.o HT.o lib/BF_64.a -no-pie

main_SHT.o:
	$(CC) $(CFLAFS) -c $(INC) examples/main_SHT.c

SHT.o:
	$(CC) $(CFLAFS) -c src/SHT.c

SHTList.o:
	$(CC) $(CFLAFS) -c src/SHTList.c

HtList.o:
	$(CC) $(CFLAFS) -c src/HtList.c

utilities.o:
	$(CC) $(CFLAFS) -c src/utilities.c

main_HT.o:
	$(CC) $(CFLAFS) -c examples/main_HT.c

HT.o:
	$(CC) $(CFLAFS) -c src/HT.c

move:
	mv *.o ./bin/
	mv SHT ./build/

clean:
	rm *.o SHT block_files/* ###bin/* build/* ##########