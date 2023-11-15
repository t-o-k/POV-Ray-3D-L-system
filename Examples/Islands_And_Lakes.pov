// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10
/*

https://github.com/t-o-k/POV-Ray-L-system

2D version:
    "Combination of islands and lakes" from Przemyslaw Prusinkiewicz & Aristid Lindenmayer's
    "The Algorithmic Beauty of Plants", Figure 1.8 on page 9

3D version:
    From Laurens Lapré's "Lparser", BOP05.LS. Modified by "Droj" and Tor Olav Kristensen

*/
// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

#version 3.7;

#include "../L-system_3D.inc"
#include "colors.inc"

global_settings { assumed_gamma 1.0 }


// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

// Variables
#declare Rules[asc("X")] = ":^F+F+F+F+&f";
#declare Rules[asc("F")] = "F+f-FF+F+FF+Ff+FF-f+FF-F-FF-Ff-FFF";
#declare Rules[asc("f")] = "ffffff;";

InsertNoChangeFunctions(Functions, "X")

#declare Axiom = "X+X+X+X";

#declare Iterations = 3;  // Produces 10369 objects
#declare L_string = L_Transform(Axiom, Rules, Iterations);

#declare StackSize = 0*Iterations;
#declare pStart = <0, 0, 0>;

#declare InitialAngle = +90.0;  // degrees
#declare InitialLength = 2.0;
#declare InitialRadius = 0.4;

#declare C = 7/8;

#declare FourMaterials =
    array[4] {
        material {
            texture {
                pigment { color (1 - C)*White + C*Red }
            }
        },
        material {
            texture {
                pigment { color (1 - C)*White + C*Green }
            }
        },
        material {
            texture {
                pigment { color (1 - C)*White + C*Blue }
            }
        },
        material {
            texture {
                pigment { color (1 - C)*White + C*White }
            }
        }
    }

union {
    L_Draw(
        Functions,
        L_string,
        StackSize,
        pStart,
        x, y, z,
        InitialAngle,
        InitialLength,
        InitialRadius,
        FourMaterials,
        true,
        true
    )
    rotate 45*y
    translate 10*<-4, -3, +5>
}

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

background { color White/40 }

light_source {
    5*<-3, +20, -20>
    color White
    shadowless
}

camera {
    perspective
    location 27*<-1, 0, -5>
    look_at <0, 0, 0>
}

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10
