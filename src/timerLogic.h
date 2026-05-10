#ifndef TIMERLOGIC_H
#define TIMERLOGIC_H
#include "../clay.h"
#include <stdbool.h>

// Timer sound events
enum {
    START = 's',
    END = 'e',
    WARN = 'w',
};

// Centralized timer state
typedef struct PomodoroTimerState {
    double startTime;
    double timerDuration;
    double baseWorkTime;
    double workTime;
    double baseBreakTime;
    double breakTime;
    bool initializeTimer;
    bool workTimerIsRunning;
    bool breakTimerIsRunning;
    char TIMERSTRING[12]; // "MM:SS"
} PomodoroTimerState;

// Timer logic API using the state struct
void PomodoroTimer_Init(PomodoroTimerState *timer, double baseWork, double baseBreak);
double PomodoroTimer_getCurrentTime(void);
void PomodoroTimer_start(PomodoroTimerState *timer, double duration);
void PomodoroTimer_stop(PomodoroTimerState *timer);
void PomodoroTimer_reset(PomodoroTimerState *timer, Clay_String *timerString);
void PomodoroTimer_run(PomodoroTimerState *timer, Clay_String *timerString);
bool PomodoroTimer_isRunning(PomodoroTimerState *timer);
void PomodoroTimer_action(PomodoroTimerState *timer, bool *ButtonPressed, Clay_String cycleString);

double *PomodoroTimer_getWorkTime(PomodoroTimerState *timer);
double *PomodoroTimer_getBreakTime(PomodoroTimerState *timer);
void PomodoroTimer_playSound(char sound);

void setWindowSizeBound(void);

#endif
