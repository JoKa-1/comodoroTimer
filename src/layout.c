#include <stdbool.h>
#define CLAY_IMPLEMENTATION
#include "../clay.h"
#include "../raylib/clay_renderer_raylib.c"
#include "pomodoro.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for strlen, snprintf

void HandleClayErrors(Clay_ErrorData errorData);
Clay_RenderCommandArray CreateLayout(void);
void RedrawFrame(Font *fonts);

/* Prototypes from other translation units: use proper prototype form */
double getCurrentTime(void);

const uint32_t FONT_ID_BODY_24 = 0;
const uint32_t FONT_ID_BODY_16 = 1;
const Clay_Color BGGREY = {82, 82, 82, 255};
const Clay_Color EGGSHELLWHITE = {222, 222, 222, 255};
const Clay_Color SNOWWHITE = {255, 255, 255, 255};
const Clay_Color ACTIVEGREEN = {39, 160, 53, 255};
const Clay_Color INACTIVEGREY = {124, 124, 124, 255};
const Clay_Color BUTTONEGREY = {163, 163, 163, 255};
const char *WINDOW_TITLE = "Pomodoro Timer";

static char TIMERSTRING[12]; // "MM:SS"
static double startTime = 0;
static double timerDuration = 0;
static double baseWorkTime = 45 * 60;
static double baseBreakTime = 15 * 60;
static bool initializeTimer = false;
static bool timerIsRunning = false;

Clay_String timerString = {.chars = "45:00",
                           .length = (int32_t)strlen("45:00"),
                           .isStaticallyAllocated = true};
Clay_String workTimeString = {.chars = "45:00",
                              .length = (int32_t)strlen("45:00"),
                              .isStaticallyAllocated = true};
Clay_String breakTimeString = {.chars = "15:00",
                               .length = (int32_t)strlen("45:00"),
                               .isStaticallyAllocated = true};
Clay_String cycleString = {.chars = "3",
                           .length = (int32_t)strlen("3"),
                           .isStaticallyAllocated = true};

// ButtonData
bool TimerButtonPressed = false;

// global bool for errorHandeling
bool reinitializeClay = false;

int main(void) {
  // Clay data
  Clay_Dimensions dimensions =
      (Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()};
  Clay_ErrorHandler errorHandler = (Clay_ErrorHandler){HandleClayErrors, 0};
  uint64_t clayRequiredMemory = Clay_MinMemorySize();
  Clay_Arena clayMemory = (Clay_Arena){
      .memory = malloc(clayRequiredMemory),
      .capacity = clayRequiredMemory,
  };

  // has to be done before Font init
  Clay_Raylib_Initialize(1024, 768, WINDOW_TITLE, FLAG_WINDOW_RESIZABLE);
  Clay_Initialize(clayMemory, dimensions, errorHandler);

  // render data
  Font fonts[2];
  fonts[FONT_ID_BODY_24] =
      LoadFontEx("resources/Roboto-Regular.ttf", 128, 0, 400);
  SetTextureFilter(fonts[FONT_ID_BODY_24].texture, TEXTURE_FILTER_BILINEAR);
  fonts[FONT_ID_BODY_16] =
      LoadFontEx("resources/Roboto-Regular.ttf", 32, 0, 400);
  SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);
  Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

  setWindowSizeBound();

  // render loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    if (reinitializeClay) {
      Clay_SetMaxElementCount(8192);
      clayRequiredMemory = Clay_MinMemorySize();
      clayMemory = Clay_CreateArenaWithCapacityAndMemory(
          clayRequiredMemory, malloc(clayRequiredMemory));
      Clay_Initialize(
          clayMemory,
          (Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()},
          (Clay_ErrorHandler){HandleClayErrors, 0});
      reinitializeClay = false;
    }
    RedrawFrame(fonts);
  }

  Clay_Raylib_Close();
  return 0;
}

void setWindowSizeBound(){
  SetWindowMaxSize((int)(GetMonitorWidth(0)/2), GetMonitorHeight(0)); // FLAG_WINDOW_RESIZABLE
  SetWindowMinSize(401, 410); // FLAG_WINDOW_RESIZABLE

}

void HandleClayErrors(Clay_ErrorData errorData) {
  printf("%s", errorData.errorText.chars);
  if (errorData.errorType == CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED) {
    reinitializeClay = true;
    Clay_SetMaxElementCount(Clay_GetMaxElementCount() * 2);
  } else if (errorData.errorType ==
             CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED) {
    reinitializeClay = true;
    Clay_SetMaxMeasureTextCacheWordCount(
        Clay_GetMaxMeasureTextCacheWordCount() * 2);
  }
}

void RedrawFrame(Font *fonts) {

  static Clay_Vector2 lastPointer = {-1, -1};
  Vector2 mp = GetMousePosition();
  bool mouseDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
  if (mouseDown) {
    lastPointer.x = mp.x;
    lastPointer.y = mp.y;
    Clay_SetPointerState((Clay_Vector2){mp.x, mp.y}, true);
  } else {
    Clay_SetPointerState(lastPointer, false);
  }
  // printf("Mouse: x: %f | y: %f\n", mp.x, mp.y);
  printf("Width: %d | Height: %d\n", GetScreenWidth(), GetScreenHeight());

  Clay_SetLayoutDimensions((Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()});
  Clay_RenderCommandArray renderCommands = CreateLayout();
  BeginDrawing();
  ClearBackground(BLACK);
  Clay_Raylib_Render(renderCommands, fonts);
  EndDrawing();
}

void HandleButtonInteraction(Clay_ElementId elementId,
                             Clay_PointerData pointerInfo, void *userData) {
  (void)elementId;
  bool *buttonPressed = (bool *)userData;
  if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    *buttonPressed = !(*buttonPressed);
  }
}

void startTimer(double durationInSeconds, bool *refreshTimer) {
  startTime = getCurrentTime();
  timerDuration = durationInSeconds;
  *refreshTimer = true;
  initializeTimer = true;
}

void stopTimer(bool *refreshTimer) {
  *refreshTimer = false;
  initializeTimer = false;

  snprintf(TIMERSTRING, sizeof(TIMERSTRING), "45:00");
  timerString.chars = TIMERSTRING;
  timerString.length = (int32_t)strlen(TIMERSTRING);
}

void runTimer() {

  if (timerIsRunning) {
    double elapsed = getCurrentTime() - startTime;
    double remaining = timerDuration - elapsed;

    int minutes = (int)(remaining / 60);
    int seconds = (int)(remaining) % 60;
    // Check if timer has finished
    if (minutes == 0 && seconds == 0) {
      remaining = 0;
      timerIsRunning = false;
    }
    snprintf(TIMERSTRING, sizeof(TIMERSTRING), "%02d:%02d", minutes, seconds);
    timerString.chars = TIMERSTRING;
    timerString.length = (int32_t)strlen(TIMERSTRING);
    timerString.isStaticallyAllocated = false;
  }
}

void setTimer(Clay_String *timerString, int min, int sec) {
  snprintf(TIMERSTRING, sizeof(TIMERSTRING), "%02d:%02d", min, sec);
  timerString->chars = TIMERSTRING;
  timerString->length = (int32_t)strlen(TIMERSTRING);
  timerString->isStaticallyAllocated = false;
}

Clay_RenderCommandArray CreateLayout(void) {
  /*TODO: fetch windowsize and set some breakpoints to change fontsize
        - breakpoints in Vue
          small	    sm    400px > < 750px
          medium    md    750px > < MaxHeight
  */

  // init Functions
  runTimer();
  Clay_BeginLayout();

  CLAY(CLAY_ID("Background"), {.layout =
                                   {
                                       .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                       .sizing = {.width = CLAY_SIZING_GROW(),
                                                  .height = CLAY_SIZING_GROW()},
                                   },
                               .backgroundColor = BGGREY}) {
    CLAY(CLAY_ID("Row1"),
         {.layout =
              {
                  .layoutDirection = CLAY_LEFT_TO_RIGHT,
                  .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                     .y = CLAY_ALIGN_Y_CENTER},
                  .sizing = {.width = CLAY_SIZING_GROW(),
                             .height = CLAY_SIZING_GROW(80)},
                  .padding = {10, 10, 50, 10}, // {left, bottom, top, right}
                  .childGap = 20,
              },
          .backgroundColor = BGGREY}) {
      // add work , break time and cycles here

      CLAY(CLAY_ID("WorkTimer"),
           {.layout =
                {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .childAlignment =
                        {
                            .x = CLAY_ALIGN_X_CENTER,
                            .y = CLAY_ALIGN_Y_CENTER,
                        },
                    .sizing = {.width = CLAY_SIZING_PERCENT(0.3),
                               .height = CLAY_SIZING_PERCENT(0.3)},
                },
            .border = {.width = {1, 1, 1, 1, 0}, .color = EGGSHELLWHITE},
            .cornerRadius = {12, 12, 12, 12}}) {
        CLAY(CLAY_ID("WorkTimeLabel"),
             {
                 .layout =
                     {
                         .childAlignment =
                             {
                                 .x = CLAY_ALIGN_X_CENTER,
                                 .y = CLAY_ALIGN_Y_TOP,
                             },
                         .sizing = {.width = CLAY_SIZING_PERCENT(0.3),
                                    .height = CLAY_SIZING_PERCENT(0.3)},
                         .padding = {0, 0, 0, 0},
                     },
                 .backgroundColor = BGGREY,
             }) {
          Clay_String label = {.chars = "Work",
                               .length = (int32_t)strlen("Work"),
                               .isStaticallyAllocated = true};
          CLAY_TEXT(label, CLAY_TEXT_CONFIG(
                               {.fontId = FONT_ID_BODY_24,
                                .fontSize = 16,
                                .textColor = timerIsRunning ? ACTIVEGREEN
                                                            : EGGSHELLWHITE}));
        }

        CLAY_TEXT(workTimeString,
                  CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_24,
                                    .fontSize = 48,
                                    .textColor = EGGSHELLWHITE}));
      }

      CLAY(CLAY_ID("BreakTimer"),
           {.layout =
                {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .childAlignment =
                        {
                            .x = CLAY_ALIGN_X_CENTER,
                            .y = CLAY_ALIGN_Y_CENTER,
                        },
                    .sizing = {.width = CLAY_SIZING_PERCENT(0.3),
                               .height = CLAY_SIZING_PERCENT(0.3)},
                },
            .border = {.width = {1, 1, 1, 1, 0}, .color = EGGSHELLWHITE},
            .cornerRadius = {12, 12, 12, 12}}) {
        CLAY(CLAY_ID("BreakTimeLabel"),
             {
                 .layout =
                     {
                         .childAlignment =
                             {
                                 .x = CLAY_ALIGN_X_CENTER,
                                 .y = CLAY_ALIGN_Y_TOP,
                             },
                         .sizing = {.width = CLAY_SIZING_PERCENT(0.3),
                                    .height = CLAY_SIZING_PERCENT(0.3)},
                         .padding = {0, 0, 0, 0},
                     },
                 .backgroundColor = BGGREY,
             }) {
          Clay_String label = {.chars = "Break",
                               .length = (int32_t)strlen("Break"),
                               .isStaticallyAllocated = true};
          CLAY_TEXT(label, CLAY_TEXT_CONFIG(
                               {.fontId = FONT_ID_BODY_24,
                                .fontSize = 16,
                                .textColor = timerIsRunning ? ACTIVEGREEN
                                                            : EGGSHELLWHITE}));
        }

        CLAY_TEXT(breakTimeString,
                  CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_24,
                                    .fontSize = 48,
                                    .textColor = EGGSHELLWHITE}));
      }

      CLAY(CLAY_ID("Cycles"),
           {.layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM,
                       .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                          .y = CLAY_ALIGN_Y_CENTER},
                       .sizing = {.width = CLAY_SIZING_PERCENT(0.3),
                                  .height = CLAY_SIZING_PERCENT(0.3)}},
            .border = {.width = {1, 1, 1, 1, 0}, .color = EGGSHELLWHITE},
            .cornerRadius = {12, 12, 12, 12}}) {

        CLAY(CLAY_ID("CycleLabel"),
             {
                 .layout =
                     {
                         .childAlignment =
                             {
                                 .x = CLAY_ALIGN_X_CENTER,
                                 .y = CLAY_ALIGN_Y_TOP,
                             },
                         .sizing = {.width = CLAY_SIZING_PERCENT(0.3),
                                    .height = CLAY_SIZING_PERCENT(0.3)},
                         .padding = {0, 0, 0, 0},
                     },
                 .backgroundColor = BGGREY,
             }) {
          Clay_String label = {.chars = "Cycle",
                               .length = (int32_t)strlen("Cycle"),
                               .isStaticallyAllocated = true};
          CLAY_TEXT(label, CLAY_TEXT_CONFIG(
                               {.fontId = FONT_ID_BODY_24,
                                .fontSize = 16,
                                .textColor = timerIsRunning ? ACTIVEGREEN
                                                            : EGGSHELLWHITE}));
        }

        CLAY_TEXT(cycleString, CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_24,
                                                 .fontSize = 48,
                                                 .textColor = EGGSHELLWHITE}));
      }
    }

    CLAY(CLAY_ID("Row2"),
         {.layout =
              {
                  .layoutDirection = CLAY_TOP_TO_BOTTOM,
                  .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                     .y = CLAY_ALIGN_Y_CENTER},
                  .sizing = {.width = CLAY_SIZING_GROW(0),
                             .height = CLAY_SIZING_GROW(80)},
                  .padding = {0, 0, 50, 0}, //{left, right, top, bottom}
              },
          .backgroundColor = BGGREY}) {
      CLAY(CLAY_ID("CurrentTimer"),
           {
               .layout = {.childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                             .y = CLAY_ALIGN_Y_CENTER},
                          .sizing = {.width = CLAY_SIZING_PERCENT(0.4),
                                     .height = CLAY_SIZING_PERCENT(0.8)}},
               .backgroundColor = BGGREY, //{0, 0, 0, 255},

           }) {
        // display the timer string
        CLAY_TEXT(timerString, CLAY_TEXT_CONFIG({.fontSize = 128,
                                                 .textColor = EGGSHELLWHITE}));
      }
    }

    CLAY(CLAY_ID("Row3"),
         {.layout =
              {
                  .layoutDirection = CLAY_TOP_TO_BOTTOM,
                  .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                     .y = CLAY_ALIGN_Y_CENTER},
                  .sizing = {.width = CLAY_SIZING_GROW(0),
                             .height = CLAY_SIZING_GROW(80)},
                  .padding = {0, 0, 0, 0}, //{left, right, top, bottom}
              },
          .backgroundColor = BGGREY}) {
      // add timer controls here
      // An orange button that turns blue when hovered
      CLAY(CLAY_ID("TimerStartStopButton"),
           {
               .layout = {.sizing = {.width = CLAY_SIZING_PERCENT(0.15),
                                     .height = CLAY_SIZING_PERCENT(0.2)},
                          .padding = {20, 20, 20,
                                      20}, //{left, right, top, bottom}
                          .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                             .y = CLAY_ALIGN_Y_CENTER}},
               .cornerRadius = {12, 12, 12, 12},
               .border = {.width = {1, 1, 1, 1, 0}, .color = EGGSHELLWHITE},
           }) {
        bool buttonHovered = Clay_Hovered();
        Clay_OnHover(HandleButtonInteraction, &TimerButtonPressed);

        CLAY_TEXT(timerIsRunning ? CLAY_STRING("Stop") : CLAY_STRING("Start"),
                  CLAY_TEXT_CONFIG({.fontId = FONT_ID_BODY_16,
                                    .fontSize = 24,
                                    .textColor = {255, 255, 255, 255}}));

        if (TimerButtonPressed) {
          if (!initializeTimer) {
            startTimer(baseWorkTime, &timerIsRunning);
          } else {
            stopTimer(&timerIsRunning);
          }
          TimerButtonPressed = false;
        }
      }
    }
  }
  return Clay_EndLayout();
}
