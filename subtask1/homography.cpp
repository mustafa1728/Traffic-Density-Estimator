#include <opencv2/opencv.hpp>
#include <iostream>


using namespace cv;
using namespace std;

Mat image, grey_image, result;

int i = 0;
vector<Point2f> pts_src;
string warp_save_path("warp.jpg");
string crop_save_path("crop.jpg");

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

    int height = grey_image.rows;
    int width = grey_image.cols;

    vector<Point2f> pts_dst;
    int x1, x2, y1, y2;

    x1 = max(0, width / 2 - height / 6);
    x2 = min(width, width / 2 + height / 6);
    y1 = height / 2 - height / 3;
    y2 = height / 2 + height / 3;

    pts_dst.push_back(Point2f(x1, y1));
    pts_dst.push_back(Point2f(x2, y1));
    pts_dst.push_back(Point2f(x2, y2));
    pts_dst.push_back(Point2f(x1, y2));

    Mat h = findHomography(pts_src, pts_dst);

    warpPerspective(grey_image, result, h, grey_image.size());

    imshow("Warped Image", result);

    imwrite(warp_save_path, result);

    waitKey(0);

    Mat final_image;

    Rect crop_region(x1, y1, x2 - x1, y2 - y1);

    final_image = result(crop_region);

    imshow("cropped image ", final_image);

    imwrite(crop_save_path, final_image);

}

int main(int argc, char** argv)
{
    if (argc == 1) {
        image = imread("images/traffic.jpg");
    }
    else if (argc == 2) {
        image = imread(argv[1]);
    }
    else if (argc == 4) {
        image = imread(argv[1]);
        warp_save_path.assign(argv[2]);
        crop_save_path.assign(argv[3]);
    }
    else {
        cout << "Incorrect Command. Please run the command in the following format:\n";
        cout << "./<filename>     or    ./<filename> <path_to_image>   or   ./<filename> <path_to_image> <warp_save_path> <crop_save_path>\n";
        return 0;
    }

    cvtColor(image, grey_image, COLOR_BGR2GRAY);

    namedWindow("COP290 - Subtask 1", 1);

    setMouseCallback("COP290 - Subtask 1", CallBackFunc, NULL);

    imshow("COP290 - Subtask 1", grey_image);

    waitKey(0);

    return 0;
}