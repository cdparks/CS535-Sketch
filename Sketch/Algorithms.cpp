#include <vector>

using namespace std;

#include "Globals.h"
#include "Algorithms.h"

// Write pixel to pixel vector
void set_pixel(int x, int y, vector<Point2D> &pixels){
	pixels.push_back(Point2D(x, HEIGHT - y));
}

// Write pixel to pixel vector swapping x and y
void swap_set_pixel(int x, int y, vector<Point2D> &pixels){
	pixels.push_back(Point2D(y, HEIGHT - x));
}

// Midpoint line drawing algorithm. Largely from textbook plus modifications
// for drawing in all octants.
void make_line(Point2D p0, Point2D p1, vector<Point2D> &pixels){
	GLint dx = abs(p1.x - p0.x);
	GLint dy = abs(p1.y - p0.y);
	
	// Avoid branching in the loop
	void(*draw_pixel)(GLint x, GLint y, vector<Point2D> &pixels) = set_pixel; 
	
	// If slope is steeper than 1 or -1 ...
	if(dy > dx){
		// ... then we'll increment from y0 to y1 instead
		// of x0 to x1
		swap(dx, dy);
		swap(p0.x, p0.y);
		swap(p1.x, p1.y);
		draw_pixel = swap_set_pixel;
	}
	// Go from "small" x to "large" x
	if(p0.x > p1.x){
		swap(p0.x, p1.x);
		swap(p0.y, p1.y);
	}

	// Might be sloping up or down
	GLint step_y = p0.y > p1.y ? -1 : 1;

	// Initial x and y
	GLint x = p0.x, y = p0.y;

	GLint d = 2 * dy - dx;
	GLint dE = 2 * dy;
	GLint dNE = 2 * (dy - dx);

	draw_pixel(x, y, pixels);
	while(x < p1.x){
		/*if(d <= 0){
			d += dE;
		}else{
			d += dNE;
			y += step_y;
		}*/

		// Branchless version
		x += 1;
		bool east = d <= 0;
		bool northeast = !east;
		// Go east or northeast
		d += dE * east + dNE * northeast;
		y += step_y * northeast;

		draw_pixel(x, y, pixels);
	}
}

// Exploit radial symmetry for circle drawing
void circle_points(GLint cx, GLint cy, GLint x, GLint y, vector<Point2D> &pixels){
	set_pixel(cx + x, cy + y, pixels);
	set_pixel(cx - x, cy + y, pixels);
	set_pixel(cx + x, cy - y, pixels);
	set_pixel(cx - x, cy - y, pixels);
	if(x != y){
		set_pixel(cx + y, cy + x, pixels);
		set_pixel(cx - y, cy + x, pixels);
		set_pixel(cx + y, cy - x, pixels);
		set_pixel(cx - y, cy - x, pixels);
	}
}

// Midpoint circle algorithm. Pretty much verbatim from textbook
void make_circle(Point2D center, GLint radius, vector<Point2D> &pixels){
	GLint x = 0;
	GLint y = radius;
	GLint d = 1 - radius;
	GLint dE = 3;
	GLint dSE = -2 * radius + 5;
	circle_points(center.x, center.y, x, y, pixels);
	while(y > x){
		/*
		if(d < 0){
			d += dE;
			dE += 2;
			dSE += 2;
		}else{
			d += dSE;
			dE += 2;
			dSE += 4;
			y -= 1;
		}*/

		// Branchless version
		x += 1;
		bool east = d < 0;
		bool southeast = !east;
		d += dE * east + dSE * southeast;
		dE += 2;
		dSE += 2 * east + 4 * southeast;
		y -= 1 * southeast;

		circle_points(center.x, center.y, x, y, pixels);
	}
}

// Clock hands need to be updated each frame. The TimeAngle is calculated once per frame to
// get the normalized endpoints for each hand. Just mix the radius in and create the lines.
void make_hands(Point2D center, GLint radius, vector<Point2D> &pixels, TimeAngle &ta){
	 Point2D hourHand(center.x + radius * ta.hour_cos, center.y + radius * ta.hour_sin);
	 Point2D minHand(center.x + radius * ta.min_cos, center.y + radius * ta.min_sin);
	 Point2D secHand(center.x + radius * ta.sec_cos, center.y + radius * ta.sec_sin);
	 make_line(center, hourHand, pixels);
	 make_line(center, minHand, pixels);
	 make_line(center, secHand, pixels);
}

// Calculate lines for bezier curve and add to pixel vector
// This is in the book, but I used this as well:
// http://www.cs.helsinki.fi/group/goa/mallinnus/curves/curves.html
void make_curve(vector<Point2D> &control_points, vector<Point2D> &pixels){
	Point2D begin = control_points[0];
	Point2D end;
	for(float t = 0.025; t <= 1.0; t += 0.025){
		float _t = 1 - t;
		float f1t = _t * _t * _t;
		float f2t = 3.0 * t * _t * _t;
		float f3t = 3.0 * t * t * _t;
		float f4t = t * t * t;
		end.x = 
			control_points[0].x * f1t +
			control_points[1].x * f2t +
			control_points[2].x * f3t +
			control_points[3].x * f4t;
		end.y = 
			control_points[0].y * f1t +
			control_points[1].y * f2t +
			control_points[2].y * f3t +
			control_points[3].y * f4t;
		make_line(begin, end, pixels);
		begin = end;
	}
}
