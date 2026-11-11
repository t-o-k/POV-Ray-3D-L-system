# POV-Ray-3D-L-system

[POV-Ray](http://www.povray.org) v3.7 include file with a library for a simple 3D Lindenmayer system.

Note that this library is work in progress, so everything has not been tested and anything may suddenly change.

#### Meaning of symbols/characters in rules arrays:

```
F    Move along forward vector by length; draw a cylinder if enabled
f    Move along forward vector by length without drawing
\    Roll left by turning angle (in degrees); i.e. rotate clockwise around the forward vector
/    Roll right by turning angle (in degrees); i.e. rotate counterclockwise around the forward vector
&    Pitch down by turning angle (in degrees); i.e. rotate clockwise around the right vector
^    Pitch up by turning angle (in degrees); i.e. rotate counterclockwise around the right vector
+    Yaw right by turning angle (in degrees); i.e. rotate clockwise around the up vector
-    Yaw left by turning angle (in degrees); i.e. rotate counterclockwise around the up vector
|    Turn around (Yaw by +180 degrees); i.e. rotate clockwise around the up vector
~    Swap rotation directions for roll, pitch and yaw
[    Push current drawing state onto the stack
]    Pop current drawing state from the stack
{    Push current triangle state onto the triangle stack
}    Pop triangle state from the triangle stack and generate triangles if enabled
.    Record current position as a triangle vertex
,    Record the second vertex again as a triangle vertex (useful for closing a polygonal loop around the first vertex)
>    Increase the cylinder length by multiplying by a scale factor and/or adding an increment
<    Decrease the cylinder length by subtracting an increment and/or dividing by a scale factor
#    Increase the cylinder radius by multiplying by a scale factor and/or adding an increment
!    Decrease the cylinder radius by subtracting an increment and/or dividing by a scale factor
(    Increase the turning angle by multiplying by a scale factor and/or adding an increment
)    Decrease the turning angle by subtracting an increment and/or dividing by a scale factor
@    Draw a sphere with cylinder radius if enabled
:    Select the first material in the array of materials
;    Select the next material in the array of materials. If the current material is the last one, the first material is selected
```

The following macros can be called to enable use of the corresponding symbols when interpreting the L-system:

```
> <    InsertLengthFunctions(Fns, LengthScale, LengthIncrement)
# !    InsertRadiusFunctions(Fns, RadiusScale, RadiusIncrement)
( )    InsertAngleFunctions(Fns, AngleScale, AngleIncrement)
: ;    InsertMaterialsFunctions(Fns, NoOfMaterials)
```

#### Drawing geometry

The following macros can be called to generate different combinations of geometry:

```
L_DrawSpheres()
L_DrawCylinders()
L_DrawTriangles()
L_DrawSpheresAndCylinders()
L_DrawSpheresAndTriangles()
L_DrawCylindersAndTriangles()
L_DrawAll()
```

Triangle groups are defined using `{` and `}`. The triangle state has its own stack, separate from the drawing state stack used by `[` and `]`.

#### A brief note on how it works

The library separates L-system transformation from drawing. L_Transform() generates the symbol string from the axiom and rules, and the L_Draw...() macros interpret the resulting string and generate geometry.

Functions used to control the turtle state can access its current position, orientation, turning angles, length, radius, material number and stack level.

#### Related projects and resources

* [POV-Ray 2D L-system](https://github.com/t-o-k/POV-Ray-2D-L-system) – The 2D companion library for flat fractal generation and plane turtle graphics.
* [Wikipedia: L-system](https://wikipedia.org) – General introduction to the mathematics and history of Lindenmayer systems.
* [Paul Bourke's L-System User Notes](http://paulbourke.net/fractals/lsys) – Excellent reference for core grammar concepts, bracketed stacks, and string scaling operations.
* [The Algorithmic Beauty of Plants](http://algorithmicbotany.org) – The definitive book by P. Prusinkiewicz and A. Lindenmayer. Essential for advanced 3D plant structures using pitch, roll, and yaw.
* [POV-Ray Official Documentation](http://povray.org) – For understanding scene descriptions, include file mechanics (`.inc`), and custom transformations.
