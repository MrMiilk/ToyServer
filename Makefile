CXX = g++
DEBUG = -g
CXXFLAGS = -Wall -O0 -std=c++11 -I ./
LDFLAGS = -lprotobuf -L/usr/lib/mysql -lmysqlclient -lboost_system -lboost_filesystem -pthread
BINARIES = server

# basefiles = $(filter %.cpp,$(shell ls ./base/))
# basefiles_ = $(addprefix ./base/,$(basefiles))
dbfiles = $(filter %.cpp,$(shell ls ./db/))
dbfiles_ = $(addprefix ./db/,$(dbfiles))
ftpfiles = $(filter %.cpp,$(shell ls ./ftp/))
ftpfiles_ = $(addprefix ./ftp/,$(ftpfiles))
netfiles = $(filter %.cpp,$(shell ls ./net/))
netfiles_ = $(addprefix ./net/,$(netfiles))
threadfiles = $(filter %.cpp,$(shell ls ./thread/))
threadfiles_ = $(addprefix ./thread/,$(threadfiles))
protosfiles = $(filter %.cpp,$(shell ls ./protos/))
protosfiles_ = $(addprefix ./protos/,$(protosfiles))
encryptfiles = $(filter %.cpp,$(shell ls ./encrypt/))
encryptfiles_ = $(addprefix ./encrypt/,$(encryptfiles))

BASE_SRC = $(basefiles_) $(dbfiles_) $(ftpfiles_) $(netfiles_)\
	 $(threadfiles_) $(protosfiles_) $(encryptfiles_)

all: $(BINARIES)

$(BINARIES):
	$(CXX) $(CXXFLAGS) $(DEBUG) -o $@ $(BASE_SRC) $(filter %.cpp,$^) $(LDFLAGS)

server: server.cpp

clean_o :
	rm $(BINARIES)
