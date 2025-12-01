#include "pomodoro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
//#include "../raylib/clay_renderer_raylib.c"

static void runTime(int time) {
  /***
   * type of timer is abitrary
   * warn sound one minute before end
   ***/
  // idea: save like: int timerAsSeconds = 60 * (time - 1) and let procress
  // sleep for one second at a time, then in crease displayed timer in loop and
  // decrease timerAsSeconds by one
  sleep(60 * (time - 1));
  playSound(WARN);
  sleep(60);
}

static void halfCycle(int time) {
  /***
   * runs either work or breaktime
   * however what type of timer is abitrary
   ***/
  playSound(START);
  runTime(time);
}

static void playSound(char sound) {
  switch (sound) {
  case START:
    puts("Start Sound");
    break;
  case WARN:
    puts("Warn Sound\n");
    break;
  case END:
    puts("End Sound");
    break;
  default:
    puts("No Sound");
    break;
  }
}

double getCurrentTime() {
    return (double)clock() / CLOCKS_PER_SEC;
}

// these are only needed for CLI run 
//int _getArgAt(char **args, int IndxArg) {
//  char c = args[IndxArg][0];
//  int idx = 1;
//  int arg = 0;
//  while (c != '\0') {
//    arg = (arg * 10) + atoi(&c);
//    c = args[IndxArg][idx++];
//  }
//  return arg;
//}

//int main(void) {
//
//  int numCycl = 0;
//  int workTime = 0;
//  int breakTime = 0;
//  int brkTimeCycl = 0;
//
//  menu(&numCycl, &workTime, &breakTime, &brkTimeCycl);
//
//  for (int i = 0; i < numCycl; i++) {
//    printf("Start worktime %d min\n", workTime);
//    halfCycle(workTime);
//
//    printf("Start breaktime %d min\n", breakTime);
//    halfCycle(breakTime);
//
//    if (numCycl > 1) {
//      printf("Big break %d min\n", brkTimeCycl);
//      halfCycle(brkTimeCycl);
//    }
//  }
//  playSound(END);
//
//  return 0;
//}

//static void menu(int *numCycl, int *wTime, int *bTime, int *brkTimeCycl) {
//  int args = 0;
//
//  printf("How many Cycles do you want to work? ");
//  scanf("%d", &args); // Removed \n from the format string
//  *numCycl = args;
//
//  if (*numCycl > 1) {
//    printf("Please provide a break time for in between cycles: ");
//    scanf("%d", &args);  // Removed \n from the format string
//    *brkTimeCycl = args; // Corrected variable name
//  }
//
//  printf("How long should be the work time? ");
//  scanf("%d", &args); // Removed \n from the format string
//  *wTime = args;
//
//  printf("How long should be the break time? ");
//  scanf("%d", &args); // Removed \n from the format string
//  *bTime = args;
//
//  puts("");
//}
