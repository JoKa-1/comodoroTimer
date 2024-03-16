#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

enum{
    START = 's',
    END = 'e',
    WARN = 'w',
};

//void menu(int *numCycl, int *wTime, int *bTime, int *brkTimeCycl);
int getArgAt(char **args, int IndxArg);
void playSound(char sound);
void halfCycle(int time);
void runTime(int time);

int main(int argc, char **argv){
    
    /*
    goal is provide [0]number_of_cycles [1]worktime [2]breaktime [3]break_between_cycles 
    */ 
    int numCycl = 0;
    int workTime = 0;
    int breakTime = 0;
    int brkTimeCycl = 0;
    
    if(argc == 4){
        numCycl = getArgAt(argv, 1);    
        workTime = getArgAt(argv, 2);    
        breakTime = getArgAt(argv, 3);    
    }
    else if(argc == 5){
        brkTimeCycl = getArgAt(argv, 4);
    }
    else{
        return 1;
    }

    for(int i=0; i<numCycl; i++){
        puts("Start work Time");
        halfCycle(workTime);
        puts("Start break Time");
        halfCycle(breakTime);

        if(numCycl > 1){
            puts("Big break");
            halfCycle(brkTimeCycl);
        }
    }

    return 0;
}

int getArgAt(char **args, int IndxArg){
    char c = args[IndxArg][0];
    int idx = 1;
    int arg = 0;
    while(c != '\0'){
        arg = (arg*10)+atoi(&c);
        c = args[IndxArg][idx++];
    }
    return arg;
}

void runTime(int time){
    sleep(60*(time-1));
    playSound('w');
    sleep(60);
}

void halfCycle(int time){
    playSound('s');
    runTime(time);
}

void playSound(char sound){
    switch(sound){
        case START: puts("Start Sound");
                    break;
        case WARN: puts("Warn Sound");
                   break;
        case END: puts("End Sound");
                  break;
        default : puts("No Sound");
                  break;
    }
}
