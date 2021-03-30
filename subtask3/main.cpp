#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <pthread.h>
#include "Density.hpp"
#include "Methods/Method3.hpp"
#include "Methods/Method4.hpp"
#include "Methods/Method3_Mutex.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    TickMeter tm;
    tm.start();

    int method = 0;
    if(argc>1){
        method = atoi(argv[argc-1]);
    }
    if(method == 0){
        DensityCalculator instance(argc-1, argv);
        instance.runDensityEsitmator(false);
    }else if(method == 1){
        DensityCalculator instance(argc-2, argv);
        instance.setspeed(atoi(argv[argc-2]));
        instance.runDensityEsitmator(false);
    }else if(method == 2){
        DensityCalculator instance(argc-3, argv);
        instance.setwidth(atoi(argv[argc-3]));
        instance.setheight(atoi(argv[argc-2]));
        instance.runDensityEsitmator(false);
    }else if(method == 3){
        method3(argc-3, argv, atoi(argv[argc-3]), atoi(argv[argc-2]), false);
    }else if(method == 4){
        method4(argc-1, argv, atoi(argv[1]));
    }else if(method == 5){
        method3_beta(argc-1, argv, 2, 2);
    }

    tm.stop();
    ofstream TimeFile("time.txt", ofstream::out | ofstream::app);
    TimeFile<<"\nMethod:\t"<<method<<"\n";
    if(method==1)
        TimeFile<<"Parameter:\t numer of frames skipped = "<<atoi(argv[argc-2]);
    else if(method==2)
        TimeFile<<"Parameters:\t width divisions = "<<atoi(argv[argc-3])<<" \theight division = "<<atoi(argv[argc-2]);
    else if(method==3)
        TimeFile<<"Parameters:\t width divisions = "<<atoi(argv[argc-3])<<" \theight division = "<<atoi(argv[argc-2]);
    else if(method==4)
        TimeFile<<"Parameter:\t number of threads = "<<atoi(argv[argc-2]);
    TimeFile<<"\nTime:\t"<<tm.getTimeSec()<<" seconds\n";

    return 0;
}
