#ifndef DOOM_GENERIC
#define DOOM_GENERIC

#include <stdlib.h>
#include <stdint.h>

extern uint32_t DOOMGENERIC_RESX;
extern uint32_t DOOMGENERIC_RESY;

#ifdef CMAP256

typedef uint8_t pixel_t;

#else  // CMAP256

typedef uint32_t pixel_t;

#endif  // CMAP256


extern pixel_t* DG_ScreenBuffer;

void doomgeneric_Create(int argc, char **argv);
void doomgeneric_Tick();

typedef union {
    uint8_t is_key_event : 1;

    struct {
        uint8_t reserved : 1;

        uint8_t pressed : 1;
        unsigned char key;
    } key_event;
    struct {
        uint8_t reserved : 1;

        uint8_t left_mouse_button : 1;

        int16_t delta_x;
        int16_t delta_y;
    } mouse_event;
} DG_Event;


//Implement below functions for your platform
void DG_Init();
void DG_DrawFrame();
void DG_SleepMs(uint32_t ms);
uint32_t DG_GetTicksMs();
int DG_GetEvent(DG_Event* evt);
void DG_SetWindowTitle(const char * title);

#endif //DOOM_GENERIC
