
CC 		:= g++
INCLUDE := -Isrc/
LINK	:= -L/usr/lib/x86_64-linux-gnu/ -lpthread
SRC 	:= src/*.cpp

all: tsnake 

tsnake: $(SRC)
	$(CC) $(INCLUDE) $(SRC) tsnake.cpp -o tsnake $(LINK) 

clean:
	rm tsnake 

.PHONY: all tsnake 
