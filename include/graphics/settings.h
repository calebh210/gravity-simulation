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
typedef enum {
    SUPER_HIGH_PRECISION,
    HIGH_PRECISION,
    MEDIUM_PRECISION,
    FAST,
    VERY_FAST,
    SUPER_FAST
} SimulationMode;


float get_dt_from_settings(SimulationMode mode){

    switch(mode){
        case SUPER_HIGH_PRECISION: return 0.1f;
        case HIGH_PRECISION: return 1.0f;
        case MEDIUM_PRECISION: return 5.0f;
        case FAST: return 10.0f;
        case VERY_FAST: return 100.0f;
        case SUPER_FAST: return 1000.0f;
    }
}

#endif