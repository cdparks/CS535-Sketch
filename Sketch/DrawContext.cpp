#ifndef DRAW_FUNCTIONS_H
#define DRAW_FUNCTIONS_H

#include <sstream>
#include <ctime>

using namespace std;

#include "DrawContext.h"
#include "Algorithms.h"

// Initialize single instance once. Allow static access
DrawContext& DrawContext::get_instance(){
	static DrawContext dc;
	return dc;
}

// Initialize window and logical state
DrawContext::DrawContext():
	draw_menu(false),
	draw_control_points(false),
	drawing_curve(false),
	pressing(false),
	draw_state(LINE),
	start(0, 0),
	mouse(0, 0),
	control_points(),
	line_data(),
	curve_data(),
	circle_data(),
	control_data(),
	clock_data(),
	clock_centers(),
	clock_radii()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// Set background color to be black
	glColor3fv(WHITE);						// Set the drawing color to be white
	glMatrixMode(GL_PROJECTION);			// Set "camera shape"
	glLoadIdentity();						// Clearing the viewing matrix
	gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);	// Setting the world window
}

// Write contents of a stream to screen at (x, y)
void DrawContext::write(ostringstream &os, GLfloat x, GLfloat y, float *color){	
	string output = os.str();
	glColor3fv(color);
	glRasterPos2f(x, y);
	for(GLint i = 0; i < (GLint)output.length(); ++i){
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, output[i]);
	}
}

// Draw the menu, state, and mouse position
void DrawContext::draw_interface(void){
	Point2D cursor(MENU_POS);
	// "Help" toggle
	for(size_t i = 0; i < TOGGLE_SIZE; ++i, cursor.y -= 13){
		ostringstream toggle_stream;
		toggle_stream << TOGGLE[i];
		write(toggle_stream, cursor.x, cursor.y, (float*)GREEN);
	}
	// Help dropdown menu
	if(draw_menu){
		for(size_t i = 0; i < MENU_SIZE; ++i, cursor.y -= 13){
			ostringstream stream;
			stream << MENU[i];
			write(stream, cursor.x, cursor.y, (float*)GREEN);
		}
	}
	ostringstream state_stream;
	switch(draw_state){
	case LINE:
		state_stream << "[L";
		break;
	case CIRCLE:
		state_stream << "[O";
		break;
	case CURVE:
		state_stream << "[S";
		break;
	case CLOCK:
		state_stream << "[C";
		break;
	default:
		state_stream << "[?";
		break;
	}
	if(draw_control_points){
		state_stream << "P]";
	}else{
		state_stream << "]";
	}
	state_stream << " at (" << mouse.x << ", " << mouse.y << ")";
	write(state_stream, MOUSE_POS.x, MOUSE_POS.y, (float*)GREEN);
}

// Is (x, y) within the window?
bool DrawContext::in_window(int x, int y){
	return x > 0 && x < WIDTH && y > 0 && y < HEIGHT;
}

// Truncated distance between start and end
GLint DrawContext::int_distance(Point2D &start, Point2D &end){
	GLint dx = end.x - start.x;
	GLint dy = end.y - start.y;
	return (GLint)sqrt((double)(dx * dx + dy * dy));
}

// Draw saved lines, curves, circles, and shapes. Also, build
// and draw temporary shapes (rubber-band lines and circles) and
// clock hands.
void DrawContext::draw_shapes(){
	time_t t = time(NULL);
	TimeAngle tr(localtime(&t));
	vector<Point2D> pixels;
	glColor3fv(WHITE);
	// Add rubber-band UI to local pixel vector
	if(pressing){
		GLint radius;
		switch(draw_state){
		case LINE:
			make_line(start, mouse, pixels);
			break;
		case CIRCLE:
			make_circle(start, int_distance(start, mouse), pixels);
			break;
		case CLOCK:
			radius = int_distance(start, mouse);
			make_circle(start, radius, pixels);
			make_hands(start, radius, pixels, tr);
			break;
		default:
			break;
		}
	}
	// Draw control points and lines while building a curve
	if(drawing_curve){
		if(control_points.size()){
			for(size_t i = 1; i < control_points.size(); ++i){
				make_line(control_points[i - 1], control_points[i], pixels);
			}
			make_line(control_points.back(), mouse, pixels);
		}
	}
	// Draw all saved pixel data
	draw_buffers(line_data);
	draw_buffers(curve_data);
	draw_buffers(circle_data);
	draw_buffers(clock_data);
	for(size_t i = 0; i < clock_data.size(); ++i){
		make_hands(clock_centers[i], clock_radii[i], pixels, tr);
	}
	// Draw saved control points
	if(draw_control_points){
		glColor3fv(GREEN);
		draw_buffers(control_data);
		glColor3fv(WHITE);
	}
	// Draw all temporary pixels
	VertexBuffer(pixels).draw();
	pixels.clear();
	glutPostRedisplay();
}

// Draw every vertex buffer in a vector
void DrawContext::draw_buffers(vector<VertexBuffer*> &buffers){
	for(size_t i = 0; i < buffers.size(); ++i){
		buffers[i]->draw();
	}
}

// All saved pixel data is saved in a shape-specific stack.
// Each mode supports full undo by popping the most recent
// addition from the shape's stack.
void DrawContext::undo(){
	switch(draw_state){
	case LINE:
		if(line_data.size()){
			delete line_data.back();
			line_data.pop_back();
		}
		break;
	case CURVE:
		if(curve_data.size() && !drawing_curve){
			delete curve_data.back();
			curve_data.pop_back();
			delete control_data.back();
			control_data.pop_back();
		}
		break;
	case CIRCLE:
		if(circle_data.size()){
			delete circle_data.back();
			circle_data.pop_back();
		}
		break;
	case CLOCK:
		if(clock_data.size()){
			delete clock_data.back();
			clock_data.pop_back();
			clock_centers.pop_back();
			clock_radii.pop_back();
		}
		break;
	default:
		break;
	}
}

// Delete vertex buffers from vector
void DrawContext::delete_buffers(vector<VertexBuffer*> &buffers){
	for(size_t i = 0; i < buffers.size(); ++i){
		delete buffers[i];
	}
	buffers.clear();
}

// Lines, Circles, and Clocks work like so:
// MOUSE DOWN -> start Point
// DRAG
// MOUSE UP -> end Point
// point_start
void DrawContext::point_start(GLint button, GLint x, GLint y){
	if(button == GLUT_LEFT_BUTTON){
		switch(draw_state){
		case LINE:
		case CIRCLE:
		case CLOCK:
			if(in_window(x, y)){
				start.x = x;
				start.y = y;
			}
			break;
		case CURVE:
			if(in_window(x, y)){
				drawing_curve = true;
				control_points.push_back(Point2D(x, y));
			}
			break;
		default:
			break;
		}
	}
}

void DrawContext::point_finish(GLint button, GLint x, GLint y){
	if(button == GLUT_LEFT_BUTTON){
		GLint radius;
		Point2D end(x, y);
		vector<Point2D> pixels;
		switch(draw_state){
		case LINE:
			make_line(start, end, pixels);
			line_data.push_back(new VertexBuffer(pixels));
			break;
		case CIRCLE:
			make_circle(start, int_distance(start, end), pixels);
			circle_data.push_back(new VertexBuffer(pixels));
			break;
		case CLOCK:
			radius = int_distance(start, end);
			make_circle(start, radius, pixels);
			clock_data.push_back(new VertexBuffer(pixels));
			clock_centers.push_back(start);
			clock_radii.push_back(radius);
			break;
		case CURVE:
			if(control_points.size() == 4){
				drawing_curve = false;
				make_curve(control_points, pixels);
				curve_data.push_back(new VertexBuffer(pixels));
				pixels.clear();
				for(int i = 0; i < 3; ++i){
					make_line(control_points[i], control_points[i + 1], pixels);
				}
				control_data.push_back(new VertexBuffer(pixels));
				control_points.clear();
			}
			break;
		default:
			break;
		}
		pixels.clear();
	}
}

void DrawContext::on_display(){
	glClear(GL_COLOR_BUFFER_BIT); // clearing the buffer
	draw_interface();
	draw_shapes();
	glutSwapBuffers(); // display the buffer
}

void DrawContext::on_keyboard(unsigned char key, int x, int y){
	State old_state = draw_state;
	switch (key){
	case 'h':
	case 'H':
		draw_menu = !draw_menu;
		break;
	case 'p':
	case 'P':
		draw_control_points = !draw_control_points;
		break;
	case 'x':
	case 'X':
		delete_buffers(line_data);
		delete_buffers(circle_data);
		delete_buffers(curve_data);
		delete_buffers(control_data);
		delete_buffers(clock_data);
		clock_centers.clear();
		clock_radii.clear();
		break;
	case 'u':
	case 'U':
		undo();
		break;
	case 'l':
	case 'L':
		draw_state = LINE;
		break;
	case 'o':
	case 'O':
		draw_state = CIRCLE;
		break;
	case 's':
	case 'S':
		draw_state = CURVE;
		break;
	case 'c':
	case 'C':
		draw_state = CLOCK;
		break;
	case 'q':
	case 'Q':
	case 27:  // Typed the Escape key, so exit.
		exit(0);
		break;
	default:
		break;
	}
	if(old_state == CURVE && old_state != draw_state){
		control_points.clear();
	}
	glutPostRedisplay();
}

void DrawContext::on_mouse(int button, int state, int x, int y){
	if(button == GLUT_LEFT){
		if(state == GLUT_DOWN){
			point_start(button, x, y);
			pressing = true;
		}else{
			point_finish(button, x, y);
			pressing = false;
		}
	}
	glutPostRedisplay();
}

void DrawContext::on_motion(int x, int y){
	mouse.x = x;
	mouse.y = y;
	glutPostRedisplay();
}

void DrawContext::on_resize(GLint newWidth, GLint newHeight){
	glutReshapeWindow(WIDTH, HEIGHT);
}

void DrawContext::Display(void){
	get_instance().on_display();
};

void DrawContext::Keyboard(unsigned char key, int x, int y){
	get_instance().on_keyboard(key, x, y); 
};

void DrawContext::Mouse(int button, int state, int x, int y){
	get_instance().on_mouse(button, state, x, y);
};

void DrawContext::Motion(int x, int y){
	get_instance().on_motion(x, y);
};

void DrawContext::Resize(GLint newWidth, GLint newHeight){
	get_instance().on_resize(newWidth, newHeight);
};

#endif
