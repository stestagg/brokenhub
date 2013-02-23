## libJSONpp makefile.


CXXFLAGS = -O2 -fPIC -DJSON_POSIX -I./libJSONpp/maps
CXX = g++ $(CXXFLAGS)
LD = ld
#-Wall

FILES = value parser_errors parser_UNI parser_UTF8 convert types maps/sfh

.PHONY : libJSONpp clean check static shared

libJSONpp:  shared
	make check

static : bin/libJSONpp.la

shared : bin/libJSONpp.so

# Build shared library
bin/libJSONpp.so : obj/ bin/ $(addsuffix .o,$(addprefix obj/,${FILES})) 
	$(CXX) -shared -fPIC -Wl -o bin/libJSONpp.so $(addsuffix .o,$(addprefix obj/,${FILES})) 

# Build static library
bin/libJSONpp.la : obj/ bin/ $(addsuffix .o,$(addprefix obj/,${FILES}))
	ar rcs bin/libJSONpp.la $(addsuffix .o,$(addprefix obj/,${FILES}))

# Compile to object files
$(addsuffix .o,$(addprefix obj/,${FILES})) : obj/%.o : libJSONpp/%.cpp
	$(CXX) -c $< -o $@

# Special case SFH, as it's in maps subdir
#obj/sfh.o : libJSONpp/maps/sfh.cpp
#	$(CXX) -c $< -o $@

# Build Tests
test : bin/libJSONpp.la 
	cp tests/test.json bin/
	cp tests/test.json.UTF32 bin/
	$(CXX) tests/*.cpp bin/libJSONpp.la -o bin/test

# Run tests
check : test
	chmod +x runtest.sh
	./runtest.sh

obj/ : 
	mkdir obj
	mkdir obj/maps

bin/ :
	mkdir bin

clean :
	-rm -r obj/
	-rm -r bin/

