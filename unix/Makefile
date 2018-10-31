.SUFFIXES:

# maybe do different stuff based on what system we have...
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux) #LINUX
endif
ifeq ($(UNAME_S), Darwin) #APPLE
endif
ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
endif

CXX=
CXX += c++
CXX += -std=c++11
CXX += -O0 # 03
CXX += -gsplit-dwarf
CXX += -Wall
CXX += -Wextra
CXX += -Wno-unused-parameter

INC += -I ./

HDR=
HDR += everything.h

# find all the .cpp files and make a list of those, but with the .exe extension
CPP := $(wildcard *.cpp)
EXE := $(patsubst %.cpp,%.exe,$(CPP))

# build all the .cpp into .exe
_: $(EXE)

%.exe: %.o
	$(CXX) $(INC) -o $@ $^

%.o: %.cpp
	$(CXX) $(INC) -c -o $@ $^

%.a: %.o

%.o: %.c
	cc $(INC) -c -o $@ $<



