#ifndef Density_hpp
#define Density_hpp

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void CallBackFunc(int event, int x, int y, int flags, void* pts_src);
class DensityCalculator{

/// declaring and initialising useful variables ///
private:
    
    int speed_multiplier;
    int number_of_frames_skip, number_of_frames_avg;
    int th_h, th_s, th_v;
    int thread_id;
    int resize_factor_width, resize_factor_height;
    string out_file_name;
    String videoPath;
    Mat homographyMatrix;
    int x_1, x_2, y_1, y_2;

public:
    
    Mat background_img;
    
private:
    void setHyperParameters(int speed, int resize_w, int resize_h, int frames_skip, int frames_avg, int h, int s, int v, int id);

    bool readFrame(VideoCapture & cap, Mat & frame);
    
    void displayIntermediate(const Mat & cropped_frame, const Mat & backgroundSubtracted, const Mat & dynamicMask, string window_name);
    void displayDensities(int frame_number, float * density_prev, float * dynamic_prev, float queue_density, float dynamic_density);
    string writeDensities(int frame_number, float * density_prev, float * dynamic_prev, float queue_density, float dynamic_density);
    void setAvgQueueDensities(int frame_number, float * density_prev, float * dynamic_prev, float queue_density, float dynamic_density, vector<float> &queue_densities, vector<float> &dynamic_densities);
    
public:
    
    DensityCalculator();
    DensityCalculator(int argc, char** argv);

    void setspeed(int speed);
    void setwidth(int width);
    void setheight(int height);
    void setthreadId(int thread);
    
    void correctCameraAngleAndCrop(const Mat & frame, Mat & cropped_frame);
    float getQueueDensity(const Mat & cropped_frame, Mat & backgroundSubtracted, const Mat back_img);
    float getQueueDensity(const Mat & cropped_frame, Mat & backgroundSubtracted);
    float getDynamicDensity(int frame_number, const Mat & cropped_frame, const Mat & frame, Mat & prev_frame, Mat & dynamicMask, Mat * prev);
    
    

    void runDensityEsitmator(vector<float>& queue_densities, vector<float>& dynamic_densities, bool toDisplay);
    void runDensityEsitmator(bool toDisplay);
    void runDensityEsitmator();

    void getVideoCapture(VideoCapture & cap_local);

};

#endif /* Density_hpp */
