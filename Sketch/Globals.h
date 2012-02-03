#ifndef GLOBALS_H
#define GLOBALS_H

#include <Windows.h>
#include <GL\gl.h>
#include <glut.h>
#include <cmath>
#include <ctime>

// Global constants
const GLint WIDTH = 800;
const GLint HEIGHT = 600;
const GLfloat PI = 3.14159265;
const GLfloat PI_OVER_180 = PI / 180.0;
const GLfloat PI_OVER_2 = PI / 2.0;

// Menu header
static const char *TOGGLE[] = {
	"+---------------------+",
	"| H - Toggle Help     |",
	"+---------------------+"
};
const GLint TOGGLE_SIZE = sizeof(TOGGLE) / sizeof(char*);

// Dropdown menu
static const char *MENU[] = {
	"| L - Line            |",
	"| O - Circle          |",
	"| S - Curve           |",
	"| C - Clock           |",
	"| U - Undo            |",
	"| X - Clear           |",
	"| P - Control Points  |",
	"| Q - Quit            |",
	"+---------------------+"
};
const GLint MENU_SIZE = sizeof(MENU) / sizeof(char*);

// Useful colors
const GLfloat BLUE[] = {0.0f, 0.4f, 1.0f};
const GLfloat GREEN[] = {0.0f, 1.0f, 0.4};
const GLfloat RED[] = {1.0f, 0.0f, 0.0f};
const GLfloat WHITE[] = {1.0f, 1.0f, 1.0f};

// Represents a point in 2D
struct Point2D{
	GLint x;
	GLint y;
	
	// Constructors
	Point2D(): x(0), y(0) {}
	Point2D(GLint xc, GLint yc): x(xc), y(yc) {}
};

// We have to recalculate time every frame, so we calculate everything
// that's not dependent on a specific circle's radius here once only.
struct TimeAngle{
	GLfloat hour_cos;
	GLfloat hour_sin;
	GLfloat min_cos;
	GLfloat min_sin;
	GLfloat sec_cos;
	GLfloat sec_sin;

	TimeAngle(tm *now){
		// Convert hour/min/sec angles to radians
		GLfloat hourRad = (now->tm_hour % 12) / 12.0 * 360 * PI_OVER_180 - PI_OVER_2;
		GLfloat minRad = now->tm_min / 60.0 * 360 * PI_OVER_180 - PI_OVER_2;
		GLfloat secRad = now->tm_sec / 60.0 * 360 * PI_OVER_180 - PI_OVER_2;

		// Calculate endpoint of hand (without radius)
		hour_cos = 0.50 * cos(hourRad);
		hour_sin = 0.50 * sin(hourRad);
		min_cos = 0.75 * cos(minRad);
		min_sin = 0.75 * sin(minRad);
		sec_cos = 0.90 * cos(secRad);
		sec_sin = 0.90 * sin(secRad);
	}
};

// Menu and mouse coordinates position
const Point2D MOUSE_POS(12, 24);
const Point2D MENU_POS(12, HEIGHT - 24);

#endif
