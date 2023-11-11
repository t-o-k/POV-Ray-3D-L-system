// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10
/*

https://github.com/t-o-k/POV-Ray-3D-L-system

Copyright (c) 2023 Tor Olav Kristensen, http://subcube.com

Use of this source code is governed by the GNU Lesser General Public License version 3,
which can be found in the LICENSE file.

*/
// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

#version 3.7;

#include "../L-system_3D.inc"

global_settings { assumed_gamma 1.0 }

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

// Constants
#declare Rules[asc("F")] = "F";
#declare Rules[asc("f")] = "f";

// Variables
#declare Rules[asc("A")] = "B-F+CFC+F-D&F^D-F+&&CFC+F+B//";
#declare Rules[asc("B")] = "A&F^CFB^F^D^^-F-D^|F^B|FC^F^A//";
#declare Rules[asc("C")] = "|D^|F^B-F+C^F^A&&FA&F^C+F+B^F^D//";
#declare Rules[asc("D")] = "|CFB-F+B|FA&F^A&&FB-F+B|FC//";

#declare Axiom = "A";

InsertNoChangeFunctions(Functions, "ABCD")

#declare Iterations = 2;
#declare L_string = L_Transform(Axiom, Rules, Iterations);

#declare StackSize = 0*Iterations;
#declare pStart = <-1.5, +1.5, +1.5>;

#declare InitialAngle = +90; // degrees
#declare InitialLength = 1.0;
#declare InitialRadius = 0.11;

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

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
        true,
        true
    )
    pigment { color rgb <0.1, 0.1, 0.2> }
}

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

light_source {
    100*<-3, -2, -4>
    color rgb <1, 1, 1>*2
    shadowless
}

background { color rgb <1.0, 1.0, 1.0> }

camera {
    location <+4.8, +4.8, -8.4>
    look_at < 0.0, -0.2,  0.0>
    angle 40
}

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

