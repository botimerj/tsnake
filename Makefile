
CC 		:= g++
ARGS	:= -O2
INCLUDE := -Isrc/
LINK	:= -L/usr/lib/x86_64-linux-gnu/ -lpthread
SRC 	:= src/*.cpp

all: tsnake 

tsnake: $(SRC)
	$(CC) $(ARGS) $(INCLUDE) $(SRC) tsnake.cpp -o tsnake $(LINK) 

clean:
	rm tsnake 

.PHONY: all tsnake 
