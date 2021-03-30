#ifndef Method3_hpp
#define Method3_hpp

#include "Density.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

struct Args_spacial {
    Mat image;
    Rect crop_region;
    Mat background_subtracted;

    int thread_i, thread_j;
    int is_done;

    float queue_density;

    DensityCalculator obj;
};

void * spacialThreadWorker(void * arguments);
void method3(int argc, char** argv, const int x = 1, const int y = 1);

#endif /* Method3_hpp */
