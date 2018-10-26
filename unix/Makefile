.SUFFIXES:

EXE = $(MAKECMDGOALS)

# if EXE is empty, add all .cpp

CXX=
CXX += c++
CXX += -std=c++11
CXX += -O0 # 03
CXX += -gsplit-dwarf
CXX += -Wall
CXX += -Wextra
CXX += -Wno-unused-parameter

INC=
INC += -I ./

LIB=

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux) #LINUX
endif

ifeq ($(UNAME_S), Darwin) #APPLE
endif

ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
endif

OBJ=
OBJ += sine.o take.o
#OBJ += external/AudioFFT/AudioFFT.o

HDR=
HDR += everything.h

$(EXE): $(EXE).cpp
	$(CXX) $(INC) -o $@.exe $^ $(LIB)
	chmod 755 $@.exe

%.o: %.cpp
	$(CXX) $(INC) -c -o $@ $^

%.a: %.o

%.o: %.c
	cc $(INC) -c -o $@ $<
