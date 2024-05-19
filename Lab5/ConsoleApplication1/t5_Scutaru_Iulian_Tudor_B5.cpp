#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <glut.h>

#define WINDOW_DIMENSION 300
#define M_PI 3.14159265358979323846

typedef enum {
    WIREFRAME_CUBE,
    SOLID_CUBE,
    WIREFRAME_SPHERE,
    SOLID_SPHERE,
    WIREFRAME_TRIANGLE,
    SOLID_TRIANGLE
} ObjectType;

typedef struct {
    int type; // 0 = rotation, 1 = translation, 2 = scaling
    double angle; 
    double scaleX, scaleY, scaleZ; // Scale factor
    double translateX, translateY, translateZ; // distanta de translatie
    int axis; // 0 = X, 1 = Y, 2 = Z
} TransformationState;

TransformationState transformationState = { 0, 0, 1, 1, 1, 0, 0, 0, 0 }; // Stare fara transformari

GLfloat triangleVertices[3][3] = {
    {0.0f, 0.0f, 0.0f}, // Vertex P1
    {2.0f, 2.0f, 2.0f}, // Vertex P2
    {0.0f, 2.0f, 2.0f} // Vertex P3
};

ObjectType currentObject = WIREFRAME_CUBE;

void applyTransformation(int type, double angle, double x, double y, double z) {
    glMatrixMode(GL_MODELVIEW);
    switch (type) {
    case 0: // Rotation
        glRotated(angle, x, y, z);
        break;
    case 1: // Translation
        glTranslated(x, y, z);
        break;
    case 2: // Scaling
        glScalef(x, y, z);
        break;
    }
}

void drawMainDiagonal() {
    glColor3f(0.0f, 0.0f, 0.0f); 
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f); // Start at (0,0,0)
    glVertex3f(1.0f, 1.0f, 1.0f); // End at (1,1,1)
    glEnd();
}


void displayAxes(float angleX, float angleY, float angleZ) {
    int axisLength = 200;
    glLineWidth(2);

    
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);
    glRotatef(angleZ, 0.0f, 0.0f, 1.0f);

    // X-axis (Green)
    glColor3f(0, 1, 0);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0, 0, 0);
    glVertex3f(axisLength, 0, 0);
    glEnd();

    // Y-axis (Blue)
    glColor3f(0, 0, 1);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0, 0, 0);
    glVertex3f(0, axisLength, 0);
    glEnd();

    // Z-axis (Red)
    glColor3f(1, 0, 0);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, axisLength);
    glEnd();

    glLineWidth(1);
}



void displayCube(int type) {
    GLfloat vertices[8][3] = {
        {0.0f, 0.0f, 0.0f}, // Vertex 0
        {1.0f, 0.0f, 0.0f}, // Vertex 1
        {1.0f, 1.0f, 0.0f}, // Vertex 2
        {0.0f, 1.0f, 0.0f}, // Vertex 3
        {0.0f, 0.0f, 1.0f}, // Vertex 4
        {1.0f, 0.0f, 1.0f}, // Vertex 5
        {1.0f, 1.0f, 1.0f}, // Vertex 6
        {0.0f, 1.0f, 1.0f}  // Vertex 7
    };

    int quads[6][4] = {
        {0, 1, 2, 3}, // Bottom face
        {4, 5, 6, 7}, // Top face
        {0, 3, 7, 4}, // Left face
        {1, 2, 6, 5}, // Right face
        {0, 1, 5, 4}, // Front face
        {3, 2, 6, 7}  // Back face
    };

    
    glColor3f(1, 0, 0);
    

   
    for (int i = 0; i < 6; i++) {
        glBegin(type == WIREFRAME_CUBE ? GL_LINE_LOOP : GL_QUADS);
        for (int j = 0; j < 4; j++) {
            glVertex3fv(vertices[quads[i][j]]);
        }
        glEnd();
    }

    drawMainDiagonal();
    glPopMatrix(); 
}



void displaySphere(int type) {
    glColor3f(0, 0, 1);
    if (type == WIREFRAME_SPHERE) {
        glutWireSphere(1, 10, 10);
    }
    else if (type == SOLID_SPHERE) {
        glutSolidSphere(1, 10, 10);
    }
}

void displayTriangle(int type) {
    glColor3f(0.5f, 1.0f, 0.0f);
    glBegin(type == WIREFRAME_TRIANGLE ? GL_LINE_LOOP : GL_TRIANGLES);
    for (int i = 0; i < 3; i++) {
        glVertex3fv(triangleVertices[i]);
    }
    glEnd();
}

void rotateAroundDiagonal() {
    if (currentObject == WIREFRAME_CUBE || currentObject == SOLID_CUBE) {
        displayAxes(0, 0, 0);

        drawMainDiagonal();

        // aliniem cu axa Z si punem centrul diagonalei la origine
        glTranslated(0.5, 0.5, 0.5);

        // ducem diagonala in planul y=x prin rotatie pe z
        glRotated(-45, 0, 0, 1);

        // rotim in pe y pentru aliniere cu z
        glRotated(-acos(1.0 / sqrt(3)) * 180.0 / M_PI, 0, 1, 0);

        // Facem rotirea dorita in "jurul diagonalei" (tot z)
        glRotated(transformationState.angle, 0, 0, 1);

        // Undo la roatiile pt aliniere
        glRotated(acos(1.0 / sqrt(3)) * 180.0 / M_PI, 0, -1, 0);
        glRotated(45, 0, 0, -1);

        // mutam inapoi cubul.
        glTranslated(-0.5, -0.5, -0.5);


        displayCube(currentObject);
    }
}


void displayObject() {
    switch (transformationState.type) {
    case 0: 
        if (currentObject == WIREFRAME_CUBE || currentObject == SOLID_CUBE) {
            displayAxes(0, 0, 0);
            //drawMainDiagonal();
            switch (transformationState.axis) {
            case 0: // X-axis rotation
                glRotated(transformationState.angle, 1, 0, 0);
                break;
            case 1: // Y-axis rotation
                glRotated(transformationState.angle, 0, 1, 0);
                break;
            case 2: // Z-axis rotation
                glRotated(transformationState.angle, 0, 0, 1);
                break;
            }
        }
        else if (currentObject == WIREFRAME_TRIANGLE or currentObject == SOLID_TRIANGLE) {
            displayAxes(0, 0, 0);
            // Punem un punct la origine
            glTranslatef(-triangleVertices[0][0], -triangleVertices[0][1], -triangleVertices[0][2]);

            // rotim in jur de Z
            glRotatef(transformationState.angle, 0.0f, 0.0f, 1.0f);

            // mutam inapoit la pozitia initiala
            glTranslatef(triangleVertices[0][0], triangleVertices[0][1], triangleVertices[0][2]);
        }
        else {
            switch (transformationState.axis) {
            case 0: // X
                glRotated(transformationState.angle, 1, 0, 0);
                displayAxes(transformationState.angle, 0, 0);
                break;
            case 1: // Y
                glRotated(transformationState.angle, 0, 1, 0);
                displayAxes(0, transformationState.angle, 0);
                break;
            case 2: // Z
                glRotated(transformationState.angle, 0, 0, 1);
                displayAxes(0, 0, transformationState.angle);
                break;
            }
        }
        break;
    case 1: 
        glTranslated(transformationState.translateX, transformationState.translateY, transformationState.translateZ);
        displayAxes(0, 0, 0);
        break;
    case 2: 
        glScalef(transformationState.scaleX, transformationState.scaleY, transformationState.scaleZ);
        displayAxes(0, 0, 0);
        break;
    }

    
    switch (currentObject) {
    case WIREFRAME_CUBE:
        displayCube(currentObject);
        break;
    case SOLID_CUBE:
        drawMainDiagonal();
        displayCube(currentObject);
        break;
    case WIREFRAME_SPHERE:
        displaySphere(currentObject);
        break;
    case SOLID_SPHERE:
        displaySphere(currentObject);
        break;
    case WIREFRAME_TRIANGLE:
        displayTriangle(currentObject);
        break;
    case SOLID_TRIANGLE:
        displayTriangle(currentObject);
        break;
    }
}




void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotated(20, 1, 0, 0);
    glRotated(-20, 0, 1, 0);
    displayObject();
    //rotateAroundDiagonal();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void keyboardFunc(unsigned char key, int x, int y) {
    static int step = 0;
    switch (key) {
    case 27: 
        exit(0);
        break;
    case '1':
        currentObject = WIREFRAME_CUBE;
        break;
    case '2':
        currentObject = SOLID_CUBE;
        break;
    case '3':
        currentObject = WIREFRAME_SPHERE;
        break;
    case '4':
        currentObject = SOLID_SPHERE;
        break;
    case '5':
        currentObject = WIREFRAME_TRIANGLE;
        break;
    case '6':
        currentObject = SOLID_TRIANGLE;
        break;
    case 'x': 
        if (step == 0) {
            transformationState.type = 0; 
            transformationState.axis = 0; // X-axis
            transformationState.angle = 30; 
            step++;
        }
        break;
    case 'X': 
		transformationState.type = 0;
		transformationState.axis = 0;
		transformationState.angle -= 10;
		break;
    case 'y': 
        if (step == 1) {
            transformationState.type = 0;
            transformationState.axis = 1; // Y-axis
            transformationState.angle = 30; 
            step++;
        }
        break;
    case 'Y':
        transformationState.type = 0;
		transformationState.axis = 1;
		transformationState.angle -= 10;
		break;
    case 'z': 
        if (step == 2) {
            transformationState.type = 0;
            transformationState.axis = 2; // Z-axis
            transformationState.angle = 30; 
            step = 0;
        }
        break;
    case 'h':
        transformationState.type = 0;
        transformationState.angle += 10;
        rotateAroundDiagonal();
        break;
    case 'Z':
        transformationState.type = 0;
        transformationState.axis = 2;
        transformationState.angle -= 10;
        break;
    case 'S': 
		transformationState.type = 2;
		transformationState.scaleX += 0.1;
		transformationState.scaleY += 0.1;
		transformationState.scaleZ += 0.1;
		break;
    case 's':
        transformationState.type = 2;
		transformationState.scaleX -= 0.1;
		transformationState.scaleY -= 0.1;
		transformationState.scaleZ -= 0.1;
		break;
    case 'n': 
		transformationState.type = 1;
		transformationState.translateX -= 0.1;
		break;
    case 'm':
        transformationState.type = 1;
        transformationState.translateX += 0.1;
        break;
    case 'k':
        transformationState.type = 1;
		transformationState.translateY += 0.1;
		break;
    case 'l':
		transformationState.type = 1;
        transformationState.translateY -= 0.1;
        break;
    case 'o':
		transformationState.type = 1;
		transformationState.translateZ += 0.1;
		break;
    case 'p':
        transformationState.type = 1;
        transformationState.translateZ -= 0.1;
        break;
    case 'c':
		transformationState.angle = 0;
		transformationState.scaleX = 1;
		transformationState.scaleY = 1;
		transformationState.scaleZ = 1;
		transformationState.translateX = 0;
		transformationState.translateY = 0;
		transformationState.translateZ = 0;
		break;
    case 'r': // pt ex2
        transformationState.type = 0;
        transformationState.axis = 2; 
        transformationState.angle += 45.0; 
        break;
    }

    glutPostRedisplay(); 
}


void mouseFunc(int button, int state, int x, int y) {
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

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(WINDOW_DIMENSION, WINDOW_DIMENSION);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow(argv[0]);
    Init();
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardFunc);
    glutMouseFunc(mouseFunc);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
