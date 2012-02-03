
#include "Globals.h"
#include "DrawContext.h"
#include "VertexBuffer.h"

int main(int argc, char **argv){
   // Initialization functions
   glutInit(&argc, argv);                         
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);  
   glutInitWindowSize(WIDTH, HEIGHT);     
   glutInitWindowPosition (WIDTH/2, HEIGHT/2);
   glutCreateWindow("Sketch");
   
   // Call-back functions
   glutDisplayFunc(DrawContext::Display);
   glutKeyboardFunc(DrawContext::Keyboard);
   glutMouseFunc(DrawContext::Mouse);
   glutPassiveMotionFunc(DrawContext::Motion);
   glutMotionFunc(DrawContext::Motion);
   glutReshapeFunc(DrawContext::Resize);
   
   // Infinite Loop
   glutMainLoop();           
   return 0;
}
// End of the program
