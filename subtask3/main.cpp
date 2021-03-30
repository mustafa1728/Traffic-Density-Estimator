#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <pthread.h>
#include "Density.hpp"
#include "Method3.hpp"

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


void method3_beta(int argc, char** argv, const int x = 1, const int y = 1){
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

        if (waitKey(10) == 27)
        {
            break;
        }

        queue_density = queue_density / max(x*y, 1);
        cout<<queue_density<<endl;

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

    cout<<"Frame Number \tQueue Density\n";
    for(int i = 0; i<avg_queue_densities.size(); i++){
        cout<<i+1<<"\t\t"<<avg_queue_densities[i]<<endl;
    }
}

struct Args_temporal {
    int number_of_threads;
    int thread_number;

    int argc;
    char** argv;

    vector<float> queue_densities;
    vector<float> dynamic_densities;
};

void * temporalThreadWorker(void * arguments){

    int speed = 1;
    
    Args_temporal * loc_arg = ((struct Args_temporal*)arguments);

    // cout<<thread_id<<" "<<number_of_threads<<endl;

    DensityCalculator threadCalculator(loc_arg->argc, loc_arg->argv);
    threadCalculator.setHyperParameters(speed*loc_arg->number_of_threads, -1, -1, -1, -1, -1, loc_arg->thread_number,35,-1);

    threadCalculator.runDensityEsitmator(loc_arg->queue_densities, loc_arg->dynamic_densities, false);

    pthread_exit(NULL);
}

void method4(int argc, char** argv, const int number_of_threads = 10){

    pthread_t my_threads[number_of_threads];


    Args_temporal t_args[number_of_threads];

    for(long i = 0; i<number_of_threads; i++){

        

        t_args[i].number_of_threads = number_of_threads;
        t_args[i].argc = argc;
        t_args[i].argv = argv;
        t_args[i].thread_number = i;



        int ret =  pthread_create(&my_threads[i], NULL, &temporalThreadWorker, (void*)&t_args[i]);
        if(ret != 0) {
            cout<<"Error: pthread not created properly\n";
            exit(1);
        }
    }
    for(long i = 0; i<number_of_threads; i++){
        pthread_join(my_threads[i], NULL);
    }
    // pthread_exit(NULL);



    string out_file_name = "out.txt";
    ofstream MyFile(out_file_name);
    uint max_size =0;
    for(int c = 0; c<number_of_threads; c++){
        if(max_size < t_args[c].queue_densities.size()){
            max_size = t_args[c].queue_densities.size();
        }
    }
    
    int i = 0;
    int counter = 0;
    char buf[256];
    char pattern[]  = "%15s %15s %15s";
    sprintf(buf, pattern,"Frame Number", "Queue Density", "Dynamic Density");
    cout << buf<<endl;
    char pattern_loop[]  = "%15i %15f %15f";

    MyFile<<"Frame Number, Queue Density, Dynamic Density\n";
    while(i<max_size){
        for(int k = 0; k<number_of_threads; k++){

            counter+=1;

            if(i>=t_args[k].queue_densities.size()){ continue; }
            sprintf(buf, pattern_loop, counter, t_args[k].queue_densities[i], t_args[k].dynamic_densities[i]);
            cout << buf << '\n';
            MyFile<<counter<<", "<<t_args[k].queue_densities[i]<<", "<<t_args[k].dynamic_densities[i]<<"\n";
        }
        i+=1;

    }
    MyFile.close();

}



int main(int argc, char** argv)
{
    
//    DensityCalculator instance(argc, argv);
//    instance.setHyperParameters(1, 1, 1,8,10,10,8,35,-1);
//    instance.runDensityEsitmator();

    // method4(argc-1, argv, atoi(argv[1]));

     method3(argc, argv, 2, 2);
    // method3_beta(argc - 2, argv, atoi(argv[1]), atoi(argv[2]));
    // method3_beta(argc, argv, 2, 2);
    


    
    return 0;
}
