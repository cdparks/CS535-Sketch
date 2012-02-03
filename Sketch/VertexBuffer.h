#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <vector>

using namespace std;

#include "Globals.h"

struct VertexBuffer{
	VertexBuffer(vector<Point2D> &points){
		size = 2 * (points.size() + 1);
		vertices = new float[size];
		for(size_t v = 0, i = 0; i < points.size(); v += 2, i += 1){
			vertices[v] = points[i].x;
			vertices[v + 1] = points[i].y;
		};
	};

	~VertexBuffer(){
		delete[] vertices;
	};

	void draw(void){
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, vertices);
		glDrawArrays(GL_POINTS, 0, size/2);
		glDisableClientState(GL_VERTEX_ARRAY);
	};

	GLfloat *vertices;
	GLint size;
};

#endif
