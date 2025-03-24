build:
	g++ -Wall -c magic.cpp -o magic.o
	g++ -Wall champ.cpp magic.o -o champ

buildso:
	g++ -fpic -Wall -c magic.cpp -o magic.o
	g++ -fpic -Wall -c champ.cpp magic.o -o champ.o
	g++ -shared -fpic -Wall -Wl,-soname,libtruc.so -o libtruc.so champ.o magic.o

runpy:
	python3 ./test.py

run:
	./champ