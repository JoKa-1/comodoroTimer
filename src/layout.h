#ifndef LAYOUT_H
#define LAYOUT_H

#include "../clay.h"
#include "timerLogic.h"
#include "../raylib/raylib.h"

// UI App-State: encapsulates button and event info as a struct
typedef struct LayoutUIState {
    bool timerButtonPressed;
    bool resetButtonPressed;
    // Extend for more UI event state as needed
} LayoutUIState;

// UI rendering initialization
void layout_init(void);
// Redraw current frame, updated with latest fonts/timer state and UI state
void layout_redraw_frame(Font *fonts, PomodoroTimerState *timer, Clay_String *timerString, Clay_String *workTimeString, Clay_String *breakTimeString, Clay_String *cycleString, LayoutUIState *uiState);
// Handle central clay/raylib error handling
void layout_handle_errors(Clay_ErrorData errorData);

#endif
