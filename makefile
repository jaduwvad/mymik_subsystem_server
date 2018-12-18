CC = g++

LIBPATH = ./lib
SRCPATH = ./src
OBJPATH = ./obj

INCLUDEFLAG = -I ./include
LIBFLAG = -L $(LIBPATH) -lserv -Wl,-rpath,$(LIBPATH)
VERSIONFLAG = -std=c++11

OBJFLAGS = -fPIC
SHARELIBFLAGS = -shared -o
DBFLAG = -I /usr/include/mysql -lmysqlclient

all: dbconnector updater libmaker receiver

dbconnector: $(SRCPATH)/dbconnector.cpp
	$(CC) $(OBJFLAGS) -c $(SRCPATH)/dbconnector.cpp -o $(OBJPATH)/dbconnector.o $(INCLUDEFLAG) $(DBFLAG)

updater: $(SRCPATH)/updater.cpp
	$(CC) $(OBJFLAGS) -c $(SRCPATH)/updater.cpp -o $(OBJPATH)/updater.o $(INCLUDEFLAG) $(DBFLAG) $(VERSIONFLAG)

libmaker: $(LIBPATH)
	$(CC) $(SHARELIBFLAGS) $(LIBPATH)/libserv.so $(OBJPATH)/*

receiver: receiver.cpp
	$(CC) -o receiver receiver.cpp $(INCLUDEFLAG) $(DBFLAG) $(LIBFLAG) $(VERSIONFLAG)



