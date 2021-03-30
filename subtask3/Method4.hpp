#ifndef Method4_hpp
#define Method4_hpp

#include "Density.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

struct Args_temporal {
    int number_of_threads;
    int thread_number;

    int argc;
    char** argv;

    vector<float> queue_densities;
    vector<float> dynamic_densities;
};
void * temporalThreadWorker(void * arguments);
void method4(int argc, char** argv, const int number_of_threads = 10);


#endif /* Method4_hpp */
