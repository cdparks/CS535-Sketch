#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <vector>

using namespace std;

#include "Globals.h"

// Write single pixel vector
void set_pixel(int x, int y, vector<Point2D> &pixels);
void swap_set_pixel(int x, int y, vector<Point2D> &pixels);
	
// Write line pixels to vector
void make_line(Point2D p0, Point2D p1, vector<Point2D> &pixels);

// Write circle pixels to vector
void circle_points(GLint cx, GLint cy, GLint x, GLint y, vector<Point2D> &pixels);
void make_circle(Point2D center, GLint radius, vector<Point2D> &pixels);

// Write curve pixels to vector
void make_curve(vector<Point2D> &control_points, vector<Point2D> &pixels);

// Write clock hand pixels to vector
void make_hands(Point2D center, GLint radius, vector<Point2D> &pixels, TimeAngle &ta);

#endif ALGORITHMS_H