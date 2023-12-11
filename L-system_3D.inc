// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12
/*

https://github.com/t-o-k/POV-Ray-L-system_3D

Copyright (c) 2023 Tor Olav Kristensen, http://subcube.com

Use of this source code is governed by the GNU Lesser General Public License version 3,
which can be found in the LICENSE file.

*/
// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12

#version 3.7;

#include "functions.inc"

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12

#macro DebugString(IndentStr, Str)

    #local C = 120 - strlen(IndentStr);
    #local L = strlen(Str);
    #local D = div(L, C);
    #local M = mod(L, C);
    #for (I, 0, D-1)
        #debug IndentStr
        #debug substr(Str, 1 + I*C, C)
        #debug "\n"
    #end // for
    #local LastStr = substr(Str, 1 + D*C, M)
    #if (strlen(LastStr) > 0)
        #debug IndentStr
        #debug LastStr
        #debug "\n"
    #end // if

#end // macro DebugString


#macro _TmpAxisRotateFn0(Fn)

    function(x, y, z, ax, ay, az, w) {
        Fn(x, y, z, ax, ay, az, f_r(ax, ay, az), cos(w), sin(w))
    }

#end // macro _TmpAxisRotateFn0


#macro _TmpAxisRotateFn1(Fn)

    _TmpAxisRotateFn0(
        function(x, y, z, ax, ay, az, la, c, s) {
            Fn(x, y, z, ax/la, ay/la, az/la, 1 - c, c, s)
        }
    )

#end // macro _TmpAxisRotateFn1


#declare AxisRotateFnX = // function(x, y, z, ax, ay, az, w) { }
    _TmpAxisRotateFn1(
        function(x, y, z, axn ayn, azn, omc, c, s) {
            (axn*axn*omc +     c)*x +
            (axn*ayn*omc - azn*s)*y +
            (axn*azn*omc + ayn*s)*z
        }
    )
;
#declare AxisRotateFnY = // function(x, y, z, ax, ay, az, w) { }
    _TmpAxisRotateFn1(
        function(x, y, z, axn ayn, azn, omc, c, s) {
            (ayn*axn*omc + azn*s)*x +
            (ayn*ayn*omc +     c)*y +
            (ayn*azn*omc - axn*s)*z
        }
    )
;
#declare AxisRotateFnZ = // function(x, y, z, ax, ay, az, w) { }
    _TmpAxisRotateFn1(
        function(x, y, z, axn ayn, azn, omc, c, s) {
            (azn*axn*omc - ayn*s)*x +                       
            (azn*ayn*omc + axn*s)*y +
            (azn*azn*omc +     c)*z
        }
    )
;

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12

#declare MaxNoOfSymbols = 128; // No of characters in the ASCII table

#declare Rules = array[MaxNoOfSymbols];
#for (I, 0, MaxNoOfSymbols - 1)
    #declare Rules[I] = "";
#end // for

// Constants
#declare Rules[asc("[")] = "[";
#declare Rules[asc("]")] = "]";
#declare Rules[asc("|")] = "|";
#declare Rules[asc("\\")] = "\\";
#declare Rules[asc("/")] = "/";
#declare Rules[asc("&")] = "&";
#declare Rules[asc("^")] = "^";
#declare Rules[asc("+")] = "+";
#declare Rules[asc("-")] = "-";
#declare Rules[asc("~")] = "~";
#declare Rules[asc("#")] = "#";
#declare Rules[asc("!")] = "!";
#declare Rules[asc("<")] = "<";
#declare Rules[asc(">")] = ">";
#declare Rules[asc("(")] = "(";
#declare Rules[asc(")")] = ")";
#declare Rules[asc(":")] = ":";
#declare Rules[asc(";")] = ";";

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12

#declare I_PP_X =  0; // Position coordinate x
#declare I_PP_Y =  1; // Position coordinate y
#declare I_PP_Z =  2; // Position coordinate Z
#declare I_VF_X =  3; // Forward vector coordinate x
#declare I_VF_Y =  4; // Forward vector coordinate y
#declare I_VF_Z =  5; // Forward vector coordinate z
#declare I_VR_X =  6; // Right vector coordinate x
#declare I_VR_Y =  7; // Right vector coordinate y
#declare I_VR_Z =  8; // Right vector coordinate z
#declare I_VU_X =  9; // Up vector coordinate x
#declare I_VU_Y = 10; // Up vector coordinate y
#declare I_VU_Z = 11; // Up vector coordinate z
#declare I_AA_F = 12; // Turn angle
#declare I_AA_R = 13; // Turn angle
#declare I_AA_U = 14; // Turn angle
#declare I_LLLL = 15; // Length
#declare I_RRRR = 16; // Radius
#declare I_SWAP = 17; // Swap directions of vectors

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12

#declare NoChangeFunctions = array[18];
#declare NoChangeFunctions[I_PP_X] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        x
    }
;
#declare NoChangeFunctions[I_PP_Y] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        y
    }
;
#declare NoChangeFunctions[I_PP_Z] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        z
    }
;
#declare NoChangeFunctions[I_VF_X] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        fx
    }
;
#declare NoChangeFunctions[I_VF_Y] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        fy
    }
;
#declare NoChangeFunctions[I_VF_Z] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        fz
    }
;
#declare NoChangeFunctions[I_VR_X] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        rx
    }
;
#declare NoChangeFunctions[I_VR_Y] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        ry
    }
;
#declare NoChangeFunctions[I_VR_Z] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        rz
    }
;
#declare NoChangeFunctions[I_VU_X] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        ux
    }
;
#declare NoChangeFunctions[I_VU_Y] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        uy
    }
;
#declare NoChangeFunctions[I_VU_Z] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        uz
    }
;
#declare NoChangeFunctions[I_AA_F] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        af
    }
;
#declare NoChangeFunctions[I_AA_R] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        ar
    }
;
#declare NoChangeFunctions[I_AA_U] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        au
    }
;
#declare NoChangeFunctions[I_LLLL] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        l
    }
;
#declare NoChangeFunctions[I_RRRR] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        r
    }
;
#declare NoChangeFunctions[I_SWAP] =
    function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
        s
    }
;

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12

#macro InsertBasicFunctions(Fns)

    #local S = asc("F"); // Move forward while "drawing"
    #declare Fns[S][I_PP_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            x + l*fx/f_r(fx, fy, fz)
        }
    ;
    #declare Fns[S][I_PP_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            y + l*fy/f_r(fx, fy, fz)
        }
    ;
    #declare Fns[S][I_PP_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            z + l*fz/f_r(fx, fy, fz)
        }
    ;
    #declare Fns[S][I_VF_X] = NoChangeFunctions[I_VF_X];
    #declare Fns[S][I_VF_Y] = NoChangeFunctions[I_VF_Y];
    #declare Fns[S][I_VF_Z] = NoChangeFunctions[I_VF_Z];
    #declare Fns[S][I_VR_X] = NoChangeFunctions[I_VR_X];
    #declare Fns[S][I_VR_Y] = NoChangeFunctions[I_VR_Y];
    #declare Fns[S][I_VR_Z] = NoChangeFunctions[I_VR_Z];
    #declare Fns[S][I_VU_X] = NoChangeFunctions[I_VU_X];
    #declare Fns[S][I_VU_Y] = NoChangeFunctions[I_VU_Y];
    #declare Fns[S][I_VU_Z] = NoChangeFunctions[I_VU_Z];
    #declare Fns[S][I_AA_F] = NoChangeFunctions[I_AA_F];
    #declare Fns[S][I_AA_R] = NoChangeFunctions[I_AA_R];
    #declare Fns[S][I_AA_U] = NoChangeFunctions[I_AA_U];
    #declare Fns[S][I_LLLL] = NoChangeFunctions[I_LLLL];
    #declare Fns[S][I_RRRR] = NoChangeFunctions[I_RRRR];
    #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];
    #local S = asc("f"); // Move forward while not "drawing"
    #declare Fns[S][I_PP_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            x + l*fx/f_r(fx, fy, fz)
        }
    ;
    #declare Fns[S][I_PP_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            y + l*fy/f_r(fx, fy, fz)
        }
    ;
    #declare Fns[S][I_PP_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            z + l*fz/f_r(fx, fy, fz)
        }
    ;
    #declare Fns[S][I_VF_X] = NoChangeFunctions[I_VF_X];
    #declare Fns[S][I_VF_Y] = NoChangeFunctions[I_VF_Y];
    #declare Fns[S][I_VF_Z] = NoChangeFunctions[I_VF_Z];
    #declare Fns[S][I_VR_X] = NoChangeFunctions[I_VR_X];
    #declare Fns[S][I_VR_Y] = NoChangeFunctions[I_VR_Y];
    #declare Fns[S][I_VR_Z] = NoChangeFunctions[I_VR_Z];
    #declare Fns[S][I_VU_X] = NoChangeFunctions[I_VU_X];
    #declare Fns[S][I_VU_Y] = NoChangeFunctions[I_VU_Y];
    #declare Fns[S][I_VU_Z] = NoChangeFunctions[I_VU_Z];
    #declare Fns[S][I_AA_F] = NoChangeFunctions[I_AA_F];
    #declare Fns[S][I_AA_R] = NoChangeFunctions[I_AA_R];
    #declare Fns[S][I_AA_U] = NoChangeFunctions[I_AA_U];
    #declare Fns[S][I_LLLL] = NoChangeFunctions[I_LLLL];
    #declare Fns[S][I_RRRR] = NoChangeFunctions[I_RRRR];
    #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];
    #local S = asc("|"); // Turn around (Yaw +pi radians)
    #declare Fns[S][I_PP_X] = NoChangeFunctions[I_PP_X];
    #declare Fns[S][I_PP_Y] = NoChangeFunctions[I_PP_Y];
    #declare Fns[S][I_PP_Z] = NoChangeFunctions[I_PP_Z];
    #declare Fns[S][I_VF_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            -fx
        }
    ;
    #declare Fns[S][I_VF_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            -fy
        }
    ;
    #declare Fns[S][I_VF_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            -fz
        }
    ;
    #declare Fns[S][I_VR_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            -rx
        }
    ;
    #declare Fns[S][I_VR_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            -ry
        }
    ;
    #declare Fns[S][I_VR_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            -rz
        }
    ;
    #declare Fns[S][I_VU_X] = NoChangeFunctions[I_VU_X];
    #declare Fns[S][I_VU_Y] = NoChangeFunctions[I_VU_Y];
    #declare Fns[S][I_VU_Z] = NoChangeFunctions[I_VU_Z];
    #declare Fns[S][I_AA_F] = NoChangeFunctions[I_AA_F];
    #declare Fns[S][I_AA_R] = NoChangeFunctions[I_AA_R];
    #declare Fns[S][I_AA_U] = NoChangeFunctions[I_AA_U];
    #declare Fns[S][I_LLLL] = NoChangeFunctions[I_LLLL];
    #declare Fns[S][I_RRRR] = NoChangeFunctions[I_RRRR];
    #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];
    #local S = asc("\\"); // Roll left
    #declare Fns[S][I_PP_X] = NoChangeFunctions[I_PP_X];
    #declare Fns[S][I_PP_Y] = NoChangeFunctions[I_PP_Y];
    #declare Fns[S][I_PP_Z] = NoChangeFunctions[I_PP_Z];
    #declare Fns[S][I_VF_X] = NoChangeFunctions[I_VF_X];
    #declare Fns[S][I_VF_Y] = NoChangeFunctions[I_VF_Y];
    #declare Fns[S][I_VF_Z] = NoChangeFunctions[I_VF_Z];
    #declare Fns[S][I_VR_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnX(rx, ry, rz, fx, fy, fz, select(s, -af, +af, -af))
        }
    ;
    #declare Fns[S][I_VR_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnY(rx, ry, rz, fx, fy, fz, select(s, -af, +af, -af))
        }
    ;
    #declare Fns[S][I_VR_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnZ(rx, ry, rz, fx, fy, fz, select(s, -af, +af, -af))
        }
    ;
    #declare Fns[S][I_VU_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnX(ux, uy, uz, fx, fy, fz, select(s, -af, +af, -af))
        }
    ;
    #declare Fns[S][I_VU_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnY(ux, uy, uz, fx, fy, fz, select(s, -af, +af, -af))
        }
    ;
    #declare Fns[S][I_VU_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnZ(ux, uy, uz, fx, fy, fz, select(s, -af, +af, -af))
        }
    ;
    #declare Fns[S][I_AA_F] = NoChangeFunctions[I_AA_F];
    #declare Fns[S][I_AA_R] = NoChangeFunctions[I_AA_R];
    #declare Fns[S][I_AA_U] = NoChangeFunctions[I_AA_U];
    #declare Fns[S][I_LLLL] = NoChangeFunctions[I_LLLL];
    #declare Fns[S][I_RRRR] = NoChangeFunctions[I_RRRR];
    #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];
    #local S = asc("/"); // Roll right 
    #declare Fns[S][I_PP_X] = NoChangeFunctions[I_PP_X];
    #declare Fns[S][I_PP_Y] = NoChangeFunctions[I_PP_Y];
    #declare Fns[S][I_PP_Z] = NoChangeFunctions[I_PP_Z];
    #declare Fns[S][I_VF_X] = NoChangeFunctions[I_VF_X];
    #declare Fns[S][I_VF_Y] = NoChangeFunctions[I_VF_Y];
    #declare Fns[S][I_VF_Z] = NoChangeFunctions[I_VF_Z];
    #declare Fns[S][I_VR_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnX(rx, ry, rz, fx, fy, fz, select(s, +af, -af, +af))
        }
    ;
    #declare Fns[S][I_VR_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnY(rx, ry, rz, fx, fy, fz, select(s, +af, -af, +af))
        }
    ;
    #declare Fns[S][I_VR_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnZ(rx, ry, rz, fx, fy, fz, select(s, +af, -af, +af))
        }
    ;
    #declare Fns[S][I_VU_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnX(ux, uy, uz, fx, fy, fz, select(s, +af, -af, +af))
        }
    ;
    #declare Fns[S][I_VU_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnY(ux, uy, uz, fx, fy, fz, select(s, +af, -af, +af))
        }
    ;
    #declare Fns[S][I_VU_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnZ(ux, uy, uz, fx, fy, fz, select(s, +af, -af, +af))
        }
    ;
    #declare Fns[S][I_AA_F] = NoChangeFunctions[I_AA_F];
    #declare Fns[S][I_AA_R] = NoChangeFunctions[I_AA_R];
    #declare Fns[S][I_AA_U] = NoChangeFunctions[I_AA_U];
    #declare Fns[S][I_LLLL] = NoChangeFunctions[I_LLLL];
    #declare Fns[S][I_RRRR] = NoChangeFunctions[I_RRRR];
    #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];
    #local S = asc("&"); // Pitch down
    #declare Fns[S][I_PP_X] = NoChangeFunctions[I_PP_X];
    #declare Fns[S][I_PP_Y] = NoChangeFunctions[I_PP_Y];
    #declare Fns[S][I_PP_Z] = NoChangeFunctions[I_PP_Z];
    #declare Fns[S][I_VF_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnX(fx, fy, fz, rx, ry, rz, select(s, -ar, +ar, -ar))
        }
    ;
    #declare Fns[S][I_VF_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnY(fx, fy, fz, rx, ry, rz, select(s, -ar, +ar, -ar))
        }
    ;
    #declare Fns[S][I_VF_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnZ(fx, fy, fz, rx, ry, rz, select(s, -ar, +ar, -ar))
        }
    ;
    #declare Fns[S][I_VR_X] = NoChangeFunctions[I_VR_X];
    #declare Fns[S][I_VR_Y] = NoChangeFunctions[I_VR_Y];
    #declare Fns[S][I_VR_Z] = NoChangeFunctions[I_VR_Z];
    #declare Fns[S][I_VU_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnX(ux, uy, uz, rx, ry, rz, select(s, -ar, +ar, -ar))
        }
    ;
    #declare Fns[S][I_VU_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnY(ux, uy, uz, rx, ry, rz, select(s, -ar, +ar, -ar))
        }
    ;
    #declare Fns[S][I_VU_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnZ(ux, uy, uz, rx, ry, rz, select(s, -ar, +ar, -ar))
        }
    ;
    #declare Fns[S][I_AA_F] = NoChangeFunctions[I_AA_F];
    #declare Fns[S][I_AA_R] = NoChangeFunctions[I_AA_R];
    #declare Fns[S][I_AA_U] = NoChangeFunctions[I_AA_U];
    #declare Fns[S][I_LLLL] = NoChangeFunctions[I_LLLL];
    #declare Fns[S][I_RRRR] = NoChangeFunctions[I_RRRR];
    #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];
    #local S = asc("^"); // Pitch up
    #declare Fns[S][I_PP_X] = NoChangeFunctions[I_PP_X];
    #declare Fns[S][I_PP_Y] = NoChangeFunctions[I_PP_Y];
    #declare Fns[S][I_PP_Z] = NoChangeFunctions[I_PP_Z];
    #declare Fns[S][I_VF_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnX(fx, fy, fz, rx, ry, rz, select(s, +ar, -ar, +ar))
        }
    ;
    #declare Fns[S][I_VF_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnY(fx, fy, fz, rx, ry, rz, select(s, +ar, -ar, +ar))
        }
    ;
    #declare Fns[S][I_VF_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnZ(fx, fy, fz, rx, ry, rz, select(s, +ar, -ar, +ar))
        }
    ;
    #declare Fns[S][I_VR_X] = NoChangeFunctions[I_VR_X];
    #declare Fns[S][I_VR_Y] = NoChangeFunctions[I_VR_Y];
    #declare Fns[S][I_VR_Z] = NoChangeFunctions[I_VR_Z];
    #declare Fns[S][I_VU_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnX(ux, uy, uz, rx, ry, rz, select(s, +ar, -ar, +ar))
        }
    ;
    #declare Fns[S][I_VU_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnY(ux, uy, uz, rx, ry, rz, select(s, +ar, -ar, +ar))
        }
    ;
    #declare Fns[S][I_VU_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnZ(ux, uy, uz, rx, ry, rz, select(s, +ar, -ar, +ar))

        }
    ;
    #declare Fns[S][I_AA_F] = NoChangeFunctions[I_AA_F];
    #declare Fns[S][I_AA_R] = NoChangeFunctions[I_AA_R];
    #declare Fns[S][I_AA_U] = NoChangeFunctions[I_AA_U];
    #declare Fns[S][I_LLLL] = NoChangeFunctions[I_LLLL];
    #declare Fns[S][I_RRRR] = NoChangeFunctions[I_RRRR];
    #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];
    #local S = asc("+"); // Yaw right
    #declare Fns[S][I_PP_X] = NoChangeFunctions[I_PP_X];
    #declare Fns[S][I_PP_Y] = NoChangeFunctions[I_PP_Y];
    #declare Fns[S][I_PP_Z] = NoChangeFunctions[I_PP_Z];
    #declare Fns[S][I_VF_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnX(fx, fy, fz, ux, uy, uz, select(s, -au, +au, -au))
        }
    ;
    #declare Fns[S][I_VF_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnY(fx, fy, fz, ux, uy, uz, select(s, -au, +au, -au))
        }
    ;
    #declare Fns[S][I_VF_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnZ(fx, fy, fz, ux, uy, uz, select(s, -au, +au, -au))
        }
    ;
    #declare Fns[S][I_VR_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnX(rx, ry, rz, ux, uy, uz, select(s, -au, +au, -au))
        }
    ;
    #declare Fns[S][I_VR_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnY(rx, ry, rz, ux, uy, uz, select(s, -au, +au, -au))
        }
    ;
    #declare Fns[S][I_VR_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnZ(rx, ry, rz, ux, uy, uz, select(s, -au, +au, -au))
        }
    ;
    #declare Fns[S][I_VU_X] = NoChangeFunctions[I_VU_X];
    #declare Fns[S][I_VU_Y] = NoChangeFunctions[I_VU_Y];
    #declare Fns[S][I_VU_Z] = NoChangeFunctions[I_VU_Z];
    #declare Fns[S][I_AA_F] = NoChangeFunctions[I_AA_F];
    #declare Fns[S][I_AA_R] = NoChangeFunctions[I_AA_R];
    #declare Fns[S][I_AA_U] = NoChangeFunctions[I_AA_U];
    #declare Fns[S][I_LLLL] = NoChangeFunctions[I_LLLL];
    #declare Fns[S][I_RRRR] = NoChangeFunctions[I_RRRR];
    #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];
    #local S = asc("-"); // Yaw left
    #declare Fns[S][I_PP_X] = NoChangeFunctions[I_PP_X];
    #declare Fns[S][I_PP_Y] = NoChangeFunctions[I_PP_Y];
    #declare Fns[S][I_PP_Z] = NoChangeFunctions[I_PP_Z];
    #declare Fns[S][I_VF_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnX(fx, fy, fz, ux, uy, uz, select(s, +au, -au, +au))
        }
    ;
    #declare Fns[S][I_VF_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnY(fx, fy, fz, ux, uy, uz, select(s, +au, -au, +au))
        }
    ;
    #declare Fns[S][I_VF_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnZ(fx, fy, fz, ux, uy, uz, select(s, +au, -au, +au))
        }
    ;
    #declare Fns[S][I_VR_X] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnX(rx, ry, rz, ux, uy, uz, select(s, +au, -au, +au))
        }
    ;
    #declare Fns[S][I_VR_Y] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnY(rx, ry, rz, ux, uy, uz, select(s, +au, -au, +au))
        }
    ;
    #declare Fns[S][I_VR_Z] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            AxisRotateFnZ(rx, ry, rz, ux, uy, uz, select(s, +au, -au, +au))
        }
    ;
    #declare Fns[S][I_VU_X] = NoChangeFunctions[I_VU_X];
    #declare Fns[S][I_VU_Y] = NoChangeFunctions[I_VU_Y];
    #declare Fns[S][I_VU_Z] = NoChangeFunctions[I_VU_Z];
    #declare Fns[S][I_AA_F] = NoChangeFunctions[I_AA_F];
    #declare Fns[S][I_AA_R] = NoChangeFunctions[I_AA_R];
    #declare Fns[S][I_AA_U] = NoChangeFunctions[I_AA_U];
    #declare Fns[S][I_LLLL] = NoChangeFunctions[I_LLLL];
    #declare Fns[S][I_RRRR] = NoChangeFunctions[I_RRRR];
    #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];
    #local S = asc("~"); // Swap rotation directions for roll, pitch and yaw
    #declare Fns[S][I_PP_X] = NoChangeFunctions[I_PP_X];
    #declare Fns[S][I_PP_Y] = NoChangeFunctions[I_PP_Y];
    #declare Fns[S][I_PP_Z] = NoChangeFunctions[I_PP_Z];
    #declare Fns[S][I_VF_X] = NoChangeFunctions[I_VF_X];
    #declare Fns[S][I_VF_Y] = NoChangeFunctions[I_VF_Y];
    #declare Fns[S][I_VF_Z] = NoChangeFunctions[I_VF_Z];
    #declare Fns[S][I_VR_X] = NoChangeFunctions[I_VR_X];
    #declare Fns[S][I_VR_Y] = NoChangeFunctions[I_VR_Y];
    #declare Fns[S][I_VR_Z] = NoChangeFunctions[I_VR_Z];
    #declare Fns[S][I_VU_X] = NoChangeFunctions[I_VU_X];
    #declare Fns[S][I_VU_Y] = NoChangeFunctions[I_VU_Y];
    #declare Fns[S][I_VU_Z] = NoChangeFunctions[I_VU_Z];
    #declare Fns[S][I_AA_F] = NoChangeFunctions[I_AA_F];
    #declare Fns[S][I_AA_R] = NoChangeFunctions[I_AA_R];
    #declare Fns[S][I_AA_U] = NoChangeFunctions[I_AA_U];
    #declare Fns[S][I_LLLL] = NoChangeFunctions[I_LLLL];
    #declare Fns[S][I_RRRR] = NoChangeFunctions[I_RRRR];
    #declare Fns[S][I_SWAP] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            select(s, 0, 1, 0)
        }
    ;

#end // macro InsertBasicFunctions


#macro InsertLengthFunctions(Fns, LengthScale, LengthIncrement)

    #local S = asc(">");
    #declare Fns[S][I_PP_X] = NoChangeFunctions[I_PP_X];
    #declare Fns[S][I_PP_Y] = NoChangeFunctions[I_PP_Y];
    #declare Fns[S][I_PP_Z] = NoChangeFunctions[I_PP_Z];
    #declare Fns[S][I_VF_X] = NoChangeFunctions[I_VF_X];
    #declare Fns[S][I_VF_Y] = NoChangeFunctions[I_VF_Y];
    #declare Fns[S][I_VF_Z] = NoChangeFunctions[I_VF_Z];
    #declare Fns[S][I_VR_X] = NoChangeFunctions[I_VR_X];
    #declare Fns[S][I_VR_Y] = NoChangeFunctions[I_VR_Y];
    #declare Fns[S][I_VR_Z] = NoChangeFunctions[I_VR_Z];
    #declare Fns[S][I_VU_X] = NoChangeFunctions[I_VU_X];
    #declare Fns[S][I_VU_Y] = NoChangeFunctions[I_VU_Y];
    #declare Fns[S][I_VU_Z] = NoChangeFunctions[I_VU_Z];
    #declare Fns[S][I_AA_F] = NoChangeFunctions[I_AA_F];
    #declare Fns[S][I_AA_R] = NoChangeFunctions[I_AA_R];
    #declare Fns[S][I_AA_U] = NoChangeFunctions[I_AA_U];
    #declare Fns[S][I_LLLL] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            l*LengthScale + LengthIncrement
        }
    ;
    #declare Fns[S][I_RRRR] = NoChangeFunctions[I_RRRR];
    #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];
    #local S = asc("<");
    #declare Fns[S][I_PP_X] = NoChangeFunctions[I_PP_X];
    #declare Fns[S][I_PP_Y] = NoChangeFunctions[I_PP_Y];
    #declare Fns[S][I_PP_Z] = NoChangeFunctions[I_PP_Z];
    #declare Fns[S][I_VF_X] = NoChangeFunctions[I_VF_X];
    #declare Fns[S][I_VF_Y] = NoChangeFunctions[I_VF_Y];
    #declare Fns[S][I_VF_Z] = NoChangeFunctions[I_VF_Z];
    #declare Fns[S][I_VR_X] = NoChangeFunctions[I_VR_X];
    #declare Fns[S][I_VR_Y] = NoChangeFunctions[I_VR_Y];
    #declare Fns[S][I_VR_Z] = NoChangeFunctions[I_VR_Z];
    #declare Fns[S][I_VU_X] = NoChangeFunctions[I_VU_X];
    #declare Fns[S][I_VU_Y] = NoChangeFunctions[I_VU_Y];
    #declare Fns[S][I_VU_Z] = NoChangeFunctions[I_VU_Z];
    #declare Fns[S][I_AA_F] = NoChangeFunctions[I_AA_F];
    #declare Fns[S][I_AA_R] = NoChangeFunctions[I_AA_R];
    #declare Fns[S][I_AA_U] = NoChangeFunctions[I_AA_U];
    #declare Fns[S][I_LLLL] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            (l - LengthIncrement)/LengthScale
        }
    ;
    #declare Fns[S][I_RRRR] = NoChangeFunctions[I_RRRR];
    #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];

#end // macro InsertLengthFunctions


#macro InsertAngleFunctions(Fns, AngleScale, AngleIncrement)

    #local AngleInc = radians(AngleIncrement);
    #local S = asc("(");
    #declare Fns[S][I_PP_X] = NoChangeFunctions[I_PP_X];
    #declare Fns[S][I_PP_Y] = NoChangeFunctions[I_PP_Y];
    #declare Fns[S][I_PP_Z] = NoChangeFunctions[I_PP_Z];
    #declare Fns[S][I_VF_X] = NoChangeFunctions[I_VF_X];
    #declare Fns[S][I_VF_Y] = NoChangeFunctions[I_VF_Y];
    #declare Fns[S][I_VF_Z] = NoChangeFunctions[I_VF_Z];
    #declare Fns[S][I_VR_X] = NoChangeFunctions[I_VR_X];
    #declare Fns[S][I_VR_Y] = NoChangeFunctions[I_VR_Y];
    #declare Fns[S][I_VR_Z] = NoChangeFunctions[I_VR_Z];
    #declare Fns[S][I_VU_X] = NoChangeFunctions[I_VU_X];
    #declare Fns[S][I_VU_Y] = NoChangeFunctions[I_VU_Y];
    #declare Fns[S][I_VU_Z] = NoChangeFunctions[I_VU_Z];
    #declare Fns[S][I_AA_F] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            (af - AngleInc)/AngleScale
        }
    ;
    #declare Fns[S][I_AA_R] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            (ar - AngleInc)/AngleScale
        }
    ;
    #declare Fns[S][I_AA_U] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            (au - AngleInc)/AngleScale
        }
    ;
    #declare Fns[S][I_LLLL] = NoChangeFunctions[I_LLLL];
    #declare Fns[S][I_RRRR] = NoChangeFunctions[I_RRRR];
    #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];
    #local S = asc(")");
    #declare Fns[S][I_PP_X] = NoChangeFunctions[I_PP_X];
    #declare Fns[S][I_PP_Y] = NoChangeFunctions[I_PP_Y];
    #declare Fns[S][I_PP_Z] = NoChangeFunctions[I_PP_Z];
    #declare Fns[S][I_VF_X] = NoChangeFunctions[I_VF_X];
    #declare Fns[S][I_VF_Y] = NoChangeFunctions[I_VF_Y];
    #declare Fns[S][I_VF_Z] = NoChangeFunctions[I_VF_Z];
    #declare Fns[S][I_VR_X] = NoChangeFunctions[I_VR_X];
    #declare Fns[S][I_VR_Y] = NoChangeFunctions[I_VR_Y];
    #declare Fns[S][I_VR_Z] = NoChangeFunctions[I_VR_Z];
    #declare Fns[S][I_VU_X] = NoChangeFunctions[I_VU_X];
    #declare Fns[S][I_VU_Y] = NoChangeFunctions[I_VU_Y];
    #declare Fns[S][I_VU_Z] = NoChangeFunctions[I_VU_Z];
    #declare Fns[S][I_AA_F] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            af*AngleScale + AngleInc
        }
    ;
    #declare Fns[S][I_AA_R] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            ar*AngleScale + AngleInc
        }
    ;
    #declare Fns[S][I_AA_U] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            au*AngleScale + AngleInc
        }
    ;
    #declare Fns[S][I_LLLL] = NoChangeFunctions[I_LLLL];
    #declare Fns[S][I_RRRR] = NoChangeFunctions[I_RRRR];
    #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];

#end // macro InsertAngleFunctions


#macro InsertRadiusFunctions(Fns, RadiusScale, RadiusIncrement)

    #local S = asc("!");
    #declare Fns[S][I_PP_X] = NoChangeFunctions[I_PP_X];
    #declare Fns[S][I_PP_Y] = NoChangeFunctions[I_PP_Y];
    #declare Fns[S][I_PP_Z] = NoChangeFunctions[I_PP_Z];
    #declare Fns[S][I_VF_X] = NoChangeFunctions[I_VF_X];
    #declare Fns[S][I_VF_Y] = NoChangeFunctions[I_VF_Y];
    #declare Fns[S][I_VF_Z] = NoChangeFunctions[I_VF_Z];
    #declare Fns[S][I_VR_X] = NoChangeFunctions[I_VR_X];
    #declare Fns[S][I_VR_Y] = NoChangeFunctions[I_VR_Y];
    #declare Fns[S][I_VR_Z] = NoChangeFunctions[I_VR_Z];
    #declare Fns[S][I_VU_X] = NoChangeFunctions[I_VU_X];
    #declare Fns[S][I_VU_Y] = NoChangeFunctions[I_VU_Y];
    #declare Fns[S][I_VU_Z] = NoChangeFunctions[I_VU_Z];
    #declare Fns[S][I_AA_F] = NoChangeFunctions[I_AA_F];
    #declare Fns[S][I_AA_R] = NoChangeFunctions[I_AA_R];
    #declare Fns[S][I_AA_U] = NoChangeFunctions[I_AA_U];
    #declare Fns[S][I_LLLL] = NoChangeFunctions[I_LLLL];
    #declare Fns[S][I_RRRR] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            (r - RadiusIncrement)/RadiusScale
        }
    ;
    #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];
    #local S = asc("#");
    #declare Fns[S][I_PP_X] = NoChangeFunctions[I_PP_X];
    #declare Fns[S][I_PP_Y] = NoChangeFunctions[I_PP_Y];
    #declare Fns[S][I_PP_Z] = NoChangeFunctions[I_PP_Z];
    #declare Fns[S][I_VF_X] = NoChangeFunctions[I_VF_X];
    #declare Fns[S][I_VF_Y] = NoChangeFunctions[I_VF_Y];
    #declare Fns[S][I_VF_Z] = NoChangeFunctions[I_VF_Z];
    #declare Fns[S][I_VR_X] = NoChangeFunctions[I_VR_X];
    #declare Fns[S][I_VR_Y] = NoChangeFunctions[I_VR_Y];
    #declare Fns[S][I_VR_Z] = NoChangeFunctions[I_VR_Z];
    #declare Fns[S][I_VU_X] = NoChangeFunctions[I_VU_X];
    #declare Fns[S][I_VU_Y] = NoChangeFunctions[I_VU_Y];
    #declare Fns[S][I_VU_Z] = NoChangeFunctions[I_VU_Z];
    #declare Fns[S][I_AA_F] = NoChangeFunctions[I_AA_F];
    #declare Fns[S][I_AA_R] = NoChangeFunctions[I_AA_R];
    #declare Fns[S][I_AA_U] = NoChangeFunctions[I_AA_U];
    #declare Fns[S][I_LLLL] = NoChangeFunctions[I_LLLL];
    #declare Fns[S][I_RRRR] =
        function(x, y, z, fx, fy, fz, rx, ry, rz, ux, uy, uz, af, ar, au, l, r, s) {
            r*RadiusScale + RadiusIncrement
        }
    ;
    #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];

#end // macro InsertRadiusFunctions


#macro InsertNoChangeFunctions(Fns, Symbols)

    #for (I, 1, strlen(Symbols))
        #local Sym = substr(Symbols, I, 1);
        #local S = asc(Sym);
        #declare Fns[S][I_PP_X] = NoChangeFunctions[I_PP_X];
        #declare Fns[S][I_PP_Y] = NoChangeFunctions[I_PP_Y];
        #declare Fns[S][I_PP_Z] = NoChangeFunctions[I_PP_Z];
        #declare Fns[S][I_VF_X] = NoChangeFunctions[I_VF_X];
        #declare Fns[S][I_VF_Y] = NoChangeFunctions[I_VF_Y];
        #declare Fns[S][I_VF_Z] = NoChangeFunctions[I_VF_Z];
        #declare Fns[S][I_VR_X] = NoChangeFunctions[I_VR_X];
        #declare Fns[S][I_VR_Y] = NoChangeFunctions[I_VR_Y];
        #declare Fns[S][I_VR_Z] = NoChangeFunctions[I_VR_Z];
        #declare Fns[S][I_VU_X] = NoChangeFunctions[I_VU_X];
        #declare Fns[S][I_VU_Y] = NoChangeFunctions[I_VU_Y];
        #declare Fns[S][I_VU_Z] = NoChangeFunctions[I_VU_Z];
        #declare Fns[S][I_AA_F] = NoChangeFunctions[I_AA_F];
        #declare Fns[S][I_AA_R] = NoChangeFunctions[I_AA_R];
        #declare Fns[S][I_AA_U] = NoChangeFunctions[I_AA_U];
        #declare Fns[S][I_LLLL] = NoChangeFunctions[I_LLLL];
        #declare Fns[S][I_RRRR] = NoChangeFunctions[I_RRRR];
        #declare Fns[S][I_SWAP] = NoChangeFunctions[I_SWAP];
    #end // for

#end // macro InsertNoChangeFunctions

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12

#macro L_Transform(AxiomSymbols, L_Rules, NoOfIterations)

    #local AllSymbols = AxiomSymbols;
    #for (I, 1, NoOfIterations)
        #local IterSymbols = "";
        #for (J, 1, strlen(AllSymbols))
            #local Sym = substr(AllSymbols, J, 1);
            #local ProdSymbols = L_Rules[asc(Sym)];
            #if (strlen(ProdSymbols) > 0)
                #local IterSymbols = concat(IterSymbols, ProdSymbols);
            #end // if
        #end // for
        #local AllSymbols = IterSymbols;
    #end // for

    AllSymbols

#end // macro L_Transform


#macro L_Transform_Debug(AxiomSymbols, L_Rules, NoOfIterations)

    #debug "\n"
    #local I = 0;
    #debug concat("I = ", str(I, 0, 0), "\n")
    #local AllSymbols = AxiomSymbols;
    #debug "AllSymbols =\n"
    DebugString("    ", AllSymbols)
    #while (I < NoOfIterations)
        #local I = I + 1;
        #debug concat("I = ", str(I, 0, 0), "\n")
        #local IterSymbols = "";
        #for (J, 1, strlen(AllSymbols))
            #local Sym = substr(AllSymbols, J, 1);
            #local ProdSymbols = L_Rules[asc(Sym)];
            #if (strlen(ProdSymbols) > 0)
                #local IterSymbols = concat(IterSymbols, ProdSymbols);
            #end // if
        #end // for
        #local AllSymbols = IterSymbols;
        #debug "AllSymbols =\n"
        DebugString("    ", AllSymbols)
    #end // for
    #debug "\n"

    AllSymbols

#end // macro L_Transform_Debug


#macro L_Transform_DebugMore(AxiomSymbols, L_Rules, NoOfIterations)

    #debug "\n"
    #local I = 0;
    #debug concat("I = ", str(I, 0, 0), "\n")
    #local AllSymbols = AxiomSymbols;
    #debug "AllSymbols =\n"
    DebugString("    ", AllSymbols)
    #while (I < NoOfIterations)
        #local I = I + 1;
        #debug concat("I = ", str(I, 0, 0), "\n")
        #local IterSymbols = "";
        #debug "    IterSymbols =\n"
        DebugString("        ", IterSymbols)
        #for (J, 1, strlen(AllSymbols))
            #local Sym = substr(AllSymbols, J, 1);
            #local ProdSymbols = L_Rules[asc(Sym)];
            #debug concat("    Sym = ", Sym, "\n")
            #debug "    ProdSymbols =\n"
            DebugString("        ", ProdSymbols)
            #debug concat("    strlen(ProdSymbols) = ", str(strlen(ProdSymbols), 0, 0), "\n")
            #if (strlen(ProdSymbols) > 0)
                #local IterSymbols = concat(IterSymbols, ProdSymbols);
                #debug "    IterSymbols =\n"
                DebugString("        ", IterSymbols)
            #end // if
        #end // for
        #local AllSymbols = IterSymbols;
        #debug "AllSymbols =\n"
        DebugString("    ", AllSymbols)
    #end // for
    #debug "\n"

    AllSymbols

#end // macro L_Transform_DebugMore


#macro _FunctionEval(Fn, pP, vF, vR, vU, AngleF, AngleR, AngleU, Length, Radius, Swap)

    Fn(
        pP.x, pP.y, pP.z,
        vF.x, vF.y, vF.z,
        vR.x, vR.y, vR.z,
        vU.x, vU.y, vU.z,
        AngleF,
        AngleR,
        AngleU,
        Length,
        Radius,
        Swap
    )

#end // macro _FunctionEval


#macro L_Draw(Fns, Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials, Spheres, Cylinders)

    #local Nil = 1e-6;
    #if (StackSize > 0)
        #local SS = StackSize;
    #else
        #local SS = 100;
    #end // if
    #local NoOfMaterials = dimension_size(Materials, 1);
    #local PP_Stack = array[SS];
    #local VF_Stack = array[SS];
    #local VR_Stack = array[SS];
    #local VU_Stack = array[SS];
    #local AF_Stack = array[SS];
    #local AR_Stack = array[SS];
    #local AU_Stack = array[SS];
    #local LL_Stack = array[SS];
    #local RR_Stack = array[SS];
    #local SW_Stack = array[SS];
    #local MM_Stack = array[SS];
    // Initial start position
    #local p0 = pStart;
    // Initial forward direction
    #if (vlength(vF) > 0)
        #local vF0 = vF;
    #else
        #local vF0 = +x; // vF0 = vcross(vR0, vU0) = vcross(-z, +y)
    #end // if
    // Initial right direction
    #if (vlength(vR) > 0)
        #local vR0 = vR;
    #else
        #local vR0 = -z; // vR0 = vcross(vU0, vF0) = vcross(+y, +x)
    #end // if
    // Initial up direction
    #if (vlength(vU) > 0)
        #local vU0 = vU;
    #else
        #local vU0 = +y; // vU0 = vcross(vF0, vR0) = vcross(+x, -z)
    #end // if
    // Initial angle for rotation around forward axis; i.e. roll
    #local AngleF0 = radians(Angle);
    // Initial angle for rotation around right axis; i.e. pitch
    #local AngleR0 = radians(Angle);
    // Initial angle for rotation around up axis; i.e. yaw
    #local AngleU0 = radians(Angle);
    // Initial length
    #local Length0 = Length;
    // Initial radius
    #local Radius0 = Radius;
    // Initial swap
    #local Swap0 = 0; //
    // Initial Material no.
    #local MaterialNo = 0;
    #local SP = -1;
    #if (Spheres)
        sphere {
            p0, Radius0
            #if (NoOfMaterials > 1)
                material { Materials[MaterialNo] }
            #end // if
        }
    #end // if
    #for (I, 1, strlen(Symbols))
        #local Sym = substr(Symbols, I, 1);
        #local S = asc(Sym);
        #switch (S)
            #case (asc("[")) // Push
                #local SP = SP + 1;
                #local PP_Stack[SP] = p0;
                #local VF_Stack[SP] = vF0;
                #local VR_Stack[SP] = vR0;
                #local VU_Stack[SP] = vU0;
                #local AF_Stack[SP] = AngleF0;
                #local AR_Stack[SP] = AngleR0;
                #local AU_Stack[SP] = AngleU0;
                #local LL_Stack[SP] = Length0;
                #local RR_Stack[SP] = Radius0;
                #local SW_Stack[SP] = Swap0;
                #local MM_Stack[SP] = MaterialNo;
                #break
            #case (asc("]")) // Pop
                #local p0 = PP_Stack[SP];
                #local vF0 = VF_Stack[SP];
                #local vR0 = VR_Stack[SP];
                #local vU0 = VU_Stack[SP];
                #local AngleF0 = AF_Stack[SP];
                #local AngleR0 = AR_Stack[SP];
                #local AngleU0 = AU_Stack[SP];
                #local Length0 = LL_Stack[SP];
                #local Radius0 = RR_Stack[SP];
                #local Swap0 = SW_Stack[SP];
                #local MaterialNo = MM_Stack[SP];
                #local SP = SP - 1;
                #break
            #case (asc(":")) // 
                 #local MaterialNo = 0;
                #break
            #case (asc(";")) // 
                 #local MaterialNo = mod(MaterialNo + 1, NoOfMaterials);
                #break
            #case (asc("@"))
                sphere {
                    p0, Radius0
                    #if (NoOfMaterials > 1)
                        material { Materials[MaterialNo] }
                    #end // if
                }
                #break
            #else
                #local p1 =
                    <
                        _FunctionEval(
                            Fns[S][I_PP_X],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            Swap0
                        ),
                        _FunctionEval(
                            Fns[S][I_PP_Y],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            Swap0
                        ),
                        _FunctionEval(
                            Fns[S][I_PP_Z],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            Swap0
                        )
                    >
                ;
                #local vF1 =
                    <
                        _FunctionEval(
                            Fns[S][I_VF_X],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            Swap0
                        ),
                        _FunctionEval(
                            Fns[S][I_VF_Y],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            Swap0
                        ),
                        _FunctionEval(
                            Fns[S][I_VF_Z],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            Swap0
                        )
                    >
                ;
                #local vR1 =
                    <
                        _FunctionEval(
                            Fns[S][I_VR_X],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            Swap0
                        ),
                        _FunctionEval(
                            Fns[S][I_VR_Y],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            Swap0
                        ),
                        _FunctionEval(
                            Fns[S][I_VR_Z],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            Swap0
                        )
                    >
                ;
                #local vU1 =
                    <
                        _FunctionEval(
                            Fns[S][I_VU_X],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            Swap0
                        ),
                        _FunctionEval(
                            Fns[S][I_VU_Y],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            Swap0
                        ),
                        _FunctionEval(
                            Fns[S][I_VU_Z],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            Swap0
                        )
                    >
                ;
                #local AngleF1 =
                    _FunctionEval(
                        Fns[S][I_AA_F],
                        p0,
                        vF0,
                        vR0,
                        vU0,
                        AngleF0,
                        AngleR0,
                        AngleU0,
                        Length0,
                        Radius0,
                        Swap0
                    )
                ;
                #local AngleR1 =
                    _FunctionEval(
                        Fns[S][I_AA_R],
                        p0,
                        vF0,
                        vR0,
                        vU0,
                        AngleF0,
                        AngleR0,
                        AngleU0,
                        Length0,
                        Radius0,
                        Swap0
                    )
                ;
                #local AngleU1 =
                    _FunctionEval(
                        Fns[S][I_AA_U],
                        p0,
                        vF0,
                        vR0,
                        vU0,
                        AngleF0,
                        AngleR0,
                        AngleU0,
                        Length0,
                        Radius0,
                        Swap0
                    )
                ;
                #local Length1 =
                    _FunctionEval(
                        Fns[S][I_LLLL],
                        p0,
                        vF0,
                        vR0,
                        vU0,
                        AngleF0,
                        AngleR0,
                        AngleU0,
                        Length0,
                        Radius0,
                        Swap0
                    )
                ;
                #local Radius1 =
                    _FunctionEval(
                        Fns[S][I_RRRR],
                        p0,
                        vF0,
                        vR0,
                        vU0,
                        AngleF0,
                        AngleR0,
                        AngleU0,
                        Length0,
                        Radius0,
                        Swap0
                    )
                ;
                #local Swap1 =
                    _FunctionEval(
                        Fns[S][I_SWAP],
                        p0,
                        vF0,
                        vR0,
                        vU0,
                        AngleF0,
                        AngleR0,
                        AngleU0,
                        Length0,
                        Radius0,
                        Swap0
                    )
                ;
                #if ((S != asc("f")) & (vlength(p1 - p0) > Nil))
                    #if (Cylinders)
                        cylinder {
                            p0, p1, Radius0
                            #if (NoOfMaterials > 1)
                                material { Materials[MaterialNo] }
                            #end // if
                        }
                    #end // if
                    #if (Spheres)
                        sphere {
                            p1, Radius1
                            #if (NoOfMaterials > 1)
                                material { Materials[MaterialNo] }
                            #end // if
                        }
                    #end // if
                #end // if
                #local p0 = p1;
                #local vF0 = vF1;
                #local vR0 = vR1;
                #local vU0 = vU1;
                #local AngleF0 = AngleF1;
                #local AngleR0 = AngleR1;
                #local AngleU0 = AngleU1;
                #local Length0 = Length1;
                #local Radius0 = Radius1;
                #local Swap0 = Swap1;
        #end // switch
    #end // for

#end // macro L_Draw


#macro L_DrawSpheres(Fns, Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials)

    L_Draw(Fns, Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials, true, false)

#end // macro L_DrawSpheres


#macro L_DrawCylinders(Fns, Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials)

    L_Draw(Fns, Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials, false, true)

#end // macro L_DrawCylinders

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12

#declare Functions = array[MaxNoOfSymbols][18];

InsertBasicFunctions(Functions)


#declare NoMaterials = array[1] { material { } };

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12
