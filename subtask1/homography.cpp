
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;
 
int main( int argc, char** argv)
{

    Mat image = imread("images/traffic.jpg");

    vector<Point2f> pts_src;
    pts_src.push_back(Point2f(980, 213));
    pts_src.push_back(Point2f(1261, 200));
    pts_src.push_back(Point2f(1573, 1077));
    pts_src.push_back(Point2f(92, 1080));

    vector<Point2f> pts_dst;
    pts_dst.push_back(Point2f(472,52));
    pts_dst.push_back(Point2f(800,52));
    pts_dst.push_back(Point2f(800,830));
    pts_dst.push_back(Point2f(472,830));

    Mat h = findHomography(pts_src, pts_dst);

    Mat result;

    warpPerspective(image, result, h, image.size());

    imshow("Source Image", image);
    imshow("Warped Source Image", result);
 
    waitKey(0);
}
