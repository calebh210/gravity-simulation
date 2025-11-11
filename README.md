# gravity-simulation

Using the equations from [here](https://orbital-mechanics.space) as a guide.

## To Do
- Add checking if init values make sense for "relative" frame
- Add 3D Support
- Create embedding diagram with Flamm's Parabloid
- Black Holes? Raytracing?
- Make the orbit trails configurable


## Guide

| Flag | Function                                      | Arguments               |
|------|-----------------------------------------------|-------------------------|
| -m   | Set the frame of reference for the simulation | inertial, relative, cog,  cr3bp, n-body|                                                             
| -t   | Set the time delta between steps              | Floats > 0.0            |
| -d   | Enable debug printing to console              | N/A                     |
| -h   | Open the help menu                            | N/A                     |
| -n   | Define the number of bodies to render         | 0 - N                   |

