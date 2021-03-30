#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <pthread.h>
#include "Density.hpp"
#include "Method3.hpp"
#include "Method4.hpp"
#include "Method3_Mutex.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    int method = atoi(argv[argc-1]);
    if(method == 0){
            DensityCalculator instance(argc-1, argv);
            instance.runDensityEsitmator();
    }else if(method == 1){
        DensityCalculator instance(argc-2, argv);
        instance.setspeed(atoi(argv[argc-2]));
        instance.runDensityEsitmator();
    }else if(method == 2){
        DensityCalculator instance(argc-3, argv);
        instance.setwidth(atoi(argv[argc-2]));
        instance.setheight(atoi(argv[argc-3]));
        instance.runDensityEsitmator();
    }else if(method == 3){
        method3(argc-1, argv, 2, 2);
    }else if(method == 4){
        method4(argc-1, argv, atoi(argv[1]));
    }else{
        method3_beta(argc-1, argv, 2, 2);
    }

    // method4(argc-1, argv, atoi(argv[1]));

     //method3(argc, argv, 2, 2);
    // method3_beta(argc - 2, argv, atoi(argv[1]), atoi(argv[2]));
     //method3_beta(argc, argv, 2, 2);
    


    
    return 0;
}
