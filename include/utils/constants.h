#ifndef CONSTANTS_H
#define CONSTANTS_H

// Newton's gravitational constant
extern const double G;

//One Astronomical Unit
extern const long AU;

// number of frames to run for. -1 will be infinite
#define RUN_LIMIT -1

#define PI 3.14159265358979323846f
#define TWO_PI 6.28318530718f
#define DEG_TO_RAD (PI / 180.0f)
#define RAD_TO_DEG (180.0f / PI)

// speed of light in m/s
extern const long c;

// Celestial Masses
extern const double mass_earth;
extern const double mass_sun;
extern const double mass_sagA;

//used for normalization
extern const double SPACE_MIN;
extern const double SPACE_MAX;

#endif
