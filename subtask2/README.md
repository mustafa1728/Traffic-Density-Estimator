# COP290 TASK 1 : Traffic Density Estimator

#### Mustafa Chasmai 2019CS10341  |  Tamajit banerjee 2019CS10408

### Instructions for MakeFile
~~~ 
make compile 
~~~
This will compile the code and generate the executable file homography in the working directory

~~~ 
make clean
~~~
This will remove the executable file.

### Instructions for compiling from command line 

~~~
g++ density.cpp -o density -pthread -std=c++11 `pkg-config --cflags --libs opencv` 
OR
make compile
~~~
    
    
### Instructions for running executable from command line

Executing the code will open a window displaying the cropped frames, the images obtained by removing the background, and the dynamic density masks generated as intermediate steps. The frame number, queue density and dynamic density will be printed out in the console and also written in csv format in a file out.txt in the same directory as the executable.
    
Command 1 :
~~~
./homography
~~~   

This will run the code with default arguments. 

Command 2 :   
~~~
./homography trafficvideo.mp4
~~~

This will run the code and run inference on the video whose path is provided as the argument. It will take default values for the remaining arguments. 
  
~~~
./homography trafficvideo.mp4 background.jpg
~~~

This will run the code and run inference on the video whose path is provided as the argument. It will use the background image as the one provided as teh second argument. The default background image used by us is 'background.jpg', included in the submission. It will take default values for the remaining arguments. 

~~~
./homography trafficvideo.mp4 background.jpg 1
~~~

Along with the previous command, we have added the additional feature of allowing the user to manually input the four points to chose for cropping. This happens when the last argument is 1, and hardcoded points are assumed when it is 0, or no argument is passed.

Wrong command line arguments will print the appropriate help instructions:

Error: Incorrect Command. Please run the command in the following format:
./< filename > or 
./< filename > < path to video > or 
./< filename > < path to video > < path to back image> or 
./< filename > < path to video > < path to back image> < to take user input (0: no, 1: yes) >

### Special Features

1. We have allowed the user to select between hardcoded points for cropping and manual inputs
2. At each frame, we have taken average densities of the last 10 (tunable parameter) frames.
3. We have made the code modular, with parts working independently of other parts. 
4. We have made the code readable, with relevantly named variables and clearly defined sections with the help of comments.


 
