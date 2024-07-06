// cavacore standalone test app, build cava first and compile with:
// gcc -c -g cavacore_test.c
// gcc -o cavacore_test cavacore_test.o cava-cavacore.o -lm -lfftw3

#include "cavacore.h"
#include <math.h>
#include <stdio.h>

#include <stdlib.h>
#define PI 3.141592654

void main() {

    printf("welcome to cavalib standalone test app\n");

    int bars_per_channel = 10;
    int channels = 2;
    int buffer_size = 512 * channels; // number of samples per cava execute
    int rate = 44100;
    int blueprint_2000MHz[10] = {0, 0, 0, 0, 0, 0, 78, 20, 0, 0};
    int blueprint_200MHz[10] = {0, 0, 98, 4, 0, 0, 0, 0, 0, 0};

    printf("planning cava 20 bars (left+right) 44100 rate 2 cahnnels, 100 target height, 86 "
           "framerate \n");

    struct cava_plan *plan = cava_init(bars_per_channel, rate, channels, 1, 0.77, 50, 10000);
    printf("got lower cut off frequencies:\n");

    for (int i = 0; i < 10; i++) {
        printf("%.0f \t", plan->cut_off_frequency[i]);
    }
    printf("MHz\n\n");

    printf("allocating buffers and generating sine wave for test\n\n");

    double *cava_out;
    double *cava_in;

    cava_out = (double *)malloc(bars_per_channel * channels * sizeof(double));

    cava_in = (double *)malloc(buffer_size * sizeof(double));

    for (int i = 0; i < bars_per_channel * channels; i++) {
        cava_out[i] = 0;
    }

    printf("running cava execute 300 times (simulating about 3.5 seconds run time)\n\n");
    for (int k = 0; k < 300; k++) {
        // filling up 512*2 samples at a time, making sure the sinus wave is unbroken
        for (int n = 0; n < buffer_size / 2; n++) {
            cava_in[n * 2] = sin(2 * PI * 200 / rate * (n + (k * buffer_size / 2))) * 10000;
            cava_in[n * 2 + 1] = sin(2 * PI * 2000 / rate * (n + (k * buffer_size / 2))) * 10000;
        }
        cava_execute(cava_in, buffer_size, cava_out, plan);
    }

    int bp_ok = 1;
    printf("\noutput left, max value should be at 2000Hz:\n");
    for (int i = 0; i < bars_per_channel; i++) {
        printf("%d \t", (int)(cava_out[i] * 100));

        // checking if result matches blueprint
        if ((int)(cava_out[i] * 100) != blueprint_2000MHz[i])
            bp_ok = 0;
    }
    printf("MHz\n");

    printf("output right,  max value should be at 200Hz:\n");
    for (int i = 0; i < bars_per_channel; i++) {
        printf("%d \t", (int)(cava_out[i + bars_per_channel] * 100));

        // checking if result matches blueprint
        if ((int)(cava_out[i + bars_per_channel] * 100) != blueprint_200MHz[i])
            bp_ok = 0;
    }
    printf("MHz\n\n");
    cava_destroy(plan);
    free(plan);
    free(cava_in);
    free(cava_out);
    if (bp_ok == 1) {
        printf("matching blueprint\n");
        exit(0);
    } else {
        printf("not matching blueprint\n");
        exit(1);
    }
}