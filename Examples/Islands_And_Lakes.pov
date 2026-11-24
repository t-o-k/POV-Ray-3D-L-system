// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10
/*

https://github.com/t-o-k/POV-Ray-3D-L-system

2D version:
    "Combination of islands and lakes" from Przemyslaw Prusinkiewicz & Aristid Lindenmayer's
    "The Algorithmic Beauty of Plants", Figure 1.8 on page 9

3D version:
    From Laurens Lapre's "Lparser", BOP05.LS. Modified by "Droj" and Tor Olav Kristensen

*/
// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

#version 3.7;

#include "../L-system_3D.inc"
#include "colors.inc"

global_settings { assumed_gamma 1.0 }

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

// Variables
AddRule("X", ":^F+F+F+F+&f")
AddRule("F", "F+f-FF+F+FF+Ff+FF-f+FF-F-FF-Ff-FFF")
AddRule("f", "ffffff;")

#declare Axiom = "X+X+X+X";

#declare Iterations = 3;  // Produces 10369 objects
#declare L_string = L_Transform(Axiom, Iterations);

#declare StackSize = 0*Iterations;
#declare pStart = <0, 0, 0>;

#declare InitialAngle = +90.0;  // degrees
#declare InitialLength = 2.0;
#declare InitialRadius = 0.4;

#declare C = 7/8;

#declare NoOfMaterials = 4;

#declare Materials =
    array[NoOfMaterials] {
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

InsertMaterialsFunctions(NoOfMaterials)

union {
    L_Draw(
        L_string,
        StackSize,
        pStart,
        x, y, z,
        InitialAngle,
        InitialLength,
        InitialRadius,
        Materials,
        true,
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
