CC=clang++
CFLAGS = -Wall -std=c++14 -O3
MAIN_FLAGS = $(CFLAGS) `pkg-config --cflags sdl2 --static`
LDFLAGS = `pkg-config --libs sdl2`
EXE = conway
TEST_EXE = test
SOURCES = InputParser.cpp QuadTreeNode.cpp QuadTree.cpp
MAIN_SOURCES = $(SOURCES) Game.cpp main.cpp
TEST_SOURCES = $(SOURCES) tests/test.cpp  tests/TestInputParser.cpp tests/TestQuadTree.cpp tests/TestQuadTreeNode.cpp

default: conway

conway:
	$(CC) $(MAIN_FLAGS) $(MAIN_SOURCES) -o $(EXE) $(LDFLAGS)

test:
	$(CC) $(CFLAGS) $(TEST_SOURCES) -o $(TEST_EXE)

all: test conway

clean:
	rm conway test
