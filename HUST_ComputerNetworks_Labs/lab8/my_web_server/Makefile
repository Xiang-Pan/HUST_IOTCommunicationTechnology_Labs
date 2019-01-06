DIR_INC = ./include
# DIR_SRC = ./src
# DIR_OBJ = ./obj
# DIR_BIN = ./bin

CXX = g++

EXEC_HTTP = ./bin/server_http
EXEC_HTTPS = ./bin/server_https

SOURCE_HTTP = ./src/server_http_runner.cpp
SOURCE_HTTPS = ./src/server_https_runner.cpp

OBJECTS_HTTP = server_http_runner.o
OBJECTS_HTTPS =  server_https_runner.o

LDFLAGS_COMMON = -std=c++11 -O3 -pthread -lboost_system
LDFLAGS_HTTP =
LDFLAGS_HTTPS = -lssl -lcrypto

LPATH_COMMON = -I/usr/include/boost
LPATH_HTTP =
LPATH_HTTPS = -I/usr/include/openssl

LLIB_COMMON = -L/usr/lib

all:
	make http
	make https

http:
	$(CXX) $(SOURCE_HTTP) -I$(DIR_INC) $(LDFLAGS_COMMON) $(LDFLAGS_HTTP) $(LPATH_COMMON) $(LPATH_HTTP) $(LLIB_COMMON) $(LLIB_HTTP) -o $(EXEC_HTTP)
https:
	$(CXX) $(SOURCE_HTTPS) -I$(DIR_INC) $(LDFLAGS_COMMON) $(LDFLAGS_HTTPS) $(LPATH_COMMON) $(LPATH_HTTPS) $(LLIB_COMMON) $(LLIB_HTTPS) -o $(EXEC_HTTPS)

clean:
	rm -f $(EXEC_HTTP) $(EXEC_HTTPS) *.o
