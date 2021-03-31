#include "Density.hpp"
#include<stdio.h>
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


    DensityCalculator::DensityCalculator(){
        setHyperParameters(1,1,1,8,10,10,8,35,-1);
        videoPath = "/Users/tamajit/Desktop/trafficvideo.mp4";
    }

    DensityCalculator::DensityCalculator(int argc, char** argv){

        Mat image;
        String userinput, backimgPath;

        setHyperParameters(1,1,1,8,10,10,8,35,-1);

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

        
        
        image = imread(backimgPath);
        if(!image.data)
        {
            cout << "Error: The background image was not loaded properly. Please check its path.\n";
            exit(1);
        }


        /// Some values needed for homography and cropping ///
        vector<Point2f> pts_src;
        
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



    void DensityCalculator::setHyperParameters(int speed = 1, int resize_w = 1, int resize_h = 1, int frames_skip = 8, int frames_avg = 10, int h = 10, int s = 8, int v = 35, int id = -1){
        if(speed!=-1){ speed_multiplier = speed; }
        if(frames_skip!=-1){ number_of_frames_skip = frames_skip; }
        if(frames_avg!=-1){ number_of_frames_avg = frames_avg; }
        if(h!=-1){ th_h = h; }
        if(s!=-1){ th_s = s; }
        if(v!=-1){ th_v = v; }
        if(resize_w!=-1){ resize_factor_width = resize_w; }
        if(resize_h!=-1){ resize_factor_height = resize_h; }
        thread_id = id;
    }

    void DensityCalculator::setspeed(int speed){
        speed_multiplier = speed;
    }

    void DensityCalculator::setwidth(int width){
        resize_factor_width = width;
    }
    
    void DensityCalculator::setheight(int height){
        resize_factor_height =height;
    }
    void DensityCalculator::setthreadId(int thread){
        thread_id =thread;
    }

/// Camera Angle correction and cropping ///

    void DensityCalculator::correctCameraAngleAndCrop(const Mat & frame, Mat & cropped_frame){
        Mat result;

        warpPerspective(frame, result, homographyMatrix, frame.size());
        Rect crop_region(x_1, y_1, x_2 - x_1, y_2 - y_1);
        cropped_frame = result(crop_region);
        resize(cropped_frame, cropped_frame, Size(0,0), (1/(double)resize_factor_width), (1/(double)resize_factor_height));
    }

/// Queue density ///

    float DensityCalculator::getQueueDensity(const Mat & cropped_frame, Mat & backgroundSubtracted, const Mat back_img){

        Mat background_img_hsv, cropped_frame_hsv, foreground_hsv, mask;

        cvtColor(cropped_frame, cropped_frame_hsv, COLOR_BGR2HSV);
        cvtColor(back_img, background_img_hsv, COLOR_BGR2HSV);
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
        float queue_density = sum / (mask.rows * mask.cols);
        // queue_densities.push_back(queue_density);
        Mat back_temp;
        cropped_frame.copyTo(back_temp, mask);
        backgroundSubtracted = back_temp;

        return queue_density;
        
    }
    float DensityCalculator::getQueueDensity(const Mat & cropped_frame, Mat & backgroundSubtracted){
        resize(background_img, background_img, Size(cropped_frame.cols,cropped_frame.rows));
        return getQueueDensity(cropped_frame, backgroundSubtracted, background_img);
    }



/// Dynamic density ///

    float DensityCalculator::getDynamicDensity(int frame_number, const Mat & cropped_frame, const Mat & frame, Mat & prev_frame, Mat & dynamicMask, Mat * prev){
//        if(frame_number == 0){
//            prev_frame = frame;
//        }
//        if(frame_number >= number_of_frames_skip){
//            prev_frame = prev[frame_number % number_of_frames_skip];
//        }
//        // pBackSub->apply(final_image, dynamicMask);
//        Mat background_img_hsv, cropped_frame_hsv, foreground_hsv;
//
//        resize(background_img, background_img, Size(0,0), (1/(double)resize_factor_width), (1/(double)resize_factor_height));
//
//        cvtColor(cropped_frame, cropped_frame_hsv, COLOR_BGR2HSV);
//        cvtColor(background_img, background_img_hsv, COLOR_BGR2HSV);
//
//        Mat prev_homography, prev_result, prev_result_hsv, back_temp;
//
//        warpPerspective(prev_frame, prev_homography, homographyMatrix, frame.size());
//        Rect crop_region(x_1, y_1, x_2 - x_1, y_2 - y_1);
//        prev_result = prev_homography(crop_region);
//        resize(prev_result, prev_result, Size(0,0), (1/(double)resize_factor_width), (1/(double)resize_factor_height));
//        cvtColor(prev_result, prev_result_hsv, COLOR_BGR2HSV);
//        absdiff(cropped_frame_hsv, prev_result_hsv, foreground_hsv);
//        inRange(foreground_hsv, Scalar(th_h, th_s, th_v), Scalar(180, 255, 255), dynamicMask);
//
//        dilate(dynamicMask, dynamicMask, Mat(), Point(-1, -1), 2, 1, 1);
//
//        float sum = 0;
//        for (int i = 0; i < dynamicMask.rows; ++i) {
//            for (int j = 0; j < dynamicMask.cols; ++j) {
//                if(dynamicMask.at<int>(i,j)!=0){
//                    sum = sum + 1;
//                }
//            }
//        }
//        cropped_frame.copyTo(back_temp, dynamicMask);
//        dynamicMask = back_temp;
//        float dynamic_density = sum / (dynamicMask.rows * dynamicMask.cols);
//        // dynamic_densities.push_back(dynamic_density);
//        prev[frame_number % number_of_frames_skip] = frame;
//        return dynamic_density;
        return 0.0;
    }


/// Displaying Intermediate images, printing to console and writing to file


    void DensityCalculator::displayIntermediate(const Mat & cropped_frame, const Mat & backgroundSubtracted, const Mat & dynamicMask, string window_name){
        
        // cvtColor(dynamicMask, dynamicMask, COLOR_GRAY2BGR);

        Mat matArray[] = { cropped_frame, backgroundSubtracted, dynamicMask,};
        Mat concatenated;
        hconcat( matArray, 3, concatenated );
        imshow(window_name, concatenated);

        // imshow("Original video cropped", final_image);
        // imshow("Background masked video", mask);
        // imshow("Dynamic Density mask", dynamicMask);
    }

    void DensityCalculator::displayDensities(int frame_number, float * density_prev, float * dynamic_prev, float queue_density, float dynamic_density){
        

        density_prev[frame_number % number_of_frames_avg] = queue_density;
        dynamic_prev[frame_number % number_of_frames_avg] = dynamic_density;
        float avg_density = 0, avg_dynamic_density = 0;
        for( int i = 0; i<number_of_frames_avg; i++){
            avg_density += density_prev[i];
            avg_dynamic_density += dynamic_prev[i];
        }
        avg_density = avg_density / number_of_frames_avg;
        avg_dynamic_density = avg_dynamic_density / number_of_frames_avg;
    
        if(frame_number == 1){
            cout << "Frame Number, Queue Density, Dynamic Density\n";
        }
        for(int i =0; i<speed_multiplier; i++){
            cout<<speed_multiplier*(frame_number-1) + 1 + i<<", "<<avg_density<<", "<<avg_dynamic_density<<'\n';
        }  

    }

    string DensityCalculator::writeDensities(int frame_number, float * density_prev, float * dynamic_prev, float queue_density, float dynamic_density){
        density_prev[frame_number % number_of_frames_avg] = queue_density;
        dynamic_prev[frame_number % number_of_frames_avg] = dynamic_density;
        float avg_density = 0, avg_dynamic_density = 0;
        for( int i = 0; i<number_of_frames_avg; i++){
            avg_density += density_prev[i];
            avg_dynamic_density += dynamic_prev[i];
        }
        avg_density = avg_density / number_of_frames_avg;
        avg_dynamic_density = avg_dynamic_density / number_of_frames_avg;


        string toWrite = "Frame Number, Queue Density, Dynamic Density\n";
        if(frame_number != 0){      toWrite = "";    }

        for(int i =0; i<speed_multiplier; i++){
            toWrite.append(to_string(speed_multiplier*(frame_number-1) + 1 + i));
            toWrite.append(", ");
            toWrite.append(to_string(avg_density));
            toWrite.append(", ");
            toWrite.append(to_string(avg_dynamic_density));
            toWrite.append("\n");
        }
        

        return toWrite;
    }

    void DensityCalculator::setAvgQueueDensities(int frame_number, float * density_prev, float * dynamic_prev, float queue_density, float dynamic_density, vector<float> &queue_densities, vector<float> &dynamic_densities){
        density_prev[frame_number % number_of_frames_avg] = queue_density;
        dynamic_prev[frame_number % number_of_frames_avg] = dynamic_density;
        float avg_density = 0, avg_dynamic_density = 0;
        for( int i = 0; i<number_of_frames_avg; i++){
            avg_density += density_prev[i];
            avg_dynamic_density += dynamic_prev[i];
        }
        avg_density = avg_density / number_of_frames_avg;
        avg_dynamic_density = avg_dynamic_density / number_of_frames_avg;
        queue_densities.push_back(avg_density);
        dynamic_densities.push_back(avg_dynamic_density);
    }

    bool DensityCalculator::readFrame(VideoCapture & cap, Mat & frame){
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

    void DensityCalculator::runDensityEsitmator(vector<float>& queue_densities, vector<float>& dynamic_densities, bool toDisplay = true){

        VideoCapture cap(videoPath);
        if (cap.isOpened() == false)
        {
            cout<<"Error: The video was not loaded properly. Please check its path.\n";
            exit(0);
        }

        
        string window_name;
        if(toDisplay){
            window_name = "Original Cropped video, background removal and Dynamic Density mask";
            namedWindow(window_name, WINDOW_NORMAL);
        }

        ofstream MyFile(out_file_name);

        Mat* prev;
        float * density_prev;
        float * dynamic_prev;

        prev = new Mat[number_of_frames_skip];
        density_prev = new float[number_of_frames_avg];
        dynamic_prev = new float[number_of_frames_avg];
        for( int i = 0; i<number_of_frames_avg; i++){
            density_prev[i] = 0;
            dynamic_prev[i] = 0;
        }

        for(int i = 0; i<thread_id; i++){   bool random = cap.grab();   }
        // if(thread_id!=-1){ frame_number+=thread_id; }
        Mat frame, prev_frame, cropped_frame, background_subtracted, dynamic_mask;
        int frame_number = 0;
        float queue_density = 0.0;
        float dynamic_density = 0.0;

        float avg_queue_density = 0.0;
        float avg_dynamic_density = 0.0;

        while(true)
        {
            if (!readFrame(cap, frame)){     cout << "The video is over!\n"; break;      }

            correctCameraAngleAndCrop(frame, cropped_frame);

            queue_density = getQueueDensity(cropped_frame, background_subtracted);
            
            dynamic_density = getDynamicDensity(frame_number, cropped_frame, frame, prev_frame, dynamic_mask, prev);

            if(toDisplay){
                displayIntermediate(cropped_frame, background_subtracted, dynamic_mask, window_name);

                if (waitKey(1) == 27){     break;      }
            }
            else{
                setAvgQueueDensities(frame_number, density_prev, dynamic_prev, queue_density, dynamic_density, queue_densities, dynamic_densities);
                if(thread_id!=-1){
                    // cout<<"The thread "<<thread_id<<" is operating on its frame number "<<frame_number+1<<'\n';
                }
                
            }

            frame_number+=1;

            if(thread_id==-1){
                
                MyFile<<writeDensities(frame_number, density_prev, dynamic_prev, queue_density, dynamic_density);

                displayDensities(frame_number, density_prev, dynamic_prev, queue_density, dynamic_density);  
                
            }            
        }
        MyFile.close();

    }

    void DensityCalculator::runDensityEsitmator(bool toDisplay){
        vector<float> random1, random2;
        runDensityEsitmator(random1, random2, toDisplay);
    }
    void DensityCalculator::runDensityEsitmator(){
        vector<float> random1, random2;
        runDensityEsitmator(random1, random2, true);
    }

    void DensityCalculator::getVideoCapture(VideoCapture & cap_local){
        cap_local.open(videoPath);
    }
