#ifdef SIMULATE

#include "input.h"
#include "../src/frames.h"

#include </usr/include/stdio.h>
#include </usr/include/stdlib.h>
#include </usr/include/unistd.h>
#include </usr/include/sys/time.h>
#include <pthread.h>
#include <termios.h>

uint8_t SCAN_PORT = 0xff;
const uint8_t INPUT_PORT = 0xff;

int keypress = 0;
int running = 1;
pthread_t poll_thread;
static struct termios oldt;

void *poll_key(void *arg) {
    int *running = (int *)arg;
    while (*running) {
        keypress = getchar();
        if (keypress == 'q') {
            end_input();
            exit(0);
        }
    }
    return (void *)0;
}

void setup_input() {
    static struct termios newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    pthread_create(&poll_thread, NULL, poll_key, (void *)&running);
}

void end_input() {
    running = 0;
    pthread_join(poll_thread, NULL);
    /*restore the old settings*/
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}

void simulate_input(const Frame **next_emote) {
    if (keypress >= 0x30 && keypress <= 0x39) {
        int index = keypress - 0x30;
        *(next_emote) = frame_lut[index];
    }
}

#endif