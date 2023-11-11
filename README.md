# POV-Ray-3D-L-system
[POV-Ray](http://www.povray.org) v3.7 include file with a library for a simple 3D Lindenmayer system

https://en.wikipedia.org/wiki/L-system

Note that this library is work in progress, so everything has not been tested and anything may suddenly change.

#### Meaning of symbols/characters in rules arrays:
```
F    Move along forward vector by length drawing a cylinder
f    Move along forward vector by length without drawing
\    Roll left by turning angle (in degrees); i.e. rotate clockwise around the forward vector
/    Roll right by turning angle (in degrees); i.e. rotate counterclockwise around the forward vector
&    Pitch down by turning angle (in degrees); i.e. rotate clockwise around the right vector
^    Pitch up by turning angle (in degrees); i.e. rotate counterclockwise around the right vector
+    Yaw right by turning angle (in degrees); i.e. rotate clockwise around the up vector
-    Yaw left by turning angle (in degrees); i.e. rotate counterclockwise around the up vector
|    Turn around (Yaw by +180 degrees); i.e. rotate clockwise around the up vector
~    Swap rotation directions for roll, pitch and yaw
[    Push current drawing state onto stack
]    Pop current drawing state from the stack
>    Increase the cylinder length by multiplying by a scale factor and/or adding an increment
<    Decrease the cylinder length by subtracting an increment and/or dividing by a scale factor
#    Increase the cylinder radius by multiplying by a scale factor and/or adding an increment
!    Decrease the cylinder radius by subtracting an increment and/or dividing by a scale factor
(    Increase the turning angle by multiplying by a scale factor and/or adding an increment
)    Decrease the turning angle by subtracting an increment and/or dividing by a scale factor
@    Draw a sphere with cylinder radius
```

The following macros must be called before the L_Transform() macro in order to enable use of the symbols listed before them:

```
> <    InsertLengthFunctions()
# !    InsertRadiusFunctions()
( )    InsertAngleFunctions()
```


\- and the POV-Ray 2D L-system here:

https://github.com/t-o-k/POV-Ray-2D-L-system
