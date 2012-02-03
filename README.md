#CS535-Sketch

Sketch is a small application to test primitive drawing algorithms in OpenGL.

#Basic Usage

Sketch uses a simplified UI consisting of keyboard commands. Press the `H` 
key at any time to toggle a dropdown menu showing each of the commands. 

	+---------------------+
	| H - Toggle Help     |
	+---------------------+
	| L - Line            |
	| O - Circle          |
	| S - Curve           |
	| C - Clock           |
	+---------------------+
	| P - Control Points  |
	+---------------------+
	| U - Undo            |
	| X - Clear           |
	| Q - Quit            |
	+---------------------+

The first four commands switch the Sketch's drawing state. The middle command 
`P` shows the control points for any Bezier curves on screen. The last commands
do almost exactly what a reasonable person would expect, execpt that `Undo` is
drawing state sensitive. For example, in the `Line` state, only lines are 
removed (in the reverse order that they were drawn).

#Drawing

In the `Line`, `Circle`, and `Clock` states, the user clicks the left mouse
button down to indicate a starting point, drags the mouse to an end point, 
and releases the left mouse button. While the left mouse button is held down, a
temporary primitive will "rubber-band" beteen the start and end points.

In the `Curve` state, the user indicates 4 control points using a full click
and release. A line is drawn between each point until the fourth point is drawn
and a Bezier curve is generated. 

#Contact

Christopher D. Parks <chris.parks@uky.edu>
