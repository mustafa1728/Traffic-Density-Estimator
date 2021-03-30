#include "Method3.hpp"
#include "Method3_Mutex.hpp"
#include "../Density.hpp"
#include<stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace cv;
using namespace std;

pthread_mutex_t spacial_lock;

void * spacialThreadWorkerBeta(void * arguments){

    Args_spacial * loc_arg = ((struct Args_spacial*)arguments);
    while(loc_arg->is_done != 2){

        if(loc_arg->is_done == 1){continue;}


        Mat img = loc_arg->image(loc_arg->crop_region);
        Mat back_crop = loc_arg->obj.background_img(loc_arg->crop_region);

        loc_arg->queue_density = loc_arg->obj.getQueueDensity(img, loc_arg->background_subtracted, back_crop);
                
        // cout<<"Thread "<<loc_arg->thread_i<<" x "<<loc_arg->thread_j<<" has worked on a frame now!\n";
        
        pthread_mutex_lock(&spacial_lock);
        loc_arg->is_done = 1;
        pthread_mutex_unlock(&spacial_lock);
    }
    pthread_exit(NULL);
    
}


void method3_beta(int argc, char** argv, const int x, const int y){
    pthread_t my_threads[x][y];

    VideoCapture cap;
    DensityCalculator instance(argc, argv);
    instance.getVideoCapture(cap);
    int frame_number = 0;
    Mat frame_temp, frame, cropped_frame;


    const int number_of_frames_avg = 10;
    vector<float> queue_densities;

    Mat prev_frame;

    String window_name = "Background removal";
    namedWindow(window_name, WINDOW_NORMAL);

    if (pthread_mutex_init(&spacial_lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        exit(0);
    }
    Args_spacial t_args[x][y];
    for(int i = 0; i<x; i++){
        for(int j = 0; j<y; j++){
            t_args[i][j].obj = instance;
            t_args[i][j].thread_i = i;
            t_args[i][j].thread_j = j;
            t_args[i][j].image = cropped_frame;

            pthread_mutex_lock(&spacial_lock);
            t_args[i][j].is_done = 1;
            pthread_mutex_unlock(&spacial_lock);
            
            int ret =  pthread_create(&my_threads[i][j], NULL, &spacialThreadWorkerBeta, (void*)&t_args[i][j]);
            if(ret != 0) {
                cout<<"Error: pthread not created properly\n";
                exit(1);
            }
        }
    }
    int all_free;
    while(true){
        bool bSuccess = cap.read(frame_temp);
        if(!bSuccess){
            cout << "The video is over!\n";
            
            break;
        }
        frame = frame_temp;
        instance.correctCameraAngleAndCrop(frame, cropped_frame);
        

        for(int i = 0; i<x; i++){
            for(int j = 0; j<y; j++){

                if(frame_number==0){
                    int height = cropped_frame.rows;
                    int width = cropped_frame.cols;

                    Rect crop_region((int)(i*width/x), (int)(j*height/y), (int)(width/x), (int)(height/y));
                    t_args[i][j].crop_region = crop_region;

                }
                t_args[i][j].image = cropped_frame;
                
                
                pthread_mutex_lock(&spacial_lock);
                t_args[i][j].is_done = 0;
                pthread_mutex_unlock(&spacial_lock);

            }
        }
        

        all_free = 0;
        while(all_free<x*y){


            all_free = 0;
            for(int i = 0; i<x; i++){
                for(int j = 0; j<y; j++){
                    all_free += t_args[i][j].is_done;
                }
            }
        }

        


        Mat horizontals[x];
        Mat concatenated;
        
        float queue_density = 0.0;
        for(int i = 0; i<x; i++){
            Mat verticals[y];
            for(int j = 0; j<y; j++){
                queue_density+=t_args[i][j].queue_density;
                verticals[j] = t_args[i][j].background_subtracted;
            }
            vconcat( verticals, y, horizontals[i] );
        }
        hconcat(horizontals, x, concatenated);
        imshow(window_name, concatenated);

        if (waitKey(1) == 27)
        {
            break;
        }

        queue_density = queue_density / max(x*y, 1);
        // cout<<queue_density<<endl;

        queue_densities.push_back(queue_density);


        frame_number+=1;
    }
    destroyAllWindows();
    // cout<<"here - destroyWindow!\n";
    for(int i = 0; i<x; i++){
        for(int j = 0; j<y; j++){
            pthread_mutex_lock(&spacial_lock);
            t_args[i][j].is_done = 2;
            pthread_mutex_unlock(&spacial_lock);
        }
    }
    // cout<<"here - close threads!\n";
    for(int i = 0; i<x; i++){
        for(int j = 0; j<y; j++){
            // pthread_join(my_threads[i][j], NULL);
            pthread_cancel(my_threads[i][j]);
        }
    }
    // cout<<"here - join!\n";
    pthread_mutex_destroy(&spacial_lock);


    vector<float> avg_queue_densities;
    for(int i = 0; i<queue_densities.size(); i++){
        float avg = 0.0;
        int num = 0;
        for(int j = i; j>=i; j--){
            if(num>number_of_frames_avg){
                break;
            }
            avg+=queue_densities[j];
            num+=1;
        }
        avg_queue_densities.push_back(avg/num);
    }

    string out_file_name = "out.txt";
    ofstream MyFile(out_file_name);
    cout<<"Frame Number \tQueue Density\n";
    MyFile<<"Frame Number, Queue Density\n";
    for(int i = 0; i<avg_queue_densities.size(); i++){
        cout<<i+1<<"\t\t"<<avg_queue_densities[i]<<endl;
        MyFile<<i+1<<", "<<avg_queue_densities[i]<<"\n";
    }
    MyFile.close();
}
