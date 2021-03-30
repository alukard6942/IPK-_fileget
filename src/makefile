# File: makefile
# Author: xkoval18 <xkoval18@github>
# File: makefile
# Date: 20.03.2021

CFLAGS = -std=c++11 -g -Wall -pedantic -O2 
CC = g++

NAME=xkoval18

fileget: fileget.cpp server.o
	$(CC) $(CFLAGS) -o fileget fileget.cpp server.cpp server.h 

server.o: server.cpp server.h log.h
	$(CC) $(CFLAGS) -o server.o -c server.cpp server.h 

$(NAME)-src.zip: fileget
	zip $(NAME)-src.zip fileget.cpp server.cpp server.h log.h makefile 

$(NAME)-out.zip: fileget 
	mkdir out -p
	cd out && ../fileget -n 147.229.176.19:3333 -f fsp://blue.server/000093.text 
	cd out && ../fileget -n 147.229.176.19:3333 -f fsp://blue.server/000020.pdf 
	cd out && ../fileget -n 147.229.176.19:3333 -f fsp://blue.server/000052.xls 
	cd out && ../fileget -n 147.229.176.19:3333 -f fsp://red.server/index
	cd out && ../fileget -n 147.229.176.19:3333 -f fsp://green.server/*	
	cd out && zip $(NAME)-out.zip * 
	mv out/$(NAME)-out.zip .


.PHONY: clean pack example
#SERVER=127.0.0.1:6669
SERVER=147.229.176.19:3333

#FILE=fsp://me.cz/*
FILE=fsp://red.server/*

example: fileget
	mkdir -p /tmp/fileget
	cd /tmp/fileget && fileget -n $(SERVER) -f $(FILE)
	tree /tmp/fileget/

pack: clean $(NAME)-out.zip $(NAME)-src.zip
	echo you are welcome

clean:
	rm -rf *.o
	rm -rf fileget
	rm -rf index.txt
	rm -rf $(NAME)-src.zip
	rm -rf $(NAME)-out.zip

