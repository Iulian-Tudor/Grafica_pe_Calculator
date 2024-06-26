#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "glut.h"

// the size of the window measured in pixels
#define dim 300

unsigned char prevKey;

enum EObiect { cubw, cubs, sferaw, sferas } ob = cubw;

void DisplayAxe() {
    int X, Y, Z;
    X = Y = 200;
    Z = 200;

    glLineWidth(2);

    // Ox axis - green
    glColor3f(0, 1, 0);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0, 0, 0);
    glVertex3f(X, 0, 0);
    glEnd();

    // Oy axis - blue
    glColor3f(0, 0, 1);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0, 0, 0);
    glVertex3f(0, Y, 0);
    glEnd();

    // Oz axis - red
    glColor3f(1, 0, 0);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, Z);
    glEnd();

    glLineWidth(1);
}

// wireframe cube
void Display1() {
    glColor3f(1, 0, 0);
    glutWireCube(1);
}

// solid cube
void Display2() {
    glColor3f(1, 0, 0);
    glutSolidCube(1);
}

// wireframe sphere
void Display3() {
    glColor3f(0, 0, 1);
    glutWireSphere(1, 10, 10);
}

// solid sphere
void Display4() {
    glColor3f(0, 0, 1);
    glutSolidSphere(1, 10, 10);
}

void DisplayObiect()
{
    switch (ob)
    {
    case cubw:
        Display1();
        break;
    case cubs:
        Display2();
        break;
    case sferaw:
        Display3();
        break;
    case sferas:
        Display4();
        break;
    default:
        break;
    }
}

// the rotation with 10 degrees about the Ox axis
void DisplayX() {
    glMatrixMode(GL_MODELVIEW);
    glRotated(10, 1, 0, 0);
}

// the rotation with 10 degrees about the Oy axis
void DisplayY() {
    glMatrixMode(GL_MODELVIEW);
    glRotated(10, 0, 1, 0);
}

// the rotation with 10 degrees about the Oz axis
void DisplayZ() {
    glMatrixMode(GL_MODELVIEW);
    glRotated(10, 0, 0, 1);
}

// translation by 0.2, 0.2, 0.2
void DisplayT() {
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0.2, 0.2, 0.2);
}

// scaling by 1.2, 1.2, 1.2
void DisplayS() {
    glMatrixMode(GL_MODELVIEW);
    glScalef(1.2, 1.2, 1.2);
}

void Init(void) {
    glClearColor(1, 1, 1, 1);
    glLineWidth(2);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10, 10, -10, 10, 30, -30);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotated(20, 1, 0, 0);
    glRotated(-20, 0, 1, 0);
}

void Display(void) {
    switch (prevKey)
    {
    case 'a':
        DisplayAxe();
        break;
    case '0':
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotated(20, 1, 0, 0);
        glRotated(-20, 0, 1, 0);
        break;
    case '1':
        Display1();
        ob = cubw;
        break;
    case '2':
        Display2();
        ob = cubs;
        break;
    case '3':
        Display3();
        ob = sferaw;
        break;
    case '4':
        Display4();
        ob = sferas;
        break;
    case 'x':
        glClear(GL_COLOR_BUFFER_BIT);
        DisplayX();
        DisplayAxe();
        DisplayObiect();
        break;
    case 'y':
        glClear(GL_COLOR_BUFFER_BIT);
        DisplayY();
        DisplayAxe();
        DisplayObiect();
        break;
    case 'z':
        glClear(GL_COLOR_BUFFER_BIT);
        DisplayZ();
        DisplayAxe();
        DisplayObiect();
        break;
    case 't':
        glClear(GL_COLOR_BUFFER_BIT);
        DisplayT();
        DisplayAxe();
        DisplayObiect();
        break;
    case 's':
        glClear(GL_COLOR_BUFFER_BIT);
        DisplayS();
        DisplayAxe();
        DisplayObiect();
        break;
    default:
        break;
    }
    glutSwapBuffers();
}

void Reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void KeyboardFunc(unsigned char key, int x, int y) {
    prevKey = key;
    if (key == 27)
        exit(0);
    glutPostRedisplay();
}

void MouseFunc(int button, int state, int x, int y) {
}

int main(int argc, char** argv) {

    glutInit(&argc, argv);

    glutInitWindowSize(dim, dim);

    glutInitWindowPosition(100, 100);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutCreateWindow(argv[0]);

    Init();

    glutReshapeFunc(Reshape);

    glutKeyboardFunc(KeyboardFunc);

    glutMouseFunc(MouseFunc);

    glutDisplayFunc(Display);

    glutMainLoop();

    return 0;
}
