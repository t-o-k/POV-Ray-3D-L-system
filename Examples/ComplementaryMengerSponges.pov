// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10
/*

https://github.com/t-o-k/POV-Ray-3D-L-system

Copyright (c) 2026 Tor Olav Kristensen, http://subcube.com

Use of this source code is governed by the GNU Lesser General Public License version 3,
which can be found in the LICENSE file.

*/
// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

#version 3.7;

#include "../L-system_3D.inc"

global_settings { assumed_gamma 1.0 }

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

// Complementary 3D Menger Sponges (Spheres)
//
// While both parts could be generated together using a single L-system with material-switching 
// symbols, this example demonstrates modular design by splitting the problem into two simpler, 
// independent L-systems:
//
//   1. Inverted Menger sponge (red): The 7 inner sub-spheres removed per step.
//   2. Standard Menger sponge (white): The 20 outer sub-spheres kept per step.
//
// Note: Sequences like "+f-", "^f&", and "|f|" translate the turtle sideways, vertically, or 
// backward while immediately restoring its original orientation (attitude). 
// While orientation does not affect symmetric spheres here, maintaining a consistent attitude 
// across recursion steps is a useful habit when placing asymmetric shapes or building complex 
// 3D grids.
//
// Both structures share the same 20-point recursive framework and are rendered both separately 
// and combined in the center.

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

#declare RadiusScale = 3.0; 
#declare RadiusIncrement = 0.0;
InsertRadiusFunctions(Functions, RadiusScale, RadiusIncrement)

#declare LengthScale = 3.0; 
#declare LengthIncrement = 0.0;
InsertLengthFunctions(Functions, LengthScale, LengthIncrement)

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

#declare Iterations_Menger = 3;

// The deepest recursive branch in the rule nests 4 bracket levels ([...]),
// so each iteration adds 4 levels to the maximum required stack depth.
#declare StackSize = Iterations_Menger*4;

#declare pStart = <0, 0, 0>;
#declare vF_Initial = +x;
#declare vR_Initial = +y;
#declare vU_Initial = +z;
#declare Angle_Initial = +90.0;
#declare Length_Initial = 1.0;
#declare Radius_Initial = 0.5;


// Constants
#declare Rules[asc("f")] = "f"
#declare Rules[asc("F")] = "F"

InsertNoChangeFunctions(Functions, "G ")

#declare Rules[asc("G")] = "[ <! @[f@][|f|@][+f-@][-f+@][^f&@][&f^@] [+f-[^f&G][&f^G]] [-f+[^f&G][&f^G]] [ f [+f-G[^f&G][&f^G]] [-f+G[^f&G][&f^G]] [^f&G][&f^G] ] [ |f| [+f-G[^f&G][&f^G]] [-f+G[^f&G][&f^G]] [^f&G][&f^G] ] ]"

#declare Axiom = "G"

#declare MengerSponge_Inverted =
    union {
        L_DrawAll(
            Functions,
            L_Transform(Axiom, Rules, Iterations_Menger),
            StackSize,
            pStart,
            vF_Initial,
            vR_Initial,
            vU_Initial,
            Angle_Initial,
            Length_Initial,
            Radius_Initial,
            NoMaterials
        )
        pigment { color <1.05, 0.05, 0.00> }
    }


#declare Rules[asc("G")] = "[ <! [+f-[^f&G][&f^G]] [-f+[^f&G][&f^G]] [ f [+f-G[^f&G][&f^G]] [-f+G[^f&G][&f^G]] [^f&G][&f^G] ] [ |f| [+f-G[^f&G][&f^G]] [-f+G[^f&G][&f^G]] [^f&G][&f^G] ] ]"

#declare Axiom = "G"

#declare Axiom = L_Transform(Axiom, Rules, Iterations_Menger)

#declare Rules[asc("G")] = "@"

#declare Iterations_Extra = 1;

#declare MengerSponge =
    union {
        L_DrawAll(
            Functions,
            L_Transform(Axiom, Rules, Iterations_Extra),
            StackSize,
            pStart,
            vF_Initial,
            vR_Initial,
            vU_Initial,
            Angle_Initial,
            Length_Initial,
            Radius_Initial,
            NoMaterials
        )
        pigment { color rgb <1.0, 1.0, 1.0> }
    }

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

object {
    MengerSponge_Inverted
    rotate 25*y
    translate <-1.9,  0.0, -1.5>
}

union {
    object { MengerSponge_Inverted }
    object { MengerSponge }
    rotate 25*y
}

object {
    MengerSponge
    rotate 25*y
    translate <+1.9,  0.0, +1.5>
}

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10

background { color rgb <0.00, 0.04, 0.06> } 

camera {
    location < 0.0, +7.0, -14.0>
    look_at < 0.0, -0.1,  0.0>
    angle 25
}

light_source {
    100*<7, 3, -5>
    color rgb <1.5, 1.5, 1.5>
}

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ======= 10
