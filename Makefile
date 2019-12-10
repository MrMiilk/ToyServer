CXX = g++
DEBUG = -g
CXXFLAGS = -Wall $(DEBUG) -std=c++11 -pthread
LDFLAGS = -pthread
objects = server_2.o MutexLock.o Condition.o Socket.o Event.o\
	Queue.o Thread.o ThreadPool.o Epoll.o TCPconn.o TCPserver.o FTPserver.o

server: $(objects)
	$(CXX) $(LDFLAGS) -o server $(objects)

server_2.o:  Epoll.h Event.h Queue.h Socket.h TCPconn.h TCPserver.h Thread.h ThreadPool.h RSA_.h FTPserver.h

# MutexLock.o: 
Condition.o :MutexLock.h
# Socket.o :
Event.o: Epoll.h
Queue.o : MutexLock.h Condition.h
# Thread.o :
ThreadPool.o : MutexLock.h Condition.h Thread.h
Epoll.o : Event.h
TCPconn.o : Event.h Socket.h Queue.h
TCPserver.o : Event.h Socket.h TCPconn.h
FTPserver.o : Epoll.h Event.h Queue.h Socket.h TCPconn.h

clean_o :
	rm $(objects)