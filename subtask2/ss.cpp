#include <opencv2/opencv.hpp>
#include <iostream>


using namespace cv;
using namespace std;

Mat image, background_img;

int i = 0;
vector<Point2f> pts_src;
int x_1, x_2, y_1, y_2;

int thisframe = 0;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{

    if (event == EVENT_LBUTTONDOWN || event == EVENT_RBUTTONDOWN || event == EVENT_MBUTTONDOWN) {
        thisframe = 1;
    }
    else {
        return;
    }
}

int main(int argc, char** argv)
{

    VideoCapture cap("../../trafficvideo.mp4"); 
    if (cap.isOpened() == false)  
    {
        cout << "Cannot open the video file" << endl;
        cin.get();
        return -1;
    }

    // String window_name = "Angle corrected and cropped video";

    // namedWindow(window_name, WINDOW_NORMAL);

    // namedWindow("background subtracted", WINDOW_NORMAL);

    while (true)
    {

        Mat frame;
        int threshold = 40;
        bool bSuccess = cap.read(frame); 

        if (bSuccess == false) 
        {
            cout << "Found the end of the video" << endl;
            break;
        }

        namedWindow("Chose the four points", 1);
        setMouseCallback("Chose the four points", CallBackFunc, NULL);
        imshow("Chose the four points", frame);
        
        if (thisframe == 1){
            imwrite("screenshot.jpg", frame);
            break;
        }
        
        if (waitKey(10) == 27)
        {
            break;
        }
    }

    return 0;
}