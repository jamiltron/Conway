CC=clang++
CFLAGS = -Wall -std=c++14 -O3
MAIN_FLAGS = $(CFLAGS) `pkg-config --cflags sdl2 --static`
LDFLAGS = `pkg-config --libs sdl2`
EXE = conway
TEST_EXE = test
SOURCES = Game.cpp InputParser.cpp QuadTreeNode.cpp QuadTree.cpp
MAIN_SOURCES = $(SOURCES) main.cpp
TEST_SOURCES = $(SOURCES) tests/test.cpp tests/TestGame.cpp tests/TestInputParser.cpp tests/TestQuadTree.cpp tests/TestQuadTreeNode.cpp

default: conway

conway:
	$(CC) $(MAIN_FLAGS) $(MAIN_SOURCES) -o $(EXE) $(LDFLAGS)

test:
	$(CC) $(MAIN_FLAGS) $(TEST_SOURCES) -o $(TEST_EXE) $(LDFLAGS)

all: test conway

clean:
	rm conway test
