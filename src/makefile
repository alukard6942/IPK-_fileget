# File: makefile
# Author: alukard <alukard@github>
# Date: 20.03.2021

CFLAGS = -Wall -std=c++20 # -pedantic  #-O2 -g-std=c++11 -Wextra  
CC = g++

example: fileget
	./fileget -n 127.0.0.1:6669 -f fsp://me.cz/index.txt

fileget: fileget.cpp server.o
	$(CC) $(CFLAGS) -o fileget fileget.cpp server.cpp server.h 

server.o: server.cpp server.h
	$(CC) $(CFLAGS) -o server.o -c server.cpp server.h 

port: port.cpp
	$(CC) $(CFLAGS) port.cpp -o port

getinfo: getinfo.cpp
	$(CC) $(CFLAGS) getinfo.cpp -o getinfo

.PHONY: clean 
clean:
	rm -rf port
	rm -rf example
	rm -rf getinfo
	rm -rf client.o
	rm -rf server.o
	rm -rf fileget
	rm -rf index.txt

