compile: 
	$(info Compiling code...)
	@g++ density.cpp -o density -pthread -std=c++11 `pkg-config --cflags --libs opencv4`
run: 
	$(info Running compiled code...)
	@./density
all:
	$(info Compiling and running code...)
	@g++ density.cpp -o density -pthread -std=c++11 `pkg-config --cflags --libs opencv4` && ./density
clean:
	$(info Deleting compiled files...)
	@rm density