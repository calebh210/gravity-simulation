# gravity-simulation

Using the equations from [here](https://orbital-mechanics.space) as a guide.

## To Do
- Add sim for the CR3B Problem
- Add checking if init values make sense for "relative" frame
- Add N-Body Support
- Add 3D Support
- Create embedding diagram with Flamm's Parabloid
- Black Holes? Raytracing?


## Guide

| Flag | Function                                      | Arguments               |
|------|-----------------------------------------------|-------------------------|
| -m   | Set the frame of reference for the simulation | inertial, relative, cog |
| -t   | Set the time delta between steps              | Floats > 0.0            |
| -d   | Enable debug printing to console              | N/A                     |
| -h   | Open the help menu                            | N/A                     |

