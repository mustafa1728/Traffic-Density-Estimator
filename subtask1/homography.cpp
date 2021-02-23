#include <opencv2/opencv.hpp>
#include <iostream>


using namespace cv;
using namespace std;

Mat image, grey_image, result;

int i = 0;
vector<Point2f> pts_src;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{

    if (event == EVENT_LBUTTONDOWN || event == EVENT_RBUTTONDOWN || event == EVENT_MBUTTONDOWN) {
        pts_src.push_back(Point2f(x, y));
        ++i;
    }
    else {
        return;
    }

    if (i != 4) {
        return;
    }

    vector<Point2f> pts_dst;
    pts_dst.push_back(Point2f(720, 135));
    pts_dst.push_back(Point2f(1080, 135));
    pts_dst.push_back(Point2f(1080, 1080));
    pts_dst.push_back(Point2f(720, 1080));

    Mat h = findHomography(pts_src, pts_dst);

    warpPerspective(grey_image, result, h, grey_image.size());

    imshow("Warped Image", result);

    waitKey(0);

    Mat final_image;

    Rect crop_region(720, 135, 360, 945);

    final_image = result(crop_region);

    imshow("cropped image ", final_image);

}



int main(int argc, char** argv)
{
    image = imread("images/empty.jpg");

    cvtColor(image, grey_image, COLOR_BGR2GRAY);

    namedWindow("COP290 - Subtask 1", 1);

    setMouseCallback("COP290 - Subtask 1", CallBackFunc, NULL);

    imshow("COP290 - Subtask 1", grey_image);

    waitKey(0);

    return 0;
}