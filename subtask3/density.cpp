#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <pthread.h>


using namespace cv;
using namespace std;


void CallBackFunc(int event, int x, int y, int flags, void* pts_src)
{

    vector<Point2f>*p = (vector<Point2f>*)pts_src;
    if (event == EVENT_LBUTTONDOWN || event == EVENT_RBUTTONDOWN || event == EVENT_MBUTTONDOWN) {
        p->push_back(Point2f(x, y));
    }
    else {
        return;
    }
}




class DensityCalculator{

/// declaring and initialising useful variables ///
private:
    Mat image, background_img;
    String videoPath, backimgPath;
    String userinput;
    
    int speed_multiplier;
    int number_of_frames_skip, number_of_frames_avg;
    int th_h, th_s, th_v;


    string out_file_name;

    VideoCapture cap;

    Mat homographyMatrix;
    vector<Point2f> pts_src;

    int x_1, x_2, y_1, y_2;

public:
    DensityCalculator(int argc, char** argv){

        out_file_name = "out.txt";

        /// handling command line arguments ///
        backimgPath = "background.jpg";
        userinput = "0";
        videoPath = "../../trafficvideo.mp4";
        if (argc == 1) {
           ;
        }
        else if (argc==2){
            videoPath = argv[1];
        }
        else if (argc==3){
            videoPath = argv[1];
            backimgPath = argv[2];
        }
        else if (argc==4){
            videoPath = argv[1];
            backimgPath = argv[2];
            userinput = argv[3];
        }
        else{
            cout << "Error: Incorrect Command. Please run the command in the following format:\n";
            cout << "./<filename> or \n";
            cout<<"./<filename> <path to video> or \n";
            cout<<"./<filename> <path to video> <path to back image> or \n";
            cout<<"./<filename> <path to video> <path to back image> <to take user input (0: no, 1: yes)>\n";
            exit(1);
        }

        /// Reading the video and background image ///

        cap.open(videoPath); 
        if (cap.isOpened() == false)  
        {
            cout<<"Error: The video was not loaded properly. Please check its path.\n";
            exit(1);
        }
        
        image = imread(backimgPath);
        if(!image.data)
        {
            cout << "Error: The background image was not loaded properly. Please check its path.\n";
            exit(1);
        }


        /// Some values needed for homography and cropping ///

        if(userinput.compare("1") == 0){
            String user_input_window_name = "Chose 4 points to crop. (order: top left, top right, bottom right, bottom left)";
            namedWindow(user_input_window_name, WINDOW_NORMAL);
            setMouseCallback(user_input_window_name, CallBackFunc, &pts_src);
            imshow(user_input_window_name, image);
            waitKey(0);
            destroyWindow(user_input_window_name);
        }
        else{
            pts_src.push_back(Point2f(990, 207));
            pts_src.push_back(Point2f(1280, 207));
            pts_src.push_back(Point2f(1572, 1080));
            pts_src.push_back(Point2f(260, 1080));
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

        homographyMatrix = findHomography(pts_src, pts_dst);

        Mat result_back;
        warpPerspective(image, result_back, homographyMatrix, image.size());
        Rect crop_region(x_1, y_1, x_2 - x_1, y_2 - y_1);
        background_img = result_back(crop_region);
    }



    void setHyperParameters(int speed = 5, int frames_skip = 8, int frames_avg = 10, int h = 10, int s = 8, int v = 35){
        speed_multiplier = speed;
        number_of_frames_skip = frames_skip;
        number_of_frames_avg = frames_avg;
        th_h = h;
        th_s = s; 
        th_v = v;        
    }

/// Camera Angle correction and cropping ///

private:
    Mat cropped_frame;

private: 
    void correctCameraAngleAndCrop(){
        Mat result; 
        warpPerspective(frame, result, homographyMatrix, frame.size());
        Rect crop_region(x_1, y_1, x_2 - x_1, y_2 - y_1);
        cropped_frame = result(crop_region);

    }

/// Queue density /// 
private:
    float queue_density;
    Mat backgroundSubtracted;


private: 
    void updateQueueDensity(){

        Mat background_img_hsv, cropped_frame_hsv, foreground_hsv, mask;

        cvtColor(cropped_frame, cropped_frame_hsv, COLOR_BGR2HSV);
        cvtColor(background_img, background_img_hsv, COLOR_BGR2HSV);
        absdiff(cropped_frame_hsv, background_img_hsv, foreground_hsv);
        inRange(foreground_hsv, Scalar(th_h, th_s, th_v), Scalar(180, 255, 255), mask);

        dilate(mask, mask, Mat(), Point(-1, -1), 2, 1, 1);
        dilate(mask, mask, Mat(), Point(-1, -1), 2, 1, 1);        

        float sum = 0;
        for (int i = 0; i < mask.rows; ++i) {
            for (int j = 0; j < mask.cols; ++j) {
    
                if(mask.at<int>(i,j)!=0){
                    sum = sum + 1;
                }
                
            }
        }

        queue_density = sum / (mask.rows * mask.cols);
        Mat back_temp;
        cropped_frame.copyTo(back_temp, mask);
        backgroundSubtracted = back_temp;
        
    }



/// Dynamic density ///

private:
    float dynamic_density;
    int frame_number;
    Mat prev_frame;
    Mat dynamicMask;
    Mat* prev;


private: 
    void updateDynamicDensity(){
        if(frame_number == 0){
            prev_frame = frame;
        }
        if(frame_number >= number_of_frames_skip){
            prev_frame = prev[frame_number % number_of_frames_skip];
        }

        // pBackSub->apply(final_image, dynamicMask);
        Mat background_img_hsv, cropped_frame_hsv, foreground_hsv;

        cvtColor(cropped_frame, cropped_frame_hsv, COLOR_BGR2HSV);
        cvtColor(background_img, background_img_hsv, COLOR_BGR2HSV);

        Mat prev_homography, prev_result, prev_result_hsv;

        warpPerspective(prev_frame, prev_homography, homographyMatrix, frame.size());
        Rect crop_region(x_1, y_1, x_2 - x_1, y_2 - y_1);
        prev_result = prev_homography(crop_region);
        cvtColor(prev_result, prev_result_hsv, COLOR_BGR2HSV);       
        absdiff(cropped_frame_hsv, prev_result_hsv, foreground_hsv);
        inRange(foreground_hsv, Scalar(th_h, th_s, th_v), Scalar(180, 255, 255), dynamicMask);
        
        dilate(dynamicMask, dynamicMask, Mat(), Point(-1, -1), 2, 1, 1);

        float sum = 0;
        for (int i = 0; i < dynamicMask.rows; ++i) {
            for (int j = 0; j < dynamicMask.cols; ++j) {
                if(dynamicMask.at<int>(i,j)!=0){
                    sum = sum + 1;
                }
            }
        }
        dynamic_density = sum / (dynamicMask.rows * dynamicMask.cols);
        
        prev[frame_number % number_of_frames_skip] = frame;

        
    }


/// Displaying Intermediate images, printing to console and writing to file
private:
    float * density_prev;
    float * dynamic_prev;

public:

    void displayIntermediate(){
        cvtColor(dynamicMask, dynamicMask, COLOR_GRAY2BGR);

        Mat matArray[] = { cropped_frame, backgroundSubtracted, dynamicMask,};
        Mat concatenated;
        hconcat( matArray, 3, concatenated );
        imshow(window_name, concatenated);

        // imshow("Original video cropped", final_image);
        // imshow("Background masked video", mask);
        // imshow("Dynamic Density mask", dynamicMask);
    }

    string displayWriteDensities(){
        

        density_prev[frame_number % number_of_frames_avg] = queue_density;
        dynamic_prev[frame_number % number_of_frames_avg] = dynamic_density;
        float avg_density = 0, avg_dynamic_density = 0;
        for( int i = 0; i<number_of_frames_avg; i++){
            avg_density += density_prev[i];
            avg_dynamic_density += dynamic_prev[i];
        }
        avg_density = avg_density / number_of_frames_avg;
        avg_dynamic_density = avg_dynamic_density / number_of_frames_avg;

        char buf[256];
        
        if(frame_number == 0){
            char pattern[]  = "%15s %15s %15s";
            sprintf(buf, pattern, "Frame Number", "Queue Density", "Dynamic Density");
            cout << buf << '\n';
        }
        char pattern[]  = "%15i %15f %15f";
        sprintf(buf, pattern, frame_number + 1, avg_density, avg_dynamic_density);
        cout<<buf<<'\n';

        string toWrite = "Frame Number, Queue Density, Dynamic Density\n";
        if(frame_number != 0){      toWrite = "";    }

        toWrite.append(to_string(frame_number+1));
        toWrite.append(", ");
        toWrite.append(to_string(avg_density));
        toWrite.append(", ");
        toWrite.append(to_string(avg_dynamic_density));
        toWrite.append("\n");

        return toWrite;

    }

    bool readFrame(){
        Mat frame_temp;
        bool bSuccess;
        for (int i = 0; i<speed_multiplier; i++){
             bSuccess = cap.grab();
        }
        bSuccess = cap.retrieve(frame_temp);

        frame = frame_temp;

        return bSuccess;
    }





/// Main loop ///
private: 
    String window_name;
    Mat frame;

public:
    void runDensityEsitmator(){

        frame_number = 0;
        queue_density = 0.0;
        dynamic_density = 0.0;
        window_name = "Original Cropped video, background removal and Dynamic Density mask";
        namedWindow(window_name, WINDOW_NORMAL);

        ofstream MyFile(out_file_name);


        prev = new Mat[number_of_frames_skip];
        density_prev = new float[number_of_frames_avg];
        dynamic_prev = new float[number_of_frames_avg];
        for( int i = 0; i<number_of_frames_avg; i++){
            density_prev[i] = 0;
            dynamic_prev[i] = 0;
        }

        while(true)
        {
            if (!readFrame()){     cout << "The video is over!\n"; break;      }

            correctCameraAngleAndCrop();

            updateQueueDensity();

            updateDynamicDensity();

            displayIntermediate();

            MyFile<<displayWriteDensities();

            if (waitKey(10) == 27){     break;      }

            frame_number+=1;
        }
        MyFile.close();

    }


};


int main(int argc, char** argv)
{
    DensityCalculator instance(argc, argv);
    instance.setHyperParameters(1, 8, 10);

    instance.runDensityEsitmator();

    return 0;
}