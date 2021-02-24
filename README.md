#COP290 SUBTASK 1

####Mustafa Chasmai 2019CS10341  |  Tamajit banerjee 2019CS10408

###Instructions for MakeFile
~~~ 
make compile 
~~~
This will compile the code and generate the executable file homography in the working directory
~~~ 
make run
~~~
This will execute the code and open a window named "COP290 - Subtask 1" which will contain the default traffic image.

~~~ 
make all
~~~
This will run both compile and run.

~~~ 
make clean
~~~
This will remove the executable file.

###Instrctions for compiling from command line 

    g++ homography.cpp -o homography -pthread -std=c++11 `pkg-config --cflags --libs opencv` 
    OR
    make compile
    
    
###Instrctions for running executable from command line
    
~~~
 ./homography
 OR        
 make run
~~~    

This will execute the code and open a window named "COP290 - Subtask 1" which will contain the default traffic image it will save the warped image by the default name "warp.jpg" and the cropped image by the default name "crop.jpg" in the same folder. 
   
~~~
 ./homography image.jpg
~~~

This will execute the code and open a window named "COP290 - Subtask 1" which will contain the image specified by the path "image.jpg" and it will save the warped image by the default name "warp.jpg" and the cropped image by the default name "crop.jpg" in the same folder.
  
~~~
 ./homography image.jpg warp_test.jpg crop_test.jpg  
~~~

This will execute the code and open a window named "COP290 - Subtask 1" which will contain the image specified by the path "image.jpg" and it will save the warped image by the name "warp_test.jpg" and the cropped image by the name "crop_test.jpg" in the same folder.

Wrong command line arguments will print the appropriate help instructions.

###Note

We have tried to dynamically create the images sizes for the warped image and the cropped image based on the input image size.

 