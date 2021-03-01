#include <opencv2/opencv.hpp>
#include <iostream>


using namespace cv;
using namespace std;

Mat image;

int i = 0;
vector<Point2f> pts_src;
int x_1, x_2, y_1, y_2;

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

    int height = image.rows;
    int width = image.cols;

    x_1 = max(0, width / 2 - height / 6);
    x_2 = min(width, width / 2 + height / 6);
    y_1 = height / 2 - height / 3;
    y_2 = height / 2 + height / 3;

}

int main(int argc, char** argv)
{
    
    image = imread("empty.jpg");

    namedWindow("COP290 - Task1 - Subtask 2", 1);

    setMouseCallback("COP290 - Task1 - Subtask 2", CallBackFunc, NULL);

    imshow("COP290 - Task1 - Subtask 2", image);

    waitKey(0);

    VideoCapture cap("../../trafficvideo.mp4"); 
    if (cap.isOpened() == false)  
    {
        cout << "Cannot open the video file" << endl;
        cin.get();
        return -1;
    }

    String window_name = "Angle corrected and cropped video";

    namedWindow(window_name, WINDOW_NORMAL);

    namedWindow("Original video", WINDOW_NORMAL);

    while (true)
    {
        Mat frame;
        bool bSuccess = cap.read(frame); 

        if (bSuccess == false) 
        {
            cout << "Found the end of the video" << endl;
            break;
        }

        vector<Point2f> pts_dst;
        pts_dst.push_back(Point2f(x_1, y_1));
        pts_dst.push_back(Point2f(x_2, y_1));
        pts_dst.push_back(Point2f(x_2, y_2));
        pts_dst.push_back(Point2f(x_1, y_2));

        Mat h = findHomography(pts_src, pts_dst);
        Mat result;
        warpPerspective(frame, result, h, frame.size());

        Mat final_image;

        Rect crop_region(x_1, y_1, x_2 - x_1, y_2 - y_1);

        final_image = result(crop_region);

        imshow(window_name, final_image);
        imshow("Original video", frame);
        
        if (waitKey(10) == 27)
        {
            break;
        }
    }

    return 0;
}