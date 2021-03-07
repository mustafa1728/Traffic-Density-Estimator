#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>


using namespace cv;
using namespace std;

Mat image, background_img;

int i = 0;
vector<Point2f> pts_src;
int x_1, x_2, y_1, y_2;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{

    if (event == EVENT_LBUTTONDOWN || event == EVENT_RBUTTONDOWN || event == EVENT_MBUTTONDOWN) {
        pts_src.push_back(Point2f(x, y));
        ++i;
        // cout<<x<<<<y;
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


    vector<Point2f> pts_dst;
    pts_dst.push_back(Point2f(x_1, y_1));
    pts_dst.push_back(Point2f(x_2, y_1));
    pts_dst.push_back(Point2f(x_2, y_2));
    pts_dst.push_back(Point2f(x_1, y_2));

    Mat h = findHomography(pts_src, pts_dst);
    Mat result;
    warpPerspective(image, result, h, image.size());

    Rect crop_region(x_1, y_1, x_2 - x_1, y_2 - y_1);

    background_img = result(crop_region);
    // imshow("background", background_img);

}

int main(int argc, char** argv)
{
    int threshold = 40;
    int th_h = 10, th_s = 8, th_v = 35;


    image = imread("screenshot.jpg");

    namedWindow("COP290 - Task1 - Subtask 2", 1);

    setMouseCallback("COP290 - Task1 - Subtask 2", CallBackFunc, NULL);

    imshow("COP290 - Task1 - Subtask 2", image);

    waitKey(0);

    int count = 0;

    VideoCapture cap("../../trafficvideo.mp4"); 
    if (cap.isOpened() == false)  
    {
        cout << "Cannot open the video file" << endl;
        cin.get();
        return -1;
    }

    String window_name = "Angle corrected and cropped video";

    namedWindow(window_name, WINDOW_NORMAL);

    namedWindow("background subtracted", WINDOW_NORMAL);

    ofstream MyFile("out.txt");
    int i = 0;
    while (true)
    {

        Mat frame;
        
        bool bSuccess = cap.read(frame); 

        if (bSuccess == false) 
        {
            cout << "Found the end of the video" << endl;
            break;
        }
        
        
        i+=1;
        if(i%10<9){
            continue;
        }


        // while(i!=4){
        //     image = frame;
        //     namedWindow("Chose the four points", 1);
        //     setMouseCallback("Chose the four points", CallBackFunc, NULL);
        //     imshow("Chose the four points", image);
        //     waitKey(0);
        // }

        vector<Point2f> pts_dst;
        pts_dst.push_back(Point2f(x_1, y_1));
        pts_dst.push_back(Point2f(x_2, y_1));
        pts_dst.push_back(Point2f(x_2, y_2));
        pts_dst.push_back(Point2f(x_1, y_2));

        Mat h = findHomography(pts_src, pts_dst);
        Mat result;
        warpPerspective(frame, result, h, frame.size());

        Mat final_image, foreground;

        Rect crop_region(x_1, y_1, x_2 - x_1, y_2 - y_1);

        final_image = result(crop_region);
        // count+=1;
        // if (count == 1){
        //     background_img = final_image;
        // }

        // foreground = final_image - background_img;
        Mat final_image_hsv, background_img_hsv, foreground_hsv;

        cvtColor(final_image, final_image_hsv, COLOR_BGR2HSV);
        cvtColor(background_img, background_img_hsv, COLOR_BGR2HSV);

        absdiff(final_image_hsv, background_img_hsv, foreground_hsv);

        

        inRange(foreground_hsv, Scalar(th_h, th_s, th_v), Scalar(180, 255, 255), foreground_hsv);

        Mat mask = foreground_hsv;
        dilate(mask, mask, Mat(), Point(-1, -1), 2, 1, 1);
        imshow("mask", mask);

        // cout<<"starting";
        float density = 0;
        for (int i = 0; i < mask.rows; ++i) {
            for (int j = 0; j < mask.cols; ++j) {
     
                // Add the element
                // cout<<mask.at<int>(i,j);
                if(mask.at<int>(i,j)!=0){
                    density = density + 1;
                    // density = density + mask.at<int>(i,j);
                }
            // cout<<'\n'; 
                
            }
        }

        density = density / (mask.rows * mask.cols);
        // cout<<density<<'\n';
        MyFile << density<<'\n';

        // foreground = final_image * foreground;
        final_image_hsv.copyTo(foreground_hsv, foreground_hsv);

        cvtColor(foreground_hsv, foreground, COLOR_HSV2BGR);
        
        imshow(window_name, final_image);
        imshow("background subtracted", foreground);
        // imshow("Original video", frame);
        
        if (waitKey(10) == 27)
        {
            break;
        }
    }

    MyFile.close();
    return 0;
}