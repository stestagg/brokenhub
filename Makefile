
.PHONY: clean

CC = g++ --std=gnu++0x -O3 -Ijson/libJSONpp

SRC_FILES = $(notdir $(wildcard src/*.cpp))
FILE_BASES = $(basename $(SRC_FILES))
OBJ_FILES = $(addsuffix .o,$(addprefix obj/,$(notdir $(FILE_BASES))))

brokenhub: $(OBJ_FILES) bin/
	$(CC) ${OBJ_FILES} obj/libJSONpp.la -o bin/brokenhub

bin/: 
	mkdir bin/

obj/:
	mkdir obj/

obj/libJSONpp.la:
	cd json &&	make static
	cp json/bin/libJSONpp.la obj/

${OBJ_FILES} : obj/%.o : src/%.cpp | obj/ obj/libJSONpp.la
	$(CC) -c $< -o $@

clean:
	- rm -rf bin/
	- rm -rf obj/
	cd json && make clean	
