CC = g++
OPENCV4 = -pthread -std=c++11 `pkg-config --cflags --libs opencv4`
LIBS = $(OPENCV4)
#SRC will contain the names of the cpp files
SRC = main.cpp Density.cpp Method3.cpp Method4.cpp Method3_Mutex.cpp
#OBJ names of object files
OBJ = $(SRC:.cpp = .o)
#exe name
CNAME = COP290

compile: $(OBJ) 
	$(info Compiling code...)
	@$(CC) $(OBJ) -o $(CNAME) $(LIBS) 

run: 
	$(info Running compiled code...)
	@./$(CNAME) 2 2 2

all: $(OBJ)
	$(info Compiling code...)
	@$(CC) $(OBJ) -o $(CNAME) $(LIBS)
	$(info Running compiled code...) 
	@./$(CNAME) 2 2 2

clean:
	$(info Deleting compiled files...)
	@rm $(CNAME)