#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int reimann_sum(){
    return 0;
}

// normalize values to something that opengl can render
float normalize(double value, double min, double max) {
    return (float)(2.0 * (value - min) / (max - min) - 1.0);
}