#include "timerLogic.h"
#include <stdio.h>
#include <time.h>
#include <string.h> // for strlen, snprintf
#include <stdlib.h>

// Timer logic state encapsulation

void PomodoroTimer_Init(PomodoroTimerState *timer, double baseWork, double baseBreak) {
    timer->baseWorkTime = baseWork;
    timer->baseBreakTime = baseBreak;
    timer->workTime = baseWork;
    timer->breakTime = baseBreak;
    timer->initializeTimer = false;
    timer->workTimerIsRunning = false;
    timer->breakTimerIsRunning = false;
    memset(timer->TIMERSTRING, 0, sizeof(timer->TIMERSTRING));
    snprintf(timer->TIMERSTRING, sizeof(timer->TIMERSTRING), "%02d:%02d", (int)(baseWork/60), 0);
    timer->startTime = 0;
    timer->timerDuration = 0;
}
double PomodoroTimer_getCurrentTime(void) { return (double)clock() / CLOCKS_PER_SEC; }
double *PomodoroTimer_getWorkTime(PomodoroTimerState *timer) { return &timer->workTime; }
double *PomodoroTimer_getBreakTime(PomodoroTimerState *timer) { return &timer->breakTime; }
bool PomodoroTimer_isRunning(PomodoroTimerState *timer) { return timer->workTimerIsRunning || timer->breakTimerIsRunning; }

void PomodoroTimer_start(PomodoroTimerState *timer, double duration) {
    timer->startTime = PomodoroTimer_getCurrentTime();
    timer->timerDuration = duration;
    timer->workTimerIsRunning = true;
}

void PomodoroTimer_stop(PomodoroTimerState *timer) {
    timer->workTimerIsRunning = false;
    timer->breakTimerIsRunning = false;
    timer->initializeTimer = false;
}

void PomodoroTimer_reset(PomodoroTimerState *timer, Clay_String *timerString) {
    PomodoroTimer_stop(timer);
    snprintf(timer->TIMERSTRING, sizeof(timer->TIMERSTRING), "45:00");
    timerString->chars = timer->TIMERSTRING;
    timerString->length = (int32_t)strlen(timer->TIMERSTRING);
    timerString->isStaticallyAllocated = false;
}

void PomodoroTimer_run(PomodoroTimerState *timer, Clay_String *timerString) {
    if (PomodoroTimer_isRunning(timer)) {
        double elapsed = PomodoroTimer_getCurrentTime() - timer->startTime;
        double remaining = timer->timerDuration - elapsed;
        int minutes = (int)(remaining / 60);
        int seconds = (int)(remaining) % 60;
        // Check if timer has finished
        if (minutes == 0 && seconds == 0) {
            remaining = 0;
            timer->breakTimerIsRunning = false;
            timer->workTimerIsRunning = false;
        }
        snprintf(timer->TIMERSTRING, sizeof(timer->TIMERSTRING), "%02d:%02d", minutes, seconds);
        timerString->chars = timer->TIMERSTRING;
        timerString->length = (int32_t)strlen(timer->TIMERSTRING);
        timerString->isStaticallyAllocated = false;
    }
}

void PomodoroTimer_action(PomodoroTimerState *timer, bool *ButtonPressed, Clay_String cycleString) {
    if (!timer->initializeTimer) {
        PomodoroTimer_start(timer, *PomodoroTimer_getWorkTime(timer));
        timer->initializeTimer = true;
    } else {
        PomodoroTimer_stop(timer);
        if (atoi(cycleString.chars) > 0 && !timer->initializeTimer) {
            PomodoroTimer_start(timer, *PomodoroTimer_getBreakTime(timer));
            timer->breakTimerIsRunning = true;
        }
    }
    if (ButtonPressed) *ButtonPressed = false;
}

void PomodoroTimer_playSound(char sound) {
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

// these are only needed for CLI run
// static void runTime(int time) {
//  /***
//   * type of timer is abitrary
//   * warn sound one minute before end
//   ***/
//  // idea: save like: int timerAsSeconds = 60 * (time - 1) and let procress
//  // sleep for one second at a time, then in crease displayed timer in loop
//  and
//  // decrease timerAsSeconds by one
//  sleep(60 * (time - 1));
//  playSound(WARN);
//  sleep(60);
//}
//
// static void halfCycle(int time) {
//  /***
//   * runs either work or breaktime
//   * however what type of timer is abitrary
//   ***/
//  playSound(START);
//  runTime(time);
//}
// int _getArgAt(char **args, int IndxArg) {
//  char c = args[IndxArg][0];
//  int idx = 1;
//  int arg = 0;
//  while (c != '\0') {
//    arg = (arg * 10) + atoi(&c);
//    c = args[IndxArg][idx++];
//  }
//  return arg;
//}

// int main(void) {
//
//   int numCycl = 0;
//   int workTime = 0;
//   int breakTime = 0;
//   int brkTimeCycl = 0;
//
//   menu(&numCycl, &workTime, &breakTime, &brkTimeCycl);
//
//   for (int i = 0; i < numCycl; i++) {
//     printf("Start worktime %d min\n", workTime);
//     halfCycle(workTime);
//
//     printf("Start breaktime %d min\n", breakTime);
//     halfCycle(breakTime);
//
//     if (numCycl > 1) {
//       printf("Big break %d min\n", brkTimeCycl);
//       halfCycle(brkTimeCycl);
//     }
//   }
//   playSound(END);
//
//   return 0;
// }

// static void menu(int *numCycl, int *wTime, int *bTime, int *brkTimeCycl) {
//   int args = 0;
//
//   printf("How many Cycles do you want to work? ");
//   scanf("%d", &args); // Removed \n from the format string
//   *numCycl = args;
//
//   if (*numCycl > 1) {
//     printf("Please provide a break time for in between cycles: ");
//     scanf("%d", &args);  // Removed \n from the format string
//     *brkTimeCycl = args; // Corrected variable name
//   }
//
//   printf("How long should be the work time? ");
//   scanf("%d", &args); // Removed \n from the format string
//   *wTime = args;
//
//   printf("How long should be the break time? ");
//   scanf("%d", &args); // Removed \n from the format string
//   *bTime = args;
//
//   puts("");
// }
