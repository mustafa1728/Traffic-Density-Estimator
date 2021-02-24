compile: 
	$(info Compiling code...)
	@g++ homography.cpp -o homography -pthread -std=c++11 `pkg-config --cflags --libs opencv`
run: 
	$(info Running compiled code...)
	@./homography
all:
	$(info Compiling and running code...)
	@g++ homography.cpp -o homography -pthread -std=c++11 `pkg-config --cflags --libs opencv` && ./homography
clean:
	$(info Deleting compiled files...)
	@rm homography