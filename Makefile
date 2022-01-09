CXX=g++

INCS=-I./resource

LIBS=-L./lib -lglfw3 -lglew32s -lopengl32 -lgdi32

SRC_DIR=src

all: terrain

terrain: main.o
	$(CXX) $^ $(LIBS) -o $@

main.o: $(SRC_DIR)/main.cpp
	$(CXX) -c $(INCS) $^ -o $@

.PHONY: clean
clean:
	rm terrain *.o