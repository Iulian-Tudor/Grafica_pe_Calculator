#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <float.h>
#include "glut.h"


// dimensiunea ferestrei in pixeli
#define dim 300

// numarul maxim de iteratii pentru testarea apartenentei la multimea Mandelbrot
#define nr_iteratii_Mandelbrot 500
// modulul maxim pentru testarea apartenentei la multimea Mandelbrot
#define modul_maxim_Mandelbrot 2
// ratiile pentru multimea Mandelbrot
#define ratie_x 0.01
#define ratie_y 0.01


unsigned char prevKey;
int nivel = 0;


class C2coord
{
public:
    C2coord()
    {
        m.x = m.y = 0;
    }

    C2coord(double x, double y)
    {
        m.x = x;
        m.y = y;
    }

    C2coord(C2coord& p)
    {
        m.x = p.m.x;
        m.y = p.m.y;
    }

    C2coord& operator=(C2coord& p)
    {
        m.x = p.m.x;
        m.y = p.m.y;
        return *this;
    }

    int operator==(C2coord& p)
    {
        return ((m.x == p.m.x) && (m.y == p.m.y));
    }

protected:
    struct SDate
    {
        double x, y;
    } m;
};

class CPunct : public C2coord
{
public:
    CPunct() : C2coord(0.0, 0.0)
    {}

    CPunct(double x, double y) : C2coord(x, y)
    {}

    CPunct(const CPunct& p) : C2coord(p.m.x, p.m.y)
    {}


    CPunct& operator=(const CPunct& p)
    {
        m.x = p.m.x;
        m.y = p.m.y;
        return *this;
    }

    void getxy(double& x, double& y)
    {
        x = m.x;
        y = m.y;
    }

    int operator==(CPunct& p)
    {
        return ((m.x == p.m.x) && (m.y == p.m.y));
    }

    void marcheaza()
    {
        glBegin(GL_POINTS);
        glVertex2d(m.x, m.y);
        glEnd();
    }

    void print(FILE* fis)
    {
        fprintf(fis, "(%+f,%+f)", m.x, m.y);
    }

};

class CVector : public C2coord
{
public:
    CVector() : C2coord(0.0, 0.0)
    {
        normalizare();
    }

    CVector(double x, double y) : C2coord(x, y)
    {
        normalizare();
    }

    CVector& operator=(CVector& p)
    {
        m.x = p.m.x;
        m.y = p.m.y;
        return *this;
    }

    int operator==(CVector& p)
    {
        return ((m.x == p.m.x) && (m.y == p.m.y));
    }

    CPunct getDest(CPunct& orig, double lungime)
    {
        double x, y;
        orig.getxy(x, y);
        CPunct p(x + m.x * lungime, y + m.y * lungime);
        return p;
    }

    void rotatie(double grade)
    {
        double x = m.x;
        double y = m.y;
        double t = 2 * (4.0 * atan(1)) * grade / 360.0;
        m.x = x * cos(t) - y * sin(t);
        m.y = x * sin(t) + y * cos(t);
        normalizare();
    }

    void deseneaza(CPunct p, double lungime)
    {
        double x, y;
        p.getxy(x, y);
        glColor3f(1.0, 0.1, 0.1);
        glBegin(GL_LINE_STRIP);
        glVertex2d(x, y);
        glVertex2d(x + m.x * lungime, y + m.y * lungime);
        glEnd();
    }

    void print(FILE* fis)
    {
        fprintf(fis, "%+fi %+fj", C2coord::m.x, C2coord::m.y);
    }

private:
    void normalizare()
    {
        double d = sqrt(C2coord::m.x * C2coord::m.x + C2coord::m.y * C2coord::m.y);
        if (d != 0.0)
        {
            C2coord::m.x = C2coord::m.x * 1.0 / d;
            C2coord::m.y = C2coord::m.y * 1.0 / d;
        }
    }
};

//clasa pentru lucrul cu numere complexe
class CComplex {
private:
    double re, im;
public:
    CComplex() : re(0.0), im(0.0) {}
    CComplex(double re1, double im1) : re(re1 * 1.0), im(im1 * 1.0) {}
    CComplex(const CComplex& c) : re(c.re), im(c.im) {}
    ~CComplex() {}

    CComplex& operator=(const CComplex& c)
    {
        re = c.re;
        im = c.im;
        return *this;
    }

    double getRe() { return re; }
    void setRe(double re1) { re = re1; }

    double getIm() { return im; }
    void setIm(double im1) { im = im1; }

    double getModul() { return sqrt(re * re + im * im); }

    int operator==(CComplex& c1)
    {
        return ((re == c1.re) && (im == c1.im));
    }

    CComplex powComplex()
    {
        CComplex rez;
        rez.re = powl(re * 1.0, 2) - powl(im * 1.0, 2);
        rez.im = 2.0 * re * im;
        return rez;
    }

    void print(FILE* f)
    {
        fprintf(f, "%.20f%+.20f i", re, im);
    }

    friend CComplex operator+(const CComplex& c1, const CComplex& c2)
    {
        CComplex rez(c1.re + c2.re, c1.im + c2.im);
        return rez;
    }

    friend CComplex operator*(CComplex& c1, CComplex& c2)
    {
        CComplex rez(c1.re * c2.re - c1.im * c2.im,
            c1.re * c2.im + c1.im * c2.re);
        return rez;
    }
};



class MMandelbrot {
private:
    struct SDate {
        CComplex c;
        int nr_iteratii = nr_iteratii_Mandelbrot;
        double modmax = modul_maxim_Mandelbrot; // modulul maxim
    } m;

public:
    MMandelbrot() : m({ CComplex(), nr_iteratii_Mandelbrot, modul_maxim_Mandelbrot }) {}

    int esteInMultimeaMandelbrot(CComplex& c) const {
        CComplex z0(0, 0);
        CComplex zn_1;

        for (int i = 1; i < m.nr_iteratii; i++)
        {
            zn_1 = z0 * z0 + c;

            if (zn_1.getModul() > m.modmax)
            {
                return i;
            }

            z0 = zn_1;
        }

        return 0;
    }

    void display(double xmin, double ymin, double xmax, double ymax) const {
        glBegin(GL_POINTS);
        for (double x = xmin; x <= xmax; x += ratie_x) {
            for (double y = ymin; y <= ymax; y += ratie_y) {
                CComplex punct_curent(x, y);
                int flag = esteInMultimeaMandelbrot(punct_curent);
                setColorBasedOnIteration(flag);
                renderPoint(x / 2, y / 2);
            }
        }
        glEnd();
    }

private:
    void setColorBasedOnIteration(int iteration) const {
        if (iteration == 0) {
            glColor3f(0.0, 1.0, 0.0); // Green
        }
        else if (iteration < 5) {
            glColor3f(1.0f, 1.0f, 0.0f); // Red
        }
        else if (iteration < 10) {
            glColor3f(0.0f, 0.0f, 1.0f); // Blue
        }
        else if (iteration < 15) {
            glColor3f(1.0f, 0.0f, 0.0f); // Yellow
        }
        else {
            glColor3f(0.0f, 0.0f, 0.0f); // Black
        }
    }

    void renderPoint(double x, double y) const {
        glVertex3d(x, y, 0);
    }
};


// multimea Mandelbrot
void Display1() {
    MMandelbrot mm;

    glColor3f(1.0, 0.1, 0.1);
    mm.display(-2, -2, 2, 2);

    glRasterPos2d(-0.9, 0.9);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'M');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'u');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 't');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'm');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'M');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'n');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'd');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'b');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'o');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 't');
}


class CImaginea1 {
public:
    void drawSquare(CPunct& p, double length, CVector& cv) {
        CPunct cp = p;
        for (int i = 0; i < 4; i++) {
            cv.deseneaza(cp, length);
            cp = cv.getDest(cp, length);
            cv.rotatie(-90);
        }
    }

    void imaginea1(CPunct p, double length, int level) {
        double cx, cy;
        p.getxy(cx, cy);

        double lm = length / 3;
        CVector cv(0, 1);
        CPunct cp(cx - lm / 2, cy - lm / 2);

        drawSquare(cp, lm, cv);

        if (level > 0) {
            CPunct points[8] = {
                CPunct(cx - lm, cy + lm), // stanga_sus
                CPunct(cx, cy + lm), // centru_sus
                CPunct(cx + lm, cy + lm), // dreapta_sus
                CPunct(cx - lm, cy), // stanga
                CPunct(cx + lm, cy), // dreapta
                CPunct(cx - lm, cy - lm), // stanga_jos
                CPunct(cx, cy - lm), // centru_jos
                CPunct(cx + lm, cy - lm) // dreapta_jos
            };

            for (auto& point : points) {
                imaginea1(point, lm, level - 1);
            }
        }
    }

    void desenarePatratExterior(CPunct p, double length) {
        double cx, cy;
        p.getxy(cx, cy);
        double lm = length;
        CVector cv(0, 1);
        CPunct cp(cx - lm / 2, cy - lm / 2);

        drawSquare(cp, lm, cv);
    }

    void afisare(double length, int level) {
        CPunct p(0.0, 0.0);
        imaginea1(p, length, level);
        desenarePatratExterior(p, length);
    }
};


void Display2() {
    CImaginea1 img1;
    img1.afisare(1, nivel);

    char c[3];
    sprintf(c, "%2d", nivel);
    glRasterPos2d(-0.98, -0.98);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'N');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'v');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '=');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[0]);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[1]);

    glRasterPos2d(-1.0, -0.9);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'I');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'm');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'g');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'n');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '1');

    nivel++;
}


class CImagine2
{
public:
    void imagine2(double length, int level, double scaling, CPunct point, CVector vector)
    {
        assert(scaling != 0);

        if (level != 0)
        {
            drawBranches(length, level, scaling, point, vector);
        }
    }

    void afisare(double length, int nivel)
    {
        CVector vector(0.0, -1.0);
        CPunct point(0.0, 2.5);

        vector.deseneaza(point, 0.25);
        point = vector.getDest(point, 0.25);
        imagine2(length, nivel, 0.4, point, vector);
    }

private:
    void drawBranches(double length, int level, double scaling, CPunct point, CVector vector)
    {
        CPunct endPoint1, endPoint2;

        const double ROTATE_ANGLE_1 = -50;
        const double ROTATE_ANGLE_2 = 90;
        const double ROTATE_ANGLE_3 = 30;
        const double ROTATE_ANGLE_4 = -70;
        const double ROTATE_ANGLE_5 = -90;
        const double ROTATE_ANGLE_6 = 120;

        vector.rotatie(ROTATE_ANGLE_1);
        vector.deseneaza(point, length);
        endPoint1 = vector.getDest(point, length);
        imagine2(length * scaling, level - 1, scaling, endPoint1, vector);

        vector.rotatie(ROTATE_ANGLE_2);
        vector.deseneaza(point, length);
        endPoint1 = vector.getDest(point, length);
        endPoint2 = endPoint1;

        vector.rotatie(ROTATE_ANGLE_3);
        vector.deseneaza(endPoint1, length);
        endPoint1 = vector.getDest(endPoint1, length);
        imagine2(length * scaling, level - 1, scaling, endPoint1, vector);

        endPoint1 = endPoint2;
        vector.rotatie(ROTATE_ANGLE_4);
        vector.deseneaza(endPoint1, length);
        endPoint1 = vector.getDest(endPoint1, length);
        endPoint2 = endPoint1;

        double newLength = length / 1.3;
        vector.rotatie(ROTATE_ANGLE_5);
        vector.deseneaza(endPoint1, newLength);
        endPoint1 = vector.getDest(endPoint1, newLength);
        imagine2(newLength * scaling, level - 1, scaling, endPoint1, vector);

        endPoint1 = endPoint2;
        vector.rotatie(ROTATE_ANGLE_6);
        vector.deseneaza(endPoint1, newLength);
        endPoint1 = vector.getDest(endPoint1, newLength);
        imagine2(newLength * scaling, level - 1, scaling, endPoint1, vector);
    }
};


void Display3() {
    CImagine2 img2;

    glPushMatrix();
    glLoadIdentity();
    glScaled(0.4, 0.4, 1);
    glTranslated(-0.5, -0.5, 0.0);
    img2.afisare(1, nivel);
    glPopMatrix();

    char c[3];
    sprintf(c, "%2d", nivel);
    glRasterPos2d(-0.98, -0.98);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'N');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'v');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '=');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[0]);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[1]);

    glRasterPos2d(-1.0, -0.9);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'I');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'm');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'g');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'n');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '2');

    nivel++;
}


class CImaginea3
{
public:
    static constexpr double ROTATION_ANGLE = 60.0; // Degrees

    void imaginea3(double length, int level, CPunct& startPoint, CVector& direction, int directionChange)
    {
        if (level == 0)
        {
            drawLine(startPoint, length, direction);
            return;
        }

        imaginea3(length / 3, level - 1, startPoint, direction, -directionChange);

        direction.rotatie(directionChange * ROTATION_ANGLE);
        imaginea3(length / 3, level - 1, startPoint, direction, directionChange);

        direction.rotatie(directionChange * ROTATION_ANGLE);
        imaginea3(length / 3, level - 1, startPoint, direction, -directionChange);
    }

    void afisare(double length, int level)
    {
        CVector direction(0.0, 1.0);
        CPunct startPoint(0.0, -0.5);

        imaginea3(length * pow(1.5, level), level, startPoint, direction, 1);
    }


private:
    void drawLine(CPunct& startPoint, double length, CVector& direction)
    {
        direction.deseneaza(startPoint, length);
        startPoint = direction.getDest(startPoint, length);
    }
};



void Display4() {
    CImaginea3 img3;
    img3.afisare(1, nivel);

    char c[3];
    sprintf(c, "%2d", nivel);
    glRasterPos2d(-0.98, -0.98);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'N');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'v');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '=');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[0]);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[1]);

    glRasterPos2d(-1.0, -0.9);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'I');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'm');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'g');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'n');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '3');

    nivel++;
}

void Init(void) {

    glClearColor(1.0, 1.0, 1.0, 1.0);

    glLineWidth(1);

    glPointSize(3);

    glPolygonMode(GL_FRONT, GL_LINE);
}

void Display(void)
{
    switch (prevKey)
    {
    case '0':
        glClear(GL_COLOR_BUFFER_BIT);
        nivel = 0;
        fprintf(stderr, "nivel = %d\n", nivel);
        break;
    case '1':
        glClear(GL_COLOR_BUFFER_BIT);
        Display1();
        break;
    case '2':
        glClear(GL_COLOR_BUFFER_BIT);
        Display2();
        break;
    case '3':
        glClear(GL_COLOR_BUFFER_BIT);
        Display3();
        break;
    case '4':
        glClear(GL_COLOR_BUFFER_BIT);
        Display4();
        break;
    default:
        break;
    }

    glFlush();
}

void Reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void KeyboardFunc(unsigned char key, int x, int y)
{
    prevKey = key;
    if (key == 27) // escape
        exit(0);
    glutPostRedisplay();
}

void MouseFunc(int button, int state, int x, int y)
{
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitWindowSize(dim, dim);

    glutInitWindowPosition(100, 100);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutCreateWindow(argv[0]);

    Init();

    glutReshapeFunc(Reshape);

    glutKeyboardFunc(KeyboardFunc);

    glutMouseFunc(MouseFunc);

    glutDisplayFunc(Display);

    glutMainLoop();

    return 0;
}