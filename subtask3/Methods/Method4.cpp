#include "Method4.hpp"
#include "../Density.hpp"
#include<stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace cv;
using namespace std;



void * temporalThreadWorker(void * arguments){

    int speed = 1;
    
    Args_temporal * loc_arg = ((struct Args_temporal*)arguments);

    // cout<<thread_id<<" "<<number_of_threads<<endl;

    DensityCalculator threadCalculator(loc_arg->argc, loc_arg->argv);
    threadCalculator.setHyperParameters(speed*loc_arg->number_of_threads, -1, -1, -1, -1, -1, loc_arg->thread_number,35,-1);

    threadCalculator.runDensityEsitmator(loc_arg->queue_densities, loc_arg->dynamic_densities, false);

    pthread_exit(NULL);
}

void method4(int argc, char** argv, const int number_of_threads){

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
