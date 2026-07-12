#include <stdio.h>

void print_help_menu() {
    printf("Help Menu\n\n"
           "CLI Flags\n\n"
           "| Flag | Function                                      | Arguments           |\n"
           "|------|-----------------------------------------------|-------------------------|\n"
           "| -m   | Set the frame of reference for the simulation | inertial, relative, cog, n-body|\n"
           "| -t   | Set the time delta between steps              | Floats > 0.0            |\n"
           "| -d   | Enable debug printing to console              | N/A                     |\n"
           "| -h   | Open the help menu                            | N/A                     |\n"
           "| -n   | Define the number of bodies to render         | 0 - N (N < 64)          |\n"
           "| -3   | Render the scene in 3d                        | N/A                     |\n"
           "| -f   | Set the scene config file                     | string (optional)       |\n\n"
           "Examples:\n"
           "\"./sim -m cog -n 2 -t 10 -3 -f misc/configs/two-suns.yaml\" -> Run a simulation in the CoG frame with 2 bodies with a timestep of 10 in 3d\n"
           "\"./sim -m n-body -n 6 -t 100\" -> Run a simulation in the inertial n-body frame frame with 6 bodies with a timestep of 100 in 2d\n\n"

           "If you do not include your own config file, the default is \"init.yaml\" in you current directory. Refer to \"init.example.yaml\"\n");
}
