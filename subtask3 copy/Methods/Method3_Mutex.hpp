#ifndef Method3_Mutex_hpp
#define Method3_Mutex_hpp

#include "../Density.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;


void * spacialThreadWorkerBeta(void * arguments);
void method3_beta(int argc, char** argv, const int x = 1, const int y = 1, bool toDisplay = false);
#endif /* Method3_Mutex_hpp */
