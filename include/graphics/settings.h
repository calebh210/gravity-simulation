#ifndef SETTINGS_H
#define SETTINGS_H

// Unsure if I want to keep these here, put them somewhere else, or move more stuff into here
// My organization has been kinda bad so far...


// Defines the integration interval (DT)
// 0.1 = Super High Precision (never use this)
// 1.0 = High Precision
// 5.0 = Medium Precision
// 10.0 = Fast 
// 100.0 = Very Fast
// 1000.0 = Super Fast
enum SimulatonMode {
    SUPER_HIGH_PRECISION,
    HIGH_PRECISION,
    MEDIUM_PRECISION,
    FAST,
    VERY_FAST,
    SUPER_FAST
}


#endif