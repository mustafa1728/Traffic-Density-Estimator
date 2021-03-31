#include "Method3.hpp"
#include "../Density.hpp"
#include<stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace cv;
using namespace std;



void * spacialThreadWorker(void * arguments){

    Args_spacial * loc_arg = ((struct Args_spacial*)arguments);

    Mat crop_img = loc_arg->image(loc_arg->crop_region);
    Mat back_crop = loc_arg->obj.background_img(loc_arg->crop_region);

    loc_arg->queue_density = loc_arg->obj.getQueueDensity(crop_img, loc_arg->background_subtracted, back_crop);
    Mat temp = loc_arg->frame;
    loc_arg->dynamic_density = loc_arg->obj.getDynamicDensity(loc_arg->frame_number, loc_arg->image, temp, loc_arg->prev_frame, loc_arg->dynamicMask, loc_arg->prev);   
    // cout<<"Thread "<<loc_arg->thread_i<<" x "<<loc_arg->thread_j<<" has worked on a frame now!\n";
    pthread_exit(NULL);
    
}

void method3(int argc, char** argv, const int x, const int y, bool toDisplay){
    pthread_t my_threads[x][y];


    VideoCapture cap;
    DensityCalculator instance(argc, argv);
    instance.getVideoCapture(cap);
    int frame_number = 0;
    Mat frame_temp, frame, cropped_frame;


    const int number_of_frames_avg = 10;
    const int number_of_frames_skip = 8;

    vector<float> queue_densities;
    vector<float> dynamic_densities;
    

    Mat prev_frame;

    String window_name = "Background removal";
    if(toDisplay){
        namedWindow(window_name, WINDOW_NORMAL);
    }

    Mat horizontals_queue[x];
    Mat horizontals_dynamic[x];
    Mat concatenated_queue, concatenated_dynamic, concatenated;

    Args_spacial t_args[x][y];
    while(true){
        bool bSuccess = cap.read(frame_temp);
        if(!bSuccess){
            cout << "The video is over!\n"; break;
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
                    t_args[i][j].obj = instance;
                    t_args[i][j].thread_i = i;
                    t_args[i][j].thread_j = j;
                    t_args[i][j].prev = new Mat[number_of_frames_skip];
                }
                t_args[i][j].image = cropped_frame;
                t_args[i][j].frame = frame;
                t_args[i][j].frame_number = frame_number;


                int ret =  pthread_create(&my_threads[i][j], NULL, &spacialThreadWorker, (void*)&t_args[i][j]);
                if(ret != 0) {
                    cout<<"Error: pthread not created properly\n";
                    exit(1);
                }


            }
        }

        float queue_density = 0.0;
        float dynamic_density = 0.0;
        for(int i = 0; i<x; i++){
            Mat verticals_queue[y];
            Mat verticals_dynamic[y];
            for(int j = 0; j<y; j++){

                pthread_join(my_threads[i][j], NULL);


                queue_density+=t_args[i][j].queue_density;
                dynamic_density+=t_args[i][j].dynamic_density;
                if(toDisplay){
                    verticals_queue[j] = t_args[i][j].background_subtracted;
                    verticals_dynamic[j] = t_args[i][j].background_subtracted;
                }
            }
            if(toDisplay) { 
                vconcat( verticals_queue, y, horizontals_queue[i] ); 
                vconcat( verticals_dynamic, y, horizontals_dynamic[i] ); 
            }
        }
        if(toDisplay){ 
            hconcat(horizontals_queue, x, concatenated_queue); 
            hconcat(horizontals_dynamic, x, concatenated_dynamic); 
            // Mat matArray[] = { concatenated_queue, concatenated_dynamic,};
            // hconcat(matArray, 2, concatenated);

            imshow(window_name, concatenated_queue);
            if (waitKey(1) == 27)
            {
                break;
            }
        }

        
        

        queue_density = queue_density / max(x*y, 1);
        dynamic_density = dynamic_density / max(x*y, 1);
        // cout<<queue_density<<" "<<dynamic_density<<endl;

        queue_densities.push_back(queue_density);
        dynamic_densities.push_back(dynamic_density);


        frame_number+=1;
    }

    string out_file_name = "out.txt";
    ofstream MyFile(out_file_name);
    cout<<"Frame Number, Queue Density, Dynamic Density\n";
    MyFile<<"Frame Number, Queue Density, Dynamic Density\n";
    for(int i = 0; i<queue_densities.size(); i++){
        float avg_queue = 0.0;
        float avg_dynamic = 0.0;
        int num = 0;
        for(int j = i; j>=i; j--){
            if(num>number_of_frames_avg){
                break;
            }
            avg_queue+=queue_densities[j];
            avg_dynamic += dynamic_densities[j];
            num+=1;
        }
        cout<<i+1<<", "<<avg_queue/num<<", "<<avg_dynamic/num<<'\n';
        MyFile<<i+1<<", "<<avg_queue/num<<", "<<avg_dynamic/num<<'\n';
    }

    MyFile.close();
}
