#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <ctime>


using namespace cv;
using namespace std;



int main(int argc, char** argv)
{
    Mat image, background_img;
    String videoPath, backimgPath = "background.jpg";



    /// Command line arguments and help instructions ///
    if (argc == 1) {
        videoPath = "../../trafficvideo.mp4";
    }
    else if (argc==2){
        videoPath = argv[1];
    }
    else if (argc==3){
        videoPath = argv[1];
        backimgPath = argv[2];
    }
    else{
        cout << "Error: Incorrect Command. Please run the command in the following format:\n";
        cout << "./<filename>     or    ./<filename> <path_to_video>\n";
        return -1;
    }

    /// Reading the video and background image ///

    VideoCapture cap(videoPath); 
    if (cap.isOpened() == false)  
    {
        cout<<"Error: The video was not loaded properly. Please check its path.\n";
        return -1;
    }
    
    image = imread(backimgPath);
    if(!image.data)
    {
        cout << "Error: The background image was not loaded properly. Please check its path.\n";
        return -1;
    }





    /// Some values needed for homography and cropping ///

    vector<Point2f> pts_src;
    pts_src.push_back(Point2f(990, 207));
    pts_src.push_back(Point2f(1280, 207));
    pts_src.push_back(Point2f(1572, 1080));
    pts_src.push_back(Point2f(260, 1080));

    int x_1, x_2, y_1, y_2;
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

    Mat result_back;
    warpPerspective(image, result_back, h, image.size());
    Rect crop_region(x_1, y_1, x_2 - x_1, y_2 - y_1);
    background_img = result_back(crop_region);




    /// Some parameters that can be tuned ///

    int th_h = 10, th_s = 8, th_v = 35;
    const int number_of_frames_skip = 8;
    const int number_of_frames_avg = 10;

    /// Initialisations and declarations ///


    int frame_number= 0;

    time_t original_time = time(0);
    long long int curr_time = 0, time_count = 0;

    ofstream MyFile("out.txt");
    
    char buf[256];
    char pattern[]  = "%15s %15s %15s";
    sprintf(buf, pattern, "Frame Number", "Queue Density", "Dynamic Density");
    cout << buf << endl;

    MyFile <<"Frame Number, Queue Density, Dynamic Density"<<'\n';

    Mat frame, result, final_image, foreground;
    Mat final_image_hsv, background_img_hsv, foreground_hsv, mask, dynamicMask;

    Mat prev_image, prev_homography, prev_result, prev_result_hsv;
    Mat prev[number_of_frames_skip];
    Mat concatenated;


    float density_prev[number_of_frames_avg];
    float dynamic_prev[number_of_frames_avg];
    for( int i = 0; i<number_of_frames_avg; i++){
        density_prev[i] = 0;
        dynamic_prev[i] = 0;
    }
    // Ptr<BackgroundSubtractor> pBackSub;
    // pBackSub = createBackgroundSubtractorKNN(1, 400.0, false);

    String window_name = "Original Cropped video, background removal and Dynamic Density mask";
    namedWindow(window_name, WINDOW_NORMAL);

    while(true)
    {
        Mat frame_temp;
        bool bSuccess = cap.read(frame_temp);
        frame = frame_temp;

        if (bSuccess == false) 
        {
            cout << "The video is over!" << endl;
            break;
        }


        warpPerspective(frame, result, h, frame.size());
        Rect crop_region(x_1, y_1, x_2 - x_1, y_2 - y_1);
        final_image = result(crop_region);





        /// Calculating Queue density ///

        cvtColor(final_image, final_image_hsv, COLOR_BGR2HSV);
        cvtColor(background_img, background_img_hsv, COLOR_BGR2HSV);
        absdiff(final_image_hsv, background_img_hsv, foreground_hsv);
        inRange(foreground_hsv, Scalar(th_h, th_s, th_v), Scalar(180, 255, 255), mask);

        dilate(mask, mask, Mat(), Point(-1, -1), 2, 1, 1);
        dilate(mask, mask, Mat(), Point(-1, -1), 2, 1, 1);        

        float density = 0;
        for (int i = 0; i < mask.rows; ++i) {
            for (int j = 0; j < mask.cols; ++j) {
    
                if(mask.at<int>(i,j)!=0){
                    density = density + 1;
                }
                
            }
        }

        density = density / (mask.rows * mask.cols);
        final_image.copyTo(mask, mask);





        /// Calculating Dynamic density ///

        if(frame_number == 0){
            prev_image = frame;
        }
        if(frame_number >= number_of_frames_skip){
            prev_image = prev[frame_number % number_of_frames_skip];
        }

        // pBackSub->apply(final_image, dynamicMask);
        warpPerspective(prev_image, prev_homography, h, prev_image.size());
        prev_result = prev_homography(crop_region);
        cvtColor(prev_result, prev_result_hsv, COLOR_BGR2HSV);       
        absdiff(final_image_hsv, prev_result_hsv, foreground_hsv);
        inRange(foreground_hsv, Scalar(th_h, th_s, th_v), Scalar(180, 255, 255), dynamicMask);
        
        dilate(dynamicMask, dynamicMask, Mat(), Point(-1, -1), 2, 1, 1);

        float dynamic_density = 0;
        for (int i = 0; i < dynamicMask.rows; ++i) {
            for (int j = 0; j < dynamicMask.cols; ++j) {
                if(dynamicMask.at<int>(i,j)!=0){
                    dynamic_density = dynamic_density + 1;
                }
            }
        }
        dynamic_density = dynamic_density / (dynamicMask.rows * dynamicMask.cols);
        
        prev[frame_number % number_of_frames_skip] = frame;




        /// Using Optical Flow ///
//         Mat flow(prev_image.size(), CV_32FC2);
//         method(mask_prev,mask_next, flow, std::forward<Args>(args)...);

        /// Visualization part///
//         Mat flow_parts[2];
//         split(flow, flow_parts);

        /// Convert the algorithm's output into Polar coordinates
//         Mat magnitude, angle, magn_norm;
//         cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
//         normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
//         angle *= ((1.f / 360.f) * (180.f / 255.f));

        /// Build hsv image
//         Mat _hsv[3], hsv, hsv8, bgr;
//         _hsv[0] = angle;
//         _hsv[1] = Mat::ones(angle.size(), CV_32F);
//         _hsv[2] = magn_norm;
//         merge(_hsv, 3, hsv);
//         hsv.convertTo(hsv8, CV_8U, 255.0);
        
        /// Display the results
//         cvtColor(hsv8, bgr, COLOR_HSV2BGR);
//         prvs = next;




        /// Displaying intermediate steps ///

        cvtColor(dynamicMask, dynamicMask, COLOR_GRAY2BGR);

        Mat matArray[] = { final_image, mask, dynamicMask,};
        hconcat( matArray, 3, concatenated );
        imshow(window_name, concatenated);

        // imshow("Original video cropped", final_image);
        // imshow("Background masked video", mask);
        // imshow("Dynamic Density mask", dynamicMask);




        /// incrementing counter ///

        frame_number+=1;

        /// getting average of some previous frames ///

        density_prev[frame_number % number_of_frames_avg] = density;
        dynamic_prev[frame_number % number_of_frames_avg] = dynamic_density;
        float avg_density = 0, avg_dynamic_density = 0;
        for( int i = 0; i<number_of_frames_avg; i++){
            avg_density += density_prev[i];
            avg_dynamic_density += dynamic_prev[i];
        }
        avg_density = avg_density / number_of_frames_avg;
        avg_dynamic_density = avg_dynamic_density / number_of_frames_avg;

        /// writing to file ///
        avg_dynamic_density = min(avg_dynamic_density, avg_density);

        MyFile << frame_number<<", ";
        MyFile << avg_density <<", ";
        MyFile << avg_dynamic_density<<'\n';

        /// Printing in console ///

        char buf[256];
        char pattern[]  = "%15i %15f %15f";
        sprintf(buf, pattern, frame_number, avg_density, avg_dynamic_density);
        cout<<buf<<'\n';
        

        if (waitKey(10) == 27)
        {
            break;
        }
    }

    MyFile.close();

    return 0;
}