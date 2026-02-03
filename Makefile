# Jacob's Pong Makefile
# Copyright © Stephane Jacob
# Contact: jacobstephane@outlook.com

CXX = g++
CXXFLAGS = -O2 -Wall -std=c++11
LDFLAGS = -lSDL2 -lSDL2_mixer
SRC = main.cpp game.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = Jacob's Pong

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o "$@" $^ $(LDFLAGS)

%.o: %.cpp game.h font.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJ) "$(TARGET)"
