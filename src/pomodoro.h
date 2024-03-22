
#ifndef POMODORO_H
#define POMODORO_H

enum{
    START = 's',
    END = 'e',
    WARN = 'w',
};

static void menu(int *numCycl, int *wTime, int *bTime, int *brkTimeCycl);
static void askbrkTimeCycl(int *btm, int *args);
static void playSound(char sound);
static void halfCycle(int time);
static void runTime(int time);

#endif
