
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat image = imread("images/empty.jpg");

int i = 0;
vector<Point2f> pts_src;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{

    if  ( event == EVENT_LBUTTONDOWN || event == EVENT_RBUTTONDOWN ||  event == EVENT_MBUTTONDOWN ){
        pts_src.push_back(Point2f(x, y));
        i+=1;
    }
    else{
        return;
    }
    
    if (i != 4){
        return;
    }
    vector<Point2f> pts_dst;
    pts_dst.push_back(Point2f(472,52));
    pts_dst.push_back(Point2f(800,52));
    pts_dst.push_back(Point2f(800,830));
    pts_dst.push_back(Point2f(472,830));

    Mat h = findHomography(pts_src, pts_dst);

    Mat result;

    warpPerspective(image, result, h, image.size());
    
    imshow("Warped Image", result);

    waitKey(0); 
}


int main( int argc, char** argv)
{
    namedWindow("COP290 - Subtask 1", 1);

    setMouseCallback("COP290 - Subtask 1", CallBackFunc, NULL);

    imshow("COP290 - Subtask 1", image);

    waitKey(0);
    return 0;
}
