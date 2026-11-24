// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12
/*

https://github.com/t-o-k/POV-Ray-L-system_3D

Copyright (c) 2023-2026 Tor Olav Kristensen, http://subcube.com

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


#macro _TmpAxisRotateFn0(_Fn)

    function(x, y, z, _ax, _ay, _az, _w) {
        _Fn(x, y, z, _ax, _ay, _az, f_r(_ax, _ay, _az), cos(_w), sin(_w))
    }

#end // macro _TmpAxisRotateFn0


#macro _TmpAxisRotateFn1(_Fn)

    _TmpAxisRotateFn0(
        function(x, y, z, _ax, _ay, _az, _la, _c, _s) {
            _Fn(x, y, z, _ax/_la, _ay/_la, _az/_la, 1 - _c, _c, _s)
        }
    )

#end // macro _TmpAxisRotateFn1


#declare AxisRotateFnX = // function(x, y, z, _ax, _ay, _az, _w) { }
    _TmpAxisRotateFn1(
        function(x, y, z, _axn, _ayn, _azn, _omc, _c, _s) {
            (_axn*_axn*_omc +      _c)*x +
            (_axn*_ayn*_omc - _azn*_s)*y +
            (_axn*_azn*_omc + _ayn*_s)*z
        }
    )
;
#declare AxisRotateFnY = // function(x, y, z, _ax, _ay, _az, _w) { }
    _TmpAxisRotateFn1(
        function(x, y, z, _axn, _ayn, _azn, _omc, _c, _s) {
            (_ayn*_axn*_omc + _azn*_s)*x +
            (_ayn*_ayn*_omc +      _c)*y +
            (_ayn*_azn*_omc - _axn*_s)*z
        }
    )
;
#declare AxisRotateFnZ = // function(x, y, z, _ax, _ay, _az, _w) { }
    _TmpAxisRotateFn1(
        function(x, y, z, _axn, _ayn, _azn, _omc, _c, _s) {
            (_azn*_axn*_omc - _ayn*_s)*x +                       
            (_azn*_ayn*_omc + _axn*_s)*y +
            (_azn*_azn*_omc +      _c)*z
        }
    )
;

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12

#declare _MAX_NO_OF_SYMBOLS = 128; // No of characters in the ASCII table

#declare RULES = array[_MAX_NO_OF_SYMBOLS];


#macro ResetAllRules()

    #declare RULES[0] = ""
    // Declare the remaining symbols as constants
    #for (I, 1, _MAX_NO_OF_SYMBOLS - 1)
        #declare RULES[I] = chr(I)
    #end // for

#end // macro ResetAllRules


ResetAllRules()

/*
Warning / Uncaught edge cases for the macro AddRule(Character, Symbols):

1. Silent Space-Rule Overwrite under 'charset ascii':
    When global_settings { charset ascii } is active (POV-Ray 3.7 default), 
    POV-Ray's lexer replaces any non-ASCII byte (> 127) in a string literal
    with a space (" ", ASCII 32) BEFORE this macro runs. If your file is
    saved in a 1-byte encoding (e.g. Windows-1252 / ISO-8859-1) and you pass
    a single extended character (e.g. "Ã¸"), POV-Ray turns it into " " and
    this macro will SILENTLY OVERWRITE the rule for the space character.

2. Unvalidated 'Symbols' Replacement String:
    This macro only validates 'Character', not 'Symbols':
    - Including chr(0) inside 'Symbols' will silently truncate the string at
      the null byte.
    - Including non-ASCII characters in 'Symbols' will either be silently
      converted to spaces (under 'charset ascii') or expand into unmapped
      bytes >= 128 (under 'charset utf8' / 'sys') that fail during rule lookup.
*/

#macro AddRule(Character, Symbols)

    #local S = asc(Character);
    #if (S = 0)
        #error "Character can not be empty or the ASCII NULL character"
    #end // if
    #if (strlen(Character) > 1)
        #debug concat("Character: '", Character, "'\n")
        #error "Character is not a string with 1 character"
    #end // if
    #if (_MAX_NO_OF_SYMBOLS <= S)
        #debug concat("Character: '", Character, "' byte value: ", str(S, 0, 0), "\n")
        #error "Character is not a 7-bit ASCII character (1-127)"
    #end // if

    #declare RULES[S] = Symbols;

#end // macro AddRule

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12

#declare _I_PP_X =  0; // Position coordinate x
#declare _I_PP_Y =  1; // Position coordinate y
#declare _I_PP_Z =  2; // Position coordinate Z
#declare _I_VF_X =  3; // Forward vector coordinate x
#declare _I_VF_Y =  4; // Forward vector coordinate y
#declare _I_VF_Z =  5; // Forward vector coordinate z
#declare _I_VR_X =  6; // Right vector coordinate x
#declare _I_VR_Y =  7; // Right vector coordinate y
#declare _I_VR_Z =  8; // Right vector coordinate z
#declare _I_VU_X =  9; // Up vector coordinate x
#declare _I_VU_Y = 10; // Up vector coordinate y
#declare _I_VU_Z = 11; // Up vector coordinate z
#declare _I_SWAP = 12; // Swap directions of vectors
#declare _I_AA_F = 13; // Turn angle
#declare _I_AA_R = 14; // Turn angle
#declare _I_AA_U = 15; // Turn angle
#declare _I_LLLL = 16; // Length (distance to move when going forward)
#declare _I_RRRR = 17; // Radius
#declare _I_MATN = 18; // Material number

/*
Parameters to the functions:

<x, y, z>: Position vector
<_fx, _fy, _fz>: Forward direction vector
<_rx, _ry, _rz>: Right direction vector
<_ux, _uy, _uz>: Up direction vector
_s: If directions of vectors should be swapped
_af: Turn angle around forward direction vector
_ar: Turn angle around right direction vector
_au: Turn angle around up direction vector
_l: Length
_r: Radius
_m: Material number
_sl: Stack level
*/

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12

#declare _NO_CHANGE_FUNCTIONS = array[19];

#declare _NO_CHANGE_FUNCTIONS[_I_PP_X] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        x
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_PP_Y] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        y
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_PP_Z] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        z
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_VF_X] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _fx
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_VF_Y] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _fy
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_VF_Z] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _fz
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_VR_X] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _rx
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_VR_Y] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _ry
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_VR_Z] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _rz
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_VU_X] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _ux
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_VU_Y] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _uy
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_VU_Z] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _uz
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_SWAP] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _s
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_AA_F] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _af
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_AA_R] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _ar
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_AA_U] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _au
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_LLLL] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _l
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_RRRR] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _r
    }
;
#declare _NO_CHANGE_FUNCTIONS[_I_MATN] =
    function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
        _m
    }
;


#macro _InsertNoChangeFunctions(Character)

    #local S = asc(Character);
    #declare FUNCTIONS[S][_I_PP_X] = _NO_CHANGE_FUNCTIONS[_I_PP_X];
    #declare FUNCTIONS[S][_I_PP_Y] = _NO_CHANGE_FUNCTIONS[_I_PP_Y];
    #declare FUNCTIONS[S][_I_PP_Z] = _NO_CHANGE_FUNCTIONS[_I_PP_Z];
    #declare FUNCTIONS[S][_I_VF_X] = _NO_CHANGE_FUNCTIONS[_I_VF_X];
    #declare FUNCTIONS[S][_I_VF_Y] = _NO_CHANGE_FUNCTIONS[_I_VF_Y];
    #declare FUNCTIONS[S][_I_VF_Z] = _NO_CHANGE_FUNCTIONS[_I_VF_Z];
    #declare FUNCTIONS[S][_I_VR_X] = _NO_CHANGE_FUNCTIONS[_I_VR_X];
    #declare FUNCTIONS[S][_I_VR_Y] = _NO_CHANGE_FUNCTIONS[_I_VR_Y];
    #declare FUNCTIONS[S][_I_VR_Z] = _NO_CHANGE_FUNCTIONS[_I_VR_Z];
    #declare FUNCTIONS[S][_I_VU_X] = _NO_CHANGE_FUNCTIONS[_I_VU_X];
    #declare FUNCTIONS[S][_I_VU_Y] = _NO_CHANGE_FUNCTIONS[_I_VU_Y];
    #declare FUNCTIONS[S][_I_VU_Z] = _NO_CHANGE_FUNCTIONS[_I_VU_Z];
    #declare FUNCTIONS[S][_I_SWAP] = _NO_CHANGE_FUNCTIONS[_I_SWAP];
    #declare FUNCTIONS[S][_I_AA_F] = _NO_CHANGE_FUNCTIONS[_I_AA_F];
    #declare FUNCTIONS[S][_I_AA_R] = _NO_CHANGE_FUNCTIONS[_I_AA_R];
    #declare FUNCTIONS[S][_I_AA_U] = _NO_CHANGE_FUNCTIONS[_I_AA_U];
    #declare FUNCTIONS[S][_I_LLLL] = _NO_CHANGE_FUNCTIONS[_I_LLLL];
    #declare FUNCTIONS[S][_I_RRRR] = _NO_CHANGE_FUNCTIONS[_I_RRRR];
    #declare FUNCTIONS[S][_I_MATN] = _NO_CHANGE_FUNCTIONS[_I_MATN];

#end // macro _InsertNoChangeFunctions


#declare FUNCTIONS = array[_MAX_NO_OF_SYMBOLS][19];

// NB: asc(chr(0)) = asc("") = 0
#for (I, 0, _MAX_NO_OF_SYMBOLS - 1)
    _InsertNoChangeFunctions(chr(I))
#end // for

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12

#macro InsertBasicFunctions()
   
    // "@"  Do not move forward while "drawing"
    // "F"  Move forward while "drawing"
    // "f"  Move forward while not "drawing"
    // "|"  Turn around (Yaw +pi radians)
    // "\\"  Roll left
    // "/"  Roll right 
    // "&"  Pitch down
    // "^"  Pitch up
    // "+"  Yaw right
    // "-"  Yaw left
    // "~"  Swap rotation directions for roll, pitch and yaw
    // "@"  Do not move forward while "drawing"

    #local S = asc("F"); // Move forward while "drawing"
    #undef FUNCTIONS[S][_I_PP_X]
    #declare FUNCTIONS[S][_I_PP_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            x + _l*_fx/f_r(_fx, _fy, _fz)
        }
    ;
    #undef FUNCTIONS[S][_I_PP_Y]
    #declare FUNCTIONS[S][_I_PP_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            y + _l*_fy/f_r(_fx, _fy, _fz)
        }
    ;
    #undef FUNCTIONS[S][_I_PP_Z]
    #declare FUNCTIONS[S][_I_PP_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            z + _l*_fz/f_r(_fx, _fy, _fz)
        }
    ;

    #local S = asc("f"); // Move forward while not "drawing"
    #undef FUNCTIONS[S][_I_PP_X]
    #declare FUNCTIONS[S][_I_PP_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            x + _l*_fx/f_r(_fx, _fy, _fz)
        }
    ;
    #undef FUNCTIONS[S][_I_PP_Y]
    #declare FUNCTIONS[S][_I_PP_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            y + _l*_fy/f_r(_fx, _fy, _fz)
        }
    ;
    #undef FUNCTIONS[S][_I_PP_Z]
    #declare FUNCTIONS[S][_I_PP_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            z + _l*_fz/f_r(_fx, _fy, _fz)
        }
    ;

    #local S = asc("|"); // Turn around (Yaw +pi radians)
    #undef FUNCTIONS[S][_I_VF_X]
    #declare FUNCTIONS[S][_I_VF_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            -_fx
        }
    ;
    #undef FUNCTIONS[S][_I_VF_Y]
    #declare FUNCTIONS[S][_I_VF_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            -_fy
        }
    ;
    #undef FUNCTIONS[S][_I_VF_Z]
    #declare FUNCTIONS[S][_I_VF_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            -_fz
        }
    ;
    #undef FUNCTIONS[S][_I_VR_X]
    #declare FUNCTIONS[S][_I_VR_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            -_rx
        }
    ;
    #undef FUNCTIONS[S][_I_VR_Y]
    #declare FUNCTIONS[S][_I_VR_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            -_ry
        }
    ;
    #undef FUNCTIONS[S][_I_VR_Z]
    #declare FUNCTIONS[S][_I_VR_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            -_rz
        }
    ;

    #local S = asc("\\"); // Roll left
    #undef FUNCTIONS[S][_I_VR_X]
    #declare FUNCTIONS[S][_I_VR_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnX(_rx, _ry, _rz, _fx, _fy, _fz, select(_s, -_af, +_af, -_af))
        }
    ;
    #undef FUNCTIONS[S][_I_VR_Y]
    #declare FUNCTIONS[S][_I_VR_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnY(_rx, _ry, _rz, _fx, _fy, _fz, select(_s, -_af, +_af, -_af))
        }
    ;
    #undef FUNCTIONS[S][_I_VR_Z]
    #declare FUNCTIONS[S][_I_VR_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnZ(_rx, _ry, _rz, _fx, _fy, _fz, select(_s, -_af, +_af, -_af))
        }
    ;
    #undef FUNCTIONS[S][_I_VU_X]
    #declare FUNCTIONS[S][_I_VU_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnX(_ux, _uy, _uz, _fx, _fy, _fz, select(_s, -_af, +_af, -_af))
        }
    ;
    #undef FUNCTIONS[S][_I_VU_Y]
    #declare FUNCTIONS[S][_I_VU_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnY(_ux, _uy, _uz, _fx, _fy, _fz, select(_s, -_af, +_af, -_af))
        }
    ;
    #undef FUNCTIONS[S][_I_VU_Z]
    #declare FUNCTIONS[S][_I_VU_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnZ(_ux, _uy, _uz, _fx, _fy, _fz, select(_s, -_af, +_af, -_af))
        }
    ;

    #local S = asc("/"); // Roll right 
    #undef FUNCTIONS[S][_I_VR_X]
    #declare FUNCTIONS[S][_I_VR_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnX(_rx, _ry, _rz, _fx, _fy, _fz, select(_s, +_af, -_af, +_af))
        }
    ;
    #undef FUNCTIONS[S][_I_VR_Y]
    #declare FUNCTIONS[S][_I_VR_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnY(_rx, _ry, _rz, _fx, _fy, _fz, select(_s, +_af, -_af, +_af))
        }
    ;
    #undef FUNCTIONS[S][_I_VR_Z]
    #declare FUNCTIONS[S][_I_VR_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnZ(_rx, _ry, _rz, _fx, _fy, _fz, select(_s, +_af, -_af, +_af))
        }
    ;
    #undef FUNCTIONS[S][_I_VU_X]
    #declare FUNCTIONS[S][_I_VU_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnX(_ux, _uy, _uz, _fx, _fy, _fz, select(_s, +_af, -_af, +_af))
        }
    ;
    #undef FUNCTIONS[S][_I_VU_Y]
    #declare FUNCTIONS[S][_I_VU_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnY(_ux, _uy, _uz, _fx, _fy, _fz, select(_s, +_af, -_af, +_af))
        }
    ;
    #undef FUNCTIONS[S][_I_VU_Z]
    #declare FUNCTIONS[S][_I_VU_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnZ(_ux, _uy, _uz, _fx, _fy, _fz, select(_s, +_af, -_af, +_af))
        }
    ;

    #local S = asc("&"); // Pitch down
    #undef FUNCTIONS[S][_I_VF_X]
    #declare FUNCTIONS[S][_I_VF_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnX(_fx, _fy, _fz, _rx, _ry, _rz, select(_s, -_ar, +_ar, -_ar))
        }
    ;
    #undef FUNCTIONS[S][_I_VF_Y]
    #declare FUNCTIONS[S][_I_VF_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnY(_fx, _fy, _fz, _rx, _ry, _rz, select(_s, -_ar, +_ar, -_ar))
        }
    ;
    #undef FUNCTIONS[S][_I_VF_Z]
    #declare FUNCTIONS[S][_I_VF_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnZ(_fx, _fy, _fz, _rx, _ry, _rz, select(_s, -_ar, +_ar, -_ar))
        }
    ;
    #undef FUNCTIONS[S][_I_VU_X]
    #declare FUNCTIONS[S][_I_VU_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnX(_ux, _uy, _uz, _rx, _ry, _rz, select(_s, -_ar, +_ar, -_ar))
        }
    ;
    #undef FUNCTIONS[S][_I_VU_Y]
    #declare FUNCTIONS[S][_I_VU_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnY(_ux, _uy, _uz, _rx, _ry, _rz, select(_s, -_ar, +_ar, -_ar))
        }
    ;
    #undef FUNCTIONS[S][_I_VU_Z]
    #declare FUNCTIONS[S][_I_VU_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnZ(_ux, _uy, _uz, _rx, _ry, _rz, select(_s, -_ar, +_ar, -_ar))
        }
    ;

    #local S = asc("^"); // Pitch up
    #undef FUNCTIONS[S][_I_VF_X]
    #declare FUNCTIONS[S][_I_VF_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnX(_fx, _fy, _fz, _rx, _ry, _rz, select(_s, +_ar, -_ar, +_ar))
        }
    ;
    #undef FUNCTIONS[S][_I_VF_Y]
    #declare FUNCTIONS[S][_I_VF_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnY(_fx, _fy, _fz, _rx, _ry, _rz, select(_s, +_ar, -_ar, +_ar))
        }
    ;
    #undef FUNCTIONS[S][_I_VF_Z]
    #declare FUNCTIONS[S][_I_VF_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnZ(_fx, _fy, _fz, _rx, _ry, _rz, select(_s, +_ar, -_ar, +_ar))
        }
    ;
    #undef FUNCTIONS[S][_I_VU_X]
    #declare FUNCTIONS[S][_I_VU_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnX(_ux, _uy, _uz, _rx, _ry, _rz, select(_s, +_ar, -_ar, +_ar))
        }
    ;
    #undef FUNCTIONS[S][_I_VU_Y]
    #declare FUNCTIONS[S][_I_VU_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnY(_ux, _uy, _uz, _rx, _ry, _rz, select(_s, +_ar, -_ar, +_ar))
        }
    ;
    #undef FUNCTIONS[S][_I_VU_Z]
    #declare FUNCTIONS[S][_I_VU_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnZ(_ux, _uy, _uz, _rx, _ry, _rz, select(_s, +_ar, -_ar, +_ar))
        }
    ;

    #local S = asc("+"); // Yaw right
    #undef FUNCTIONS[S][_I_VF_X]
    #declare FUNCTIONS[S][_I_VF_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnX(_fx, _fy, _fz, _ux, _uy, _uz, select(_s, -_au, +_au, -_au))
        }
    ;
    #undef FUNCTIONS[S][_I_VF_Y]
    #declare FUNCTIONS[S][_I_VF_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnY(_fx, _fy, _fz, _ux, _uy, _uz, select(_s, -_au, +_au, -_au))
        }
    ;
    #undef FUNCTIONS[S][_I_VF_Z]
    #declare FUNCTIONS[S][_I_VF_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnZ(_fx, _fy, _fz, _ux, _uy, _uz, select(_s, -_au, +_au, -_au))
        }
    ;
    #undef FUNCTIONS[S][_I_VR_X]
    #declare FUNCTIONS[S][_I_VR_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnX(_rx, _ry, _rz, _ux, _uy, _uz, select(_s, -_au, +_au, -_au))
        }
    ;
    #undef FUNCTIONS[S][_I_VR_Y]
    #declare FUNCTIONS[S][_I_VR_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnY(_rx, _ry, _rz, _ux, _uy, _uz, select(_s, -_au, +_au, -_au))
        }
    ;
    #undef FUNCTIONS[S][_I_VR_Z]
    #declare FUNCTIONS[S][_I_VR_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnZ(_rx, _ry, _rz, _ux, _uy, _uz, select(_s, -_au, +_au, -_au))
        }
    ;

    #local S = asc("-"); // Yaw left
    #undef FUNCTIONS[S][_I_VF_X]
    #declare FUNCTIONS[S][_I_VF_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnX(_fx, _fy, _fz, _ux, _uy, _uz, select(_s, +_au, -_au, +_au))
        }
    ;
    #undef FUNCTIONS[S][_I_VF_Y]
    #declare FUNCTIONS[S][_I_VF_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnY(_fx, _fy, _fz, _ux, _uy, _uz, select(_s, +_au, -_au, +_au))
        }
    ;
    #undef FUNCTIONS[S][_I_VF_Z]
    #declare FUNCTIONS[S][_I_VF_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnZ(_fx, _fy, _fz, _ux, _uy, _uz, select(_s, +_au, -_au, +_au))
        }
    ;
    #undef FUNCTIONS[S][_I_VR_X]
    #declare FUNCTIONS[S][_I_VR_X] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnX(_rx, _ry, _rz, _ux, _uy, _uz, select(_s, +_au, -_au, +_au))
        }
    ;
    #undef FUNCTIONS[S][_I_VR_Y]
    #declare FUNCTIONS[S][_I_VR_Y] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnY(_rx, _ry, _rz, _ux, _uy, _uz, select(_s, +_au, -_au, +_au))
        }
    ;
    #undef FUNCTIONS[S][_I_VR_Z]
    #declare FUNCTIONS[S][_I_VR_Z] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            AxisRotateFnZ(_rx, _ry, _rz, _ux, _uy, _uz, select(_s, +_au, -_au, +_au))
        }
    ;

    #local S = asc("~"); // Swap rotation directions for roll, pitch and yaw
    #undef FUNCTIONS[S][_I_SWAP]
    #declare FUNCTIONS[S][_I_SWAP] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            select(_s, 0, 1, 0)
        }
    ;

#end // macro InsertBasicFunctions


#macro InsertAngleFunctions(AngleScale, AngleIncrement)

    #local AngleInc = radians(AngleIncrement);

    #local S = asc("(");
    #undef FUNCTIONS[S][_I_AA_F]
    #declare FUNCTIONS[S][_I_AA_F] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            (_af - AngleInc)/AngleScale
        }
    ;
    #undef FUNCTIONS[S][_I_AA_R]
    #declare FUNCTIONS[S][_I_AA_R] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            (_ar - AngleInc)/AngleScale
        }
    ;
    #undef FUNCTIONS[S][_I_AA_U]
    #declare FUNCTIONS[S][_I_AA_U] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            (_au - AngleInc)/AngleScale
        }
    ;

    #local S = asc(")");
    #undef FUNCTIONS[S][_I_AA_F]
    #declare FUNCTIONS[S][_I_AA_F] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            _af*AngleScale + AngleInc
        }
    ;
    #undef FUNCTIONS[S][_I_AA_R]
    #declare FUNCTIONS[S][_I_AA_R] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            _ar*AngleScale + AngleInc
        }
    ;
    #undef FUNCTIONS[S][_I_AA_U]
    #declare FUNCTIONS[S][_I_AA_U] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            _au*AngleScale + AngleInc
        }
    ;

#end // macro InsertAngleFunctions


#macro InsertLengthFunctions(LengthScale, LengthIncrement)

    #local S = asc("<");
    #undef FUNCTIONS[S][_I_LLLL]
    #declare FUNCTIONS[S][_I_LLLL] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            (_l - LengthIncrement)/LengthScale
        }
    ;

    #local S = asc(">");
    #undef FUNCTIONS[S][_I_LLLL]
    #declare FUNCTIONS[S][_I_LLLL] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            _l*LengthScale + LengthIncrement
        }
    ;

#end // macro InsertLengthFunctions


#macro InsertRadiusFunctions(RadiusScale, RadiusIncrement)

    #local S = asc("!");
    #undef FUNCTIONS[S][_I_RRRR]
    #declare FUNCTIONS[S][_I_RRRR] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            (_r - RadiusIncrement)/RadiusScale
        }
    ;

    #local S = asc("#");
    #undef FUNCTIONS[S][_I_RRRR]
    #declare FUNCTIONS[S][_I_RRRR] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            _r*RadiusScale + RadiusIncrement
        }
    ;

#end // macro InsertRadiusFunctions


#macro InsertMaterialsFunctions(NoOfMaterials)

    #local S = asc(":");
    #undef FUNCTIONS[S][_I_MATN]
    #declare FUNCTIONS[S][_I_MATN] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            0
        }
    ;

    #local S = asc(";");
    #undef FUNCTIONS[S][_I_MATN]
    #declare FUNCTIONS[S][_I_MATN] =
        function(x, y, z, _fx, _fy, _fz, _rx, _ry, _rz, _ux, _uy, _uz, _s, _af, _ar, _au, _l, _r, _m, _sl) {
            mod(_m + 1, NoOfMaterials)
        }
    ;

#end // macro InsertMaterialsFunctions

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12

#macro L_Transform(AxiomSymbols, NoOfIterations)

    #local AllSymbols = AxiomSymbols;
    #for (I, 1, NoOfIterations)
        #local IterSymbols = "";
        #for (J, 1, strlen(AllSymbols))
            #local Sym = substr(AllSymbols, J, 1);
            #local ProdSymbols = RULES[asc(Sym)];
            #if (strlen(ProdSymbols) > 0)
                #local IterSymbols = concat(IterSymbols, ProdSymbols);
            #end // if
        #end // for
        #local AllSymbols = IterSymbols;
    #end // for

    AllSymbols

#end // macro L_Transform


#macro L_Transform_Debug(AxiomSymbols, NoOfIterations)

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
            #local ProdSymbols = RULES[asc(Sym)];
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


#macro L_Transform_DebugMore(AxiomSymbols, NoOfIterations)

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
            #local ProdSymbols = RULES[asc(Sym)];
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


#macro _FunctionEval(Fn, pP, vF, vR, vU, Swap, AngleF, AngleR, AngleU, Length, Radius, MatNo, StackLvl)

    Fn(
        pP.x, pP.y, pP.z,
        vF.x, vF.y, vF.z,
        vR.x, vR.y, vR.z,
        vU.x, vU.y, vU.z,
        Swap,
        AngleF,
        AngleR,
        AngleU,
        Length,
        Radius,
        MatNo,
        StackLvl
    )

#end // macro _FunctionEval


#macro L_Draw(Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials, Spheres, Cylinders, Triangles)

    #local Nil = 1e-6;
    #local NoOfMaterials = dimension_size(Materials, 1);
    #local UseMaterials = (NoOfMaterials > 1);
    #local T_MaterialNo = 0;
    #local T_MaxNoOfVertices = 100;  // Maximum number of vertices for triangles
    #local T_Vertices = array[T_MaxNoOfVertices];  // Vertices for triangles
    #local T_VertexCnt = 0;  // Number Of vertices for triangles
    #local T_SS = 100;  // Stack size for triangles
    #local TT_MaterialNos = array[T_SS];
    #local TT_Vertices = array[T_SS];
    #local TT_VertexCnts = array[T_SS];
    #if (StackSize > 0)
        #local SS = StackSize;
    #else
        #local SS = 100;
    #end // if
    #local PP_Stack = array[SS];
    #local VF_Stack = array[SS];
    #local VR_Stack = array[SS];
    #local VU_Stack = array[SS];
    #local SW_Stack = array[SS];
    #local AF_Stack = array[SS];
    #local AR_Stack = array[SS];
    #local AU_Stack = array[SS];
    #local LL_Stack = array[SS];
    #local RR_Stack = array[SS];
    #local MM_Stack = array[SS];
    // Initial start position
    #local p0 = pStart;
    // Initial direction vectors
    #if ((vlength(vF) > 0) & (vlength(vR) > 0) & (vlength(vU) > 0))
        #local vF0 = vF;
        #local vR0 = vR;
        #local vU0 = vU;
    #else
        #debug "Warning:\n"
        #debug "One or more initial direction vectors have zero length\n"
        #debug "Using default vectors (+x, -z, +y)\n"
        #local vF0 = +x; // vF0 = vcross(vR0, vU0) = vcross(-z, +y)
        #local vR0 = -z; // vR0 = vcross(vU0, vF0) = vcross(+y, +x)
        #local vU0 = +y; // vU0 = vcross(vF0, vR0) = vcross(+x, -z)
    #end // if
    // Initial swap
    #local Swap0 = 0;
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
    // Initial material number
    #local MaterialNo0 = 0;
    #local T_SP = -1;
    #local SP = -1;
    #for (I, 1, strlen(Symbols))
        #local Sym = substr(Symbols, I, 1);
        #local S = asc(Sym);
        #switch (S)
            #case (asc(".")) // Record current position as a new triangle vertex
                #local T_Vertices[T_VertexCnt] = p0;
                #local T_VertexCnt = T_VertexCnt + 1;
                #break
            #case (asc(",")) // Re-record triangle vertex number 1 as a new triangle vertex
                #if (T_VertexCnt < 2)
                    #error "Vertex number 1 for triangles has not been recorded yet"
                #end // if
                #local T_Vertices[T_VertexCnt] = T_Vertices[1];
                #local T_VertexCnt = T_VertexCnt + 1;
                #break
            #case (asc("{")) // Push triangles
                #if (T_SP >= T_SS - 1)
                    #error "The triangle stack is full"
                #end // if
                #local T_SP = T_SP + 1;
                #local TT_MaterialNos[T_SP] = T_MaterialNo;
                #local TT_Vertices[T_SP] = T_Vertices;
                #local TT_VertexCnts[T_SP] = T_VertexCnt;
                #local T_MaterialNo = MaterialNo0;
                #local T_VertexCnt = 0;
                #local T_Vertices[T_VertexCnt] = p0;
                #local T_VertexCnt = T_VertexCnt + 1;
                #break
            #case (asc("}")) // Pop triangles
                #if (T_SP < 0)
                    #error "The triangle stack is empty"
                #end // if
                #if (Triangles)
                    #if (1 < T_VertexCnt)
                        #if (T_VertexCnt < 3)
                            #error "Too few vertices for triangles"
                        #end // if
                        #local pCommon = T_Vertices[0];
                        #for (J, 1, T_VertexCnt - 2)
                            #local pA = T_Vertices[J];
                            #local pB = T_Vertices[J+1];
                            #if (vlength(vcross(pA - pCommon, pB - pCommon)) <= Nil)
                                #warning "Degenerate triangle"
                            #else
                                triangle {
                                    pCommon, pA, pB
                                    #if (UseMaterials)
                                        material { Materials[T_MaterialNo] }
                                    #end // if
                                }
                            #end // if
                        #end // for
                    #end // if
                #end // if
                #local T_MaterialNo = TT_MaterialNos[T_SP];
                #local T_Vertices = TT_Vertices[T_SP];
                #local T_VertexCnt = TT_VertexCnts[T_SP];
                #local T_SP = T_SP - 1;
                #break
            #case (asc("[")) // Push
                #if (SP >= SS - 1)
                    #error "The stack is full"
                #end // if
                #local SP = SP + 1;
                #local PP_Stack[SP] = p0;
                #local VF_Stack[SP] = vF0;
                #local VR_Stack[SP] = vR0;
                #local VU_Stack[SP] = vU0;
                #local SW_Stack[SP] = Swap0;
                #local AF_Stack[SP] = AngleF0;
                #local AR_Stack[SP] = AngleR0;
                #local AU_Stack[SP] = AngleU0;
                #local LL_Stack[SP] = Length0;
                #local RR_Stack[SP] = Radius0;
                #local MM_Stack[SP] = MaterialNo0;
                #break
            #case (asc("]")) // Pop
                #if (SP < 0)
                    #error "The stack is empty"
                #end // if
                #local p0 = PP_Stack[SP];
                #local vF0 = VF_Stack[SP];
                #local vR0 = VR_Stack[SP];
                #local vU0 = VU_Stack[SP];
                #local Swap0 = SW_Stack[SP];
                #local AngleF0 = AF_Stack[SP];
                #local AngleR0 = AR_Stack[SP];
                #local AngleU0 = AU_Stack[SP];
                #local Length0 = LL_Stack[SP];
                #local Radius0 = RR_Stack[SP];
                #local MaterialNo0 = MM_Stack[SP];
                #local SP = SP - 1;
                #break
            #else
                #local StackLevel = SP + 1;
                #local p1 =
                    <
                        _FunctionEval(
                            FUNCTIONS[S][_I_PP_X],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            Swap0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            MaterialNo0,
                            StackLevel
                        ),
                        _FunctionEval(
                            FUNCTIONS[S][_I_PP_Y],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            Swap0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            MaterialNo0,
                            StackLevel
                        ),
                        _FunctionEval(
                            FUNCTIONS[S][_I_PP_Z],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            Swap0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            MaterialNo0,
                            StackLevel
                        )
                    >
                ;
                #local vF1 =
                    <
                        _FunctionEval(
                            FUNCTIONS[S][_I_VF_X],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            Swap0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            MaterialNo0,
                            StackLevel
                        ),
                        _FunctionEval(
                            FUNCTIONS[S][_I_VF_Y],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            Swap0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            MaterialNo0,
                            StackLevel
                        ),
                        _FunctionEval(
                            FUNCTIONS[S][_I_VF_Z],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            Swap0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            MaterialNo0,
                            StackLevel
                        )
                    >
                ;
                #local vR1 =
                    <
                        _FunctionEval(
                            FUNCTIONS[S][_I_VR_X],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            Swap0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            MaterialNo0,
                            StackLevel
                        ),
                        _FunctionEval(
                            FUNCTIONS[S][_I_VR_Y],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            Swap0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            MaterialNo0,
                            StackLevel
                        ),
                        _FunctionEval(
                            FUNCTIONS[S][_I_VR_Z],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            Swap0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            MaterialNo0,
                            StackLevel
                        )
                    >
                ;
                #local vU1 =
                    <
                        _FunctionEval(
                            FUNCTIONS[S][_I_VU_X],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            Swap0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            MaterialNo0,
                            StackLevel
                        ),
                        _FunctionEval(
                            FUNCTIONS[S][_I_VU_Y],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            Swap0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            MaterialNo0,
                            StackLevel
                        ),
                        _FunctionEval(
                            FUNCTIONS[S][_I_VU_Z],
                            p0,
                            vF0,
                            vR0,
                            vU0,
                            Swap0,
                            AngleF0,
                            AngleR0,
                            AngleU0,
                            Length0,
                            Radius0,
                            MaterialNo0,
                            StackLevel
                        )
                    >
                ;
                #local Swap1 =
                    _FunctionEval(
                        FUNCTIONS[S][_I_SWAP],
                        p0,
                        vF0,
                        vR0,
                        vU0,
                        Swap0,
                        AngleF0,
                        AngleR0,
                        AngleU0,
                        Length0,
                        Radius0,
                        MaterialNo0,
                        StackLevel
                    )
                ;
                #local AngleF1 =
                    _FunctionEval(
                        FUNCTIONS[S][_I_AA_F],
                        p0,
                        vF0,
                        vR0,
                        vU0,
                        Swap0,
                        AngleF0,
                        AngleR0,
                        AngleU0,
                        Length0,
                        Radius0,
                        MaterialNo0,
                        StackLevel
                    )
                ;
                #local AngleR1 =
                    _FunctionEval(
                        FUNCTIONS[S][_I_AA_R],
                        p0,
                        vF0,
                        vR0,
                        vU0,
                        Swap0,
                        AngleF0,
                        AngleR0,
                        AngleU0,
                        Length0,
                        Radius0,
                        MaterialNo0,
                        StackLevel
                    )
                ;
                #local AngleU1 =
                    _FunctionEval(
                        FUNCTIONS[S][_I_AA_U],
                        p0,
                        vF0,
                        vR0,
                        vU0,
                        Swap0,
                        AngleF0,
                        AngleR0,
                        AngleU0,
                        Length0,
                        Radius0,
                        MaterialNo0,
                        StackLevel
                    )
                ;
                #local Length1 =
                    _FunctionEval(
                        FUNCTIONS[S][_I_LLLL],
                        p0,
                        vF0,
                        vR0,
                        vU0,
                        Swap0,
                        AngleF0,
                        AngleR0,
                        AngleU0,
                        Length0,
                        Radius0,
                        MaterialNo0,
                        StackLevel
                    )
                ;
                #local Radius1 =
                    _FunctionEval(
                        FUNCTIONS[S][_I_RRRR],
                        p0,
                        vF0,
                        vR0,
                        vU0,
                        Swap0,
                        AngleF0,
                        AngleR0,
                        AngleU0,
                        Length0,
                        Radius0,
                        MaterialNo0,
                        StackLevel
                    )
                ;
                #local MaterialNo1 =
                    _FunctionEval(
                        FUNCTIONS[S][_I_MATN],
                        p0,
                        vF0,                                        
                        vR0,
                        vU0,
                        Swap0,
                        AngleF0,
                        AngleR0,
                        AngleU0,
                        Length0,
                        Radius0,
                        MaterialNo0,
                        StackLevel
                    )
                ;
                #if (S = asc("@"))
                    #if (Spheres)
                        #if (Radius0 <= Nil)
                            #warning "Degenerate sphere"
                        #else
                            sphere {
                                p0, Radius0
                                #if (UseMaterials)
                                    material { Materials[MaterialNo0] }
                                #end // if
                            }
                        #end // if
                    #end // if
                #end // if
                #if (S = asc("F"))
                    #if (Cylinders)
                        #if ((Radius0 <= Nil) | (vlength(p1 - p0) <= Nil))
                            #warning "Degenerate cylinder"
                        #else
                            cylinder {
                                p0, p1, Radius0
                                #if (UseMaterials)
                                    material { Materials[MaterialNo0] }
                                #end // if
                            }
                        #end // if
                    #end // if
                    #if (Spheres)
                        #if (Radius1 <= Nil)
                            #warning "Degenerate sphere"
                        #else
                            sphere {
                                p1, Radius1
                                #if (UseMaterials)
                                    material { Materials[MaterialNo0] }
                                #end // if
                            }
                        #end // if
                    #end // if
                #end // if
                #local p0 = p1;
                #local vF0 = vF1;
                #local vR0 = vR1;
                #local vU0 = vU1;
                #local Swap0 = Swap1;
                #local AngleF0 = AngleF1;
                #local AngleR0 = AngleR1;
                #local AngleU0 = AngleU1;
                #local Length0 = Length1;
                #local Radius0 = Radius1;
                #local MaterialNo0 = MaterialNo1;
        #end // switch
    #end // for

#end // macro L_Draw


#macro L_DrawSpheres(Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials)

    L_Draw(
        Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials,
        true, false, false
    )

#end // macro L_DrawSpheres


#macro L_DrawCylinders(Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials)

    L_Draw(
        Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials,
        false, true, false
    )

#end // macro L_DrawCylinders


#macro L_DrawTriangles(Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials)

    L_Draw(
       Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials,
       false, false, true
    )

#end // macro L_DrawTriangles


#macro L_DrawSpheresAndCylinders(Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials)

    L_Draw(
        Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials,
        true, true, false
    )

#end // macro L_DrawSpheresAndCylinders


#macro L_DrawSpheresAndTriangles(Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials)

    L_Draw(
        Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials,
        true, false, true
    )

#end // macro L_DrawSpheresAndTriangles


#macro L_DrawCylindersAndTriangles(Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials)

    L_Draw(
        Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials,
        false, true, true
    )

#end // macro L_DrawCylindersAndTriangles


#macro L_DrawAll(Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials)

    L_Draw(
        Symbols, StackSize, pStart, vF, vR, vU, Angle, Length, Radius, Materials,
        true, true, true
    )

#end // macro L_DrawAll

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12

InsertBasicFunctions()

#declare NoMaterials = array[1] { material { } };

// ===== 1 ======= 2 ======= 3 ======= 4 ======= 5 ======= 6 ======= 7 ======= 8 ======= 9 ====== 10 ====== 11 ====== 12
