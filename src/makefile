# File: makefile
# Author: xchuda04 
# Date: 31.03.2021

CFLAGS = -std=gnu11 -g -Wall -pedantic -O2 
CC = gcc

NAME=xchuda04

fileget: main.c
	$(CC) $(CFLAGS) -o fileget main.c

$(NAME)-src.zip: fileget 
	zip $(NAME)-src.zip main.c makefile

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

pack: clean $(NAME)-src.zip $(NAME)-out.zip
	echo you are welcome

clean:
	rm -rf *.o
	rm -rf fileget
	rm -rf index.txt
	rm -rf $(NAME)-src.zip
	rm -rf $(NAME)-out.zip
	rm -rf out

