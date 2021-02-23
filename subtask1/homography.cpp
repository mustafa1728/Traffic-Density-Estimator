#include <opencv2/opencv.hpp>
#include <iostream>


using namespace cv;
using namespace std;

Mat image, grey_image, result;

int i = 0;
vector<Point2f> pts_src;
string warp_save_path("images/warp.jpg");
string crop_save_path("images/crop.jpg");

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

    imwrite(warp_save_path, result);

    waitKey(0);

    Mat final_image;

    Rect crop_region(720, 135, 360, 945);

    final_image = result(crop_region);

    imshow("cropped image ", final_image);

    imwrite(crop_save_path, final_image);

}

int main(int argc, char** argv)
{
    if (argc==1){
        image = imread("images/traffic.jpg");
    }
    else if (argc==2){
        image = imread(argv[1]);
    }
    else if (argc==4){
        image = imread(argv[1]);
        warp_save_path.assign(argv[2]);
        crop_save_path.assign(argv[3]);
    }
    else{
        cout<<"Incorrect Command. Please run the command in the following format:\n";
        cout<<"./<filename>     or    ./<filename> <path_to_image>   or   ./<filename> <path_to_image> <warp_save_path> <crop_save_path>\n";
        return 0;
    }

    cvtColor(image, grey_image, COLOR_BGR2GRAY);

    namedWindow("COP290 - Subtask 1", 1);

    setMouseCallback("COP290 - Subtask 1", CallBackFunc, NULL);

    imshow("COP290 - Subtask 1", grey_image);

    waitKey(0);

    return 0;
}