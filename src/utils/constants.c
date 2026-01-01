#include "utils/constants.h"
// Newton's gravitational constant
const double G = (6.6742E-11);

//One Astronomical Unit
const long AU = 149597870700;

// speed of light in m/s
const long c = 299792458;

// Celestial Masses
const double mass_earth = 5.97219E24;
const double mass_sun = 1.9891E30;
const double mass_sagA = 8.5E36;

//used for normalization
// should be configurable
// const double SPACE_MIN = -1.5 * AU * 2;
// const double SPACE_MAX = 1.5 * AU * 2; 
const double SPACE_MIN = -149597870700 * 2;
const double SPACE_MAX = 149597870700 * 2;