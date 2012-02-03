#ifndef DRAW_CONTEXT_H
#define DRAW_CONTEXT_H

#include <vector>
using namespace std;

#include "Globals.h"
#include "VertexBuffer.h"

// DrawContext can be in one of 5 states 
enum State { LINE, CIRCLE, CURVE, CLOCK, UNKNOWN };

// DrawContext maintains the state of the sketch pad as well
// as a stack of pixel data for each drawing type.
class DrawContext{
public:
	// Public callbacks for glut (cannot use instance methods for 
	// callbacks).
	static void Display(void);
	static void Keyboard(unsigned char key, int x, int y);
	static void Mouse(int button, int state, int x, int y);
	static void Motion(int x, int y);
	static void Resize(GLint newWidth, GLint newHeight);

private:
	// Get private singleton
	static DrawContext& get_instance();
	
	// Don't instantiate anywhere but from get_instance
	DrawContext();
	DrawContext(DrawContext const&);
	void operator=(DrawContext const&);

	// Private callbacks called by public callbacks.
	void on_display(void);
	void on_keyboard(unsigned char key, int x, int y);
	void on_mouse(int button, int state, int x, int y);	
	void on_motion(int x, int y);
	void on_resize(GLint newWidth, GLint newHeight);

	// Write text to screen
	void write(ostringstream &os, GLfloat x, GLfloat y, float *color);

	// Draw menu and mouse position
	void draw_interface(void);

	// Is x, y in window?
	bool in_window(int x, int y);

	// Truncated distance from start to end
	GLint int_distance(Point2D &start, Point2D &end);

	// Draw everything
	void draw_shapes(void);

	// Draw every vertex buffer in a vector
	void draw_buffers(vector<VertexBuffer*> &buffers);

	// Pop shape data from vector
	void undo(void);

	// Delete vertex buffers from vector
	void delete_buffers(vector<VertexBuffer*> &buffers);

	// Rubber band UI
	void point_start(GLint button, GLint x, GLint y);
	void point_finish(GLint button, GLint x, GLint y);

	// State variables
	bool draw_menu;					// Should we draw the help menu?
	bool draw_control_points;		// Should we draw the control points?
	bool drawing_curve;				// Are we drawing a curve right now?
	bool pressing;					// Is the mouse button down?
	State draw_state;				// Which draw state are we in?

	// Current end points for line/circle/clock
	Point2D start;
	Point2D mouse;

	// Current control points for curve
	vector<Point2D> control_points;

	// Saved data
	vector<VertexBuffer*> line_data;
	vector<VertexBuffer*> curve_data;
	vector<VertexBuffer*> circle_data;
	vector<VertexBuffer*> control_data;
	vector<VertexBuffer*> clock_data;

	// Clocks need extra data =(
	vector<Point2D> clock_centers;
	vector<GLint> clock_radii;
};

#endif
