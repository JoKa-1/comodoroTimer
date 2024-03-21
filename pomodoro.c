#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

enum{
    START = 's',
    END = 'e',
    WARN = 'w',
};

static void playSound(char sound);
static void halfCycle(int time);
static void runTime(int time);
static void menu(int *numCycl, int *wTime, int *bTime, int *brkTimeCycl);
static void askbrkTimeCycl(int *btm, int *args);

int main(void){
    
    int numCycl = 0;
    int workTime = 0;
    int breakTime = 0;
    int brkTimeCycl = 0;

    menu(&numCycl, &workTime, &breakTime, &brkTimeCycl); 
    
    for(int i=0; i<numCycl; i++){
        printf("Start worktime %d min\n", workTime);
        halfCycle(workTime);

        printf("Start breaktime %d min\n", breakTime);
        halfCycle(breakTime);

        if(numCycl > 1){
            printf("Big break %d min\n", brkTimeCycl);
            halfCycle(brkTimeCycl);
        }
    }
    playSound('e');

    return 0;
}

static void runTime(int time){
    sleep(60*(time-1));
    playSound('w');
    sleep(60);
}

static void halfCycle(int time){
    playSound('s');
    runTime(time);
}

static void askbrkTimeCycl(int *btm, int *args){
    printf("Please provide a break time for in between cycles ");
    scanf("%d\n", args);
    *btm = *args;
}

static void menu(int *numCycl, int *wTime, int *bTime, int *brkTimeCycl){
    int args = 0;
    printf("How many Cycles to you want to work?\n");
    scanf("%d\n", &args);
        *numCycl = args;    
    if(*numCycl > 1){
        askbrkTimeCycl(brkTimeCycl, &args);
    }
    printf("How long should be the work time? ");
    scanf("%d\n", &args);
        *wTime = args;    
    printf("And the break time? ");
    scanf("%d\n\n", &args);
        *bTime = args;    

}

static void playSound(char sound){
    switch(sound){
        case START: puts("Start Sound");
                    break;
        case WARN: puts("Warn Sound\n");
                   break;
        case END: puts("End Sound");
                  break;
        default : puts("No Sound");
                  break;
    }
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

