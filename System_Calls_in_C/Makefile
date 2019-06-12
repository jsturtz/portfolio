buildrooms:
	gcc sturtzj.buildrooms.c -o buildrooms -std=c89
	./buildrooms

adventure: buildrooms
	gcc sturtzj.adventure.c -o adventure -lpthread -std=c89

all: adventure buildrooms

clean: 
	rm -f -r sturtzj.rooms.*
	rm -f buildrooms
	rm -f adventure
