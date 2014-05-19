////////////////////////////////////////////////////
// anim.cpp version 4.1
// Template code for drawing an articulated figure.
// CS 174A 
////////////////////////////////////////////////////

#ifdef _WIN32
#include <windows.h>
#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GLUT/glut.h>
#endif

#include "Ball.h"
#include "FrameSaver.h"
#include "Timer.h"
#include "Shapes.h"
#include "tga.h"

#include "Angel/Angel.h"

#ifdef __APPLE__
#define glutInitContextVersion(a,b)
#define glutInitContextProfile(a)
#define glewExperimental int glewExperimentalAPPLE
#define glewInit()
#endif

/////////////////////////////////////////////////////
// These are global variables
//
//
// Add additional ones if you need,
// especially for animation
//////////////////////////////////////////////////////

FrameSaver FrSaver ;
Timer TM ;

BallData *Arcball = NULL ;
int Width = 800;
int Height = 800 ;
int Button = -1 ;
float Zoom = 1 ;
int PrevY = 0 ;

int Animate = 0 ;
int Recording = 0 ;

void resetArcball() ;
void save_image();
void instructions();
void set_colour(float r, float g, float b) ;

const int STRLEN = 100;
typedef char STR[STRLEN];

#define PI 3.1415926535897
#define X 0
#define Y 1
#define Z 2

//  TwoPi constant
const GLfloat TwoPI = 2.0 * M_PI;

float mlgUpperLegAngle = 0.0;
float mlgLowerLegAngle = 0.0;

float flgUpperLegAngle = 0.0;
float flgLowerLegAngle = 0.0;

float xPosPlane = 0.0;
float yPosPlane = 0.0;
float zPosPlane = 0.0;

//texture
GLuint texture_cube;
GLuint texture_earth;

// Structs that hold the Vertex Array Object index and number of vertices of each shape.
ShapeData cubeData;
ShapeData sphereData;
ShapeData coneData;
ShapeData cylData;

// Matrix stack that can be used to push and pop the modelview matrix.
class MatrixStack {
    int    _index;
    int    _size;
    mat4*  _matrices;

   public:
    MatrixStack( int numMatrices = 32 ):_index(0), _size(numMatrices)
        { _matrices = new mat4[numMatrices]; }

    ~MatrixStack()
	{ delete[]_matrices; }

    void push( const mat4& m ) {
        assert( _index + 1 < _size );
        _matrices[_index++] = m;
    }

    mat4& pop( void ) {
        assert( _index - 1 >= 0 );
        _index--;
        return _matrices[_index];
    }
};

MatrixStack  mvstack;
mat4         model_view;
GLint        uModelView, uProjection, uView;
GLint        uAmbient, uDiffuse, uSpecular, uLightPos, uShininess;
GLint        uTex, uEnableTex;

// The eye point and look-at point.
// Currently unused. Use to control a camera with LookAt().
Angel::vec4 eye(0, 0.0, 50.0,1.0);
Angel::vec4 ref(0.0, 0.0, 0.0,1.0);
Angel::vec4 up(0.0,1.0,0.0,0.0);

double TIME = 0.0 ;

/////////////////////////////////////////////////////
//
//    My helper functions
//
////////////////////////////////////////////////////
float getRgbFloat(int val) {
    
    return (float) val / 256.0;
}


/////////////////////////////////////////////////////
//    PROC: drawCylinder()
//    DOES: this function 
//          render a solid cylinder  oriented along the Z axis. Both bases are of radius 1. 
//          The bases of the cylinder are placed at Z = 0, and at Z = 1.
//
//          
// Don't change.
//////////////////////////////////////////////////////
void drawCylinder(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cylData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cylData.numVertices );
}


//////////////////////////////////////////////////////
//    PROC: drawCone()
//    DOES: this function 
//          render a solid cone oriented along the Z axis with base radius 1. 
//          The base of the cone is placed at Z = 0, and the top at Z = 1. 
//         
// Don't change.
//////////////////////////////////////////////////////
void drawCone(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( coneData.vao );
    glDrawArrays( GL_TRIANGLES, 0, coneData.numVertices );
}


//////////////////////////////////////////////////////
//    PROC: drawCube()
//    DOES: this function draws a cube with dimensions 1,1,1
//          centered around the origin.
// 
// Don't change.
//////////////////////////////////////////////////////
void drawCube(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_cube );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
}


//////////////////////////////////////////////////////
//    PROC: drawSphere()
//    DOES: this function draws a sphere with radius 1
//          centered around the origin.
// 
// Don't change.
//////////////////////////////////////////////////////
void drawSphere(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_earth);
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( sphereData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
}


void resetArcball()
{
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}


/*********************************************************
 PROC: set_colour();
 DOES: sets all material properties to the given colour
 -- don't change
 **********************************************************/

void set_colour(float r, float g, float b)
{
    float ambient  = 0.2f;
    float diffuse  = 0.6f;
    float specular = 0.2f;
    glUniform4f(uAmbient,  ambient*r,  ambient*g,  ambient*b,  1.0f);
    glUniform4f(uDiffuse,  diffuse*r,  diffuse*g,  diffuse*b,  1.0f);
    glUniform4f(uSpecular, specular*r, specular*g, specular*b, 1.0f);
}


/*********************************************************
 PROC: instructions()
 DOES: display instruction in the console window.
 -- No need to change

 **********************************************************/
void instructions()
{
    printf("Press:\n");
    printf("  s to save the image\n");
    printf("  r to restore the original view.\n") ;
    printf("  0 to set it to the zero state.\n") ;
    printf("  a to toggle the animation.\n") ;
    printf("  m to toggle frame dumping.\n") ;
    printf("  q to quit.\n");
}


/*********************************************************
 PROC: myinit()
 DOES: performs most of the OpenGL intialization
 -- change these with care, if you must.
 
 **********************************************************/
void myinit(void)
{
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram(program);
    
    // Generate vertex arrays for geometric shapes
    generateCube(program, &cubeData);
    generateSphere(program, &sphereData);
    generateCone(program, &coneData);
    generateCylinder(program, &cylData);
    
    uModelView  = glGetUniformLocation( program, "ModelView"  );
    uProjection = glGetUniformLocation( program, "Projection" );
    uView       = glGetUniformLocation( program, "View"       );
    
    glClearColor( getRgbFloat(135), getRgbFloat(206), getRgbFloat(250), 1.0 ); // sky blue background
    
    uAmbient   = glGetUniformLocation( program, "AmbientProduct"  );
    uDiffuse   = glGetUniformLocation( program, "DiffuseProduct"  );
    uSpecular  = glGetUniformLocation( program, "SpecularProduct" );
    uLightPos  = glGetUniformLocation( program, "LightPosition"   );
    uShininess = glGetUniformLocation( program, "Shininess"       );
    uTex       = glGetUniformLocation( program, "Tex"             );
    uEnableTex = glGetUniformLocation( program, "EnableTex"       );
    
    glUniform4f(uAmbient,    0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(uDiffuse,    0.6f,  0.6f,  0.6f, 1.0f);
    glUniform4f(uSpecular,   0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(uLightPos,  15.0f, 15.0f, 30.0f, 0.0f);
    glUniform1f(uShininess, 100.0f);
    
    glEnable(GL_DEPTH_TEST);
    
    Arcball = new BallData;
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}


//////////////////////////////////////////////////////
//    PROC: myKey()
//    DOES: this function gets caled for any keypresses
// 
//////////////////////////////////////////////////////
void myKey(unsigned char key, int x, int y)
{
    float time ;
    switch (key) {
        case 'q':
        case 27:
            exit(0); 
        case 's':
            FrSaver.DumpPPM(Width,Height) ;
            break;
        case 'r':
            resetArcball() ;
            glutPostRedisplay() ;
            break ;
        case 'a': // togle animation
            Animate = 1 - Animate ;
            // reset the timer to point to the current time		
            time = TM.GetElapsedTime() ;
            TM.Reset() ;
            // printf("Elapsed time %f\n", time) ;
            break ;
        case '0':
            //reset your object
            break ;
        case 'm':
            if( Recording == 1 )
            {
                printf("Frame recording disabled.\n") ;
                Recording = 0 ;
            }
            else
            {
                printf("Frame recording enabled.\n") ;
                Recording = 1  ;
            }
            FrSaver.Toggle(Width);
            break ;
        case 'h':
        case '?':
            instructions();
            break;
    }
    glutPostRedisplay() ;

}


/**********************************************
 PROC: myMouseCB()
 DOES: handles the mouse button interaction
 
 -- don't change
 **********************************************************/
void myMouseCB(int button, int state, int x, int y)
{
    Button = button ;
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        HVect arcball_coords;
        arcball_coords.x = 2.0*(float)x/(float)Width-1.0;
        arcball_coords.y = -2.0*(float)y/(float)Height+1.0;
        Ball_Mouse(Arcball, arcball_coords) ;
        Ball_Update(Arcball);
        Ball_BeginDrag(Arcball);

    }
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_UP )
    {
        Ball_EndDrag(Arcball);
        Button = -1 ;
    }
    if( Button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
    {
        PrevY = y ;
    }


    // Tell the system to redraw the window
    glutPostRedisplay() ;
}


/**********************************************
 PROC: myMotionCB()
 DOES: handles the mouse motion interaction
 
 -- don't change
 **********************************************************/
void myMotionCB(int x, int y)
{
    if( Button == GLUT_LEFT_BUTTON )
    {
        HVect arcball_coords;
        arcball_coords.x = 2.0*(float)x/(float)Width - 1.0 ;
        arcball_coords.y = -2.0*(float)y/(float)Height + 1.0 ;
        Ball_Mouse(Arcball,arcball_coords);
        Ball_Update(Arcball);
        glutPostRedisplay() ;
    }
    else if( Button == GLUT_RIGHT_BUTTON )
    {
        if( y - PrevY > 0 )
            Zoom  = Zoom * 1.03 ;
        else 
            Zoom  = Zoom * 0.97 ;
        PrevY = y ;
        glutPostRedisplay() ;
    }
}


/**********************************************
 PROC: myReshape()
 DOES: handles the window being resized
 
 -- don't change
 **********************************************************/
void myReshape(int w, int h)
{
    Width = w;
    Height = h;
    
    glViewport(0, 0, w, h);
    
    mat4 projection = Perspective(50.0f, (float)w/(float)h, 1.0f, 1000.0f);
    glUniformMatrix4fv( uProjection, 1, GL_TRUE, projection );
}

// not 1 pop on mvstack (pop)
void drawFLGWheel(mat4 view_trans) {
    
    mat4 model_trans = mvstack.pop();
    model_trans *= Translate(0, 0.2, 0);
    model_trans *= RotateX(90);
    model_trans *= Scale(0.2, 0.2, 0.1);
    model_view = view_trans *= model_trans;
    set_colour(getRgbFloat(32), getRgbFloat(32), getRgbFloat(32));
    drawCylinder();
}

// net 1 pop on mvstack
void drawFLG(mat4 view_trans) {
    
    // upper leg of FLG
    mat4 model_trans = mvstack.pop();
    mvstack.push(model_trans); // intentional
    model_trans *= Translate(0, -1.2, 0);
    model_trans *= RotateX(90);
    
    // rotation
    model_trans *= Translate(0, 0, -0.9);
    model_trans *= RotateY(-flgUpperLegAngle);
    model_trans *= Translate(0, 0, 0.9);
    
    mvstack.push(model_trans);
    model_trans *= Scale(0.1, 0.1, 0.3);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(224), getRgbFloat(223), getRgbFloat(219)); // stainless steel color
    drawCylinder();
    
    // lower leg of FLG
    model_trans = mvstack.pop();
    model_trans *= Translate(0, 0, 0.6);
    
    // rotation
    model_trans *= Translate(0.05, 0, -0.3);
    model_trans *= RotateY(flgLowerLegAngle);
    model_trans *= Translate(-0.05, 0, 0.3);
    
    mvstack.push(model_trans);
    model_trans *= Scale(0.1, 0.1, 0.3);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(224), getRgbFloat(223), getRgbFloat(219)); // stainless steel color
    drawCylinder();
    
    // FLG wheels
    
    model_trans = mvstack.pop(); // even # push/pops after this other than very last pop w/ comment
    mvstack.push(model_trans); // save copy of model_trans
    
    model_trans *= Translate(0, 0, 0.3);
    mvstack.push(model_trans); // local copy of model_trans for wheel transformations
    drawFLGWheel(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put wheel bar center trans back on stack
    
    model_trans = mvstack.pop();
    model_trans *= ReflectXY(); // Reflect over XY plane for other three wheels
    mvstack.push(model_trans);
    
    model_trans *= Translate(0, -0.4, -0.3);
    mvstack.push(model_trans); // local copy of model_trans for wheel transformations
    drawFLGWheel(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put wheel bar center trans back on stack
    
    
    mvstack.pop(); // intentional
    mvstack.pop(); // get rid of the copy of model_trans meant for this specific FLG
}

// net 1 pop on mvstack (pop)
void drawMLGWheel(mat4 view_trans) {
    
    mat4 model_trans = mvstack.pop();
    model_trans *= Translate(0, 0.2, 0);
    model_trans *= RotateX(90);
    model_trans *= Scale(0.2, 0.2, 0.1);
    model_view = view_trans *= model_trans;
    set_colour(getRgbFloat(32), getRgbFloat(32), getRgbFloat(32));
    drawCylinder();
}

// net 1 pop on mvstack (pop, push, push, pop, push, pop, pop)
void drawMLG(mat4 view_trans) {
    
    // upper leg of MLG
    mat4 model_trans = mvstack.pop();
    mvstack.push(model_trans); // intentional
    model_trans *= Translate(0, -0.8, 1.0);
    model_trans *= RotateX(90);
    
    model_trans *= Translate(0, -0.2, -0.1);
    model_trans *= RotateX(-mlgUpperLegAngle);
    model_trans *= Translate(0, 0.2, 0.1);
    
    mvstack.push(model_trans);
    model_trans *= Scale(0.1, 0.1, 0.4);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(224), getRgbFloat(223), getRgbFloat(219)); // stainless steel color
    drawCylinder();
    
    // lower leg of MLG
    model_trans = mvstack.pop();
    model_trans *= Translate(0, 0, 0.8);
    
    model_trans *= Translate(0, -0.1, -0.4);
    model_trans *= RotateX(mlgLowerLegAngle);
    model_trans *= Translate(0, 0.1, 0.4);
    
    mvstack.push(model_trans);
    model_trans *= Scale(0.1, 0.1, 0.4);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(224), getRgbFloat(223), getRgbFloat(219));
    drawCylinder();
    
    // wheel bar of MLG
    model_trans = mvstack.pop();
    model_trans *= Translate(0, 0, 0.5);
    mvstack.push(model_trans);
    model_trans *= RotateY(90);
    model_trans *= Scale(0.1, 0.1, 0.5);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(224), getRgbFloat(223), getRgbFloat(219));
    drawCylinder();
    
    // MLG wheels
    
    model_trans = mvstack.pop(); // even # push/pops after this other than very last pop w/ comment
    mvstack.push(model_trans); // save copy of model_trans at fuselage center

    mvstack.push(model_trans); // local copy of model_trans for wheel transformations
    drawMLGWheel(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put wheel bar center trans back on stack
    
    model_trans *= Translate(-0.4, 0, 0);
    mvstack.push(model_trans); // local copy of model_trans for wheel transformations
    drawMLGWheel(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put wheel bar center trans back on stack
    
    model_trans *= Translate(0.4, 0, 0);
    mvstack.push(model_trans); // local copy of model_trans for wheel transformations
    drawMLGWheel(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put wheel bar center trans back on stack
    
    model_trans = mvstack.pop();
    model_trans *= ReflectXY(); // Reflect over XY plane for other three wheels
    mvstack.push(model_trans);
    
    model_trans *= Translate(0, -0.4, 0);
    mvstack.push(model_trans); // local copy of model_trans for wheel transformations
    drawMLGWheel(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put wheel bar center trans back on stack
    
    model_trans *= Translate(-0.4, -0.4, 0);
    mvstack.push(model_trans); // local copy of model_trans for wheel transformations
    drawMLGWheel(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put wheel bar center trans back on stack
    
    model_trans *= Translate(0.4, -0.4, 0);
    mvstack.push(model_trans); // local copy of model_trans for wheel transformations
    drawMLGWheel(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put wheel bar center trans back on stack

    mvstack.pop(); // intentional
    mvstack.pop(); // get rid of the copy of model_trans meant for this specific MLG
}

/*********************************************************
 **********************************************************
 **********************************************************
 
 PROC: display()
 DOES: this gets called by the event handler to draw the scene
       so this is where you need to build your BEE
 
 MAKE YOUR CHANGES AND ADDITIONS HERE
 
 ** Add other procedures, such as drawLegs
 *** Use a hierarchical approach
 
 **********************************************************
 **********************************************************
 **********************************************************/
void display(void)
{
    // Clear the screen with the background colour (set in myinit)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    mat4 model_trans(1.0f);
    mat4 view_trans(1.0f);
    
    view_trans *= LookAt(eye, ref, up);
    
    view_trans *= Translate(0.0f, 0.0f, 0.0f); //the same effect as zoom out
    
    // below deals with zoom in/out by mouse
    HMatrix r;
    Ball_Value(Arcball,r);
    mat4 mat_arcball_rot(
                         r[0][0], r[0][1], r[0][2], r[0][3],
                         r[1][0], r[1][1], r[1][2], r[1][3],
                         r[2][0], r[2][1], r[2][2], r[2][3],
                         r[3][0], r[3][1], r[3][2], r[3][3]);
    view_trans *= mat_arcball_rot;
    view_trans *= Scale(Zoom);
    
    glUniformMatrix4fv( uView, 1, GL_TRUE, model_view );
    
    mvstack.push(model_trans);//push, now identity is on the stack
    
   
/**************************************************************
   Your drawing/modeling starts here
***************************************************************/
    
    // model ground plane
    model_trans *= Translate(0, -5, 0);
    mvstack.push(model_trans);

    model_trans *= Scale(1000, 0.25, 1000);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(131), getRgbFloat(181), getRgbFloat(106)); // forest green color
    drawCube();
    
    // model runway
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // save ground plane location for fuselage
    model_trans *= Translate(45.0, 0.25, 0);
    model_trans *= Scale(100.0, 0.1, 10.0);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(105), getRgbFloat(105), getRgbFloat(105)); // dim gray color
    drawCube();
    
    // model control tower base
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // save ground plane location for fuselage
    model_trans *= Translate(10.0, 5.0, -20.0);
    mvstack.push(model_trans); // for top piece of control tower
    model_trans *= Scale(2.0, 10.0, 2.0);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(210), getRgbFloat(180), getRgbFloat(140)); // tan building color
    drawCube();
    
    // model control tower top
    model_trans = mvstack.pop();
    model_trans *= Translate(0, 5.0, 0);
    //mvstack.push(model_trans); // save control tower top for pole
    model_trans *= RotateX(270);
    model_trans *= Scale(2.0, 2.0, 2.0);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(70), getRgbFloat(130), getRgbFloat(180)); // steel blue color
    drawCone();
    
//    // model control tower pole
//    model_trans = mvstack.pop();
//    model_trans *= Translate(0, 3.0, 0);
//    mvstack.push(model_trans); // save pole location for wind cone
//    model_trans *= RotateX(90);
//    model_trans *= Scale(0.1, 0.1, 1.0);
//    model_view = view_trans * model_trans;
//    set_colour(getRgbFloat(224), getRgbFloat(223), getRgbFloat(219)); // stainless steel color
//    drawCylinder();
//    
//    // model control tower wind cone
//    model_trans = mvstack.pop();
//    model_trans *= Translate(-0.6, 1.0, 0);
//    model_trans *= RotateY(90);
//    model_trans *= Scale(0.5, 0.25, 0.5);
//    model_view = view_trans * model_trans;
//    set_colour(getRgbFloat(255), getRgbFloat(69), getRgbFloat(0)); // orange wind cone color
//    drawCone();
    
    // model airplane fuselage
    model_trans = mvstack.pop();
    model_trans *= Translate(xPosPlane, 2.5 + yPosPlane, zPosPlane);
    //model_trans *= RotateZ(15); // take-off
    //model_trans *= RotateX(10); // bank right
    mvstack.push(model_trans);
    model_trans *= RotateY(90);
    model_trans *= Scale(1, 1, 5);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(173), getRgbFloat(178), getRgbFloat(189)); // aluminum fuselage color
    drawCylinder();
    
    // model airplane nose
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // don't need to save nose location
    model_trans *= Translate(5, 0, 0);
    model_trans *= RotateY(90);
    model_trans *= Scale(1, 1, 2);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(173), getRgbFloat(178), getRgbFloat(189)); // aluminum fuselage color
    drawSphere();
    
    // model airplane fuselage cone
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // don't need to save fuselage cone location
    model_trans *= Translate(-6, 0, 0);
    model_trans *= RotateY(90);
    model_trans *= Scale(1, 1, 1);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(173), getRgbFloat(178), getRgbFloat(189)); // aluminum fuselage color
    drawCone();
    
    // model airplane wing
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // save fuselage location
    model_trans *= Translate(0, -0.4, 0);
    mvstack.push(model_trans); // intentional 2nd push for left engine
    mvstack.push(model_trans); // intentional 2nd push for right engine
    model_trans *= Scale(1, 0.2, 8);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(173), getRgbFloat(178), getRgbFloat(189)); // aluminum fuselage color
    drawCylinder();
    
    // model left airplane engine
    model_trans = mvstack.pop();
    model_trans *= Translate(0.2, -0.5, -2.5);
    mvstack.push(model_trans);
    model_trans *= RotateY(90);
    model_trans *= Scale(0.5, 0.5, 1);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(173), getRgbFloat(178), getRgbFloat(189)); // aluminum fuselage color
    drawCylinder();
    
    // model left airplane engine cone
    model_trans = mvstack.pop();
    model_trans *= Translate(-2, 0, 0);
    model_trans *= RotateY(90);
    model_trans *= Scale(0.5, 0.5, 1.0);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(173), getRgbFloat(178), getRgbFloat(189)); // aluminum fuselage color
    drawCone();
    
    // model right airplane engine
    model_trans = mvstack.pop();
    model_trans *= Translate(0.2, -0.5, 2.5);
    mvstack.push(model_trans);
    model_trans *= RotateY(90);
    model_trans *= Scale(0.5, 0.5, 1);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(173), getRgbFloat(178), getRgbFloat(189)); // aluminum fuselage color
    drawCylinder();
    
    // model right airplane engine cone
    model_trans = mvstack.pop();
    model_trans *= Translate(-2, 0, 0);
    model_trans *= RotateY(90);
    model_trans *= Scale(0.5, 0.5, 1.0);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(173), getRgbFloat(178), getRgbFloat(189)); // aluminum fuselage color
    drawCone();
    
    // model airplane elevator (back wing)
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // save fuselage location
    model_trans *= Translate(-5.5, 0.4, 0);
    // model_trans *= ShearXY(1.0); TODO: Incorporate shear to make wings into triangles
    model_trans *= Scale(0.5, 0.2, 3);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(173), getRgbFloat(178), getRgbFloat(189)); // aluminum fuselage color
    drawCylinder();
    
    // model airplane tail
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // save fuselage location
    model_trans *= Translate(-5, 1.01, 0);
    model_trans *= ReflectYZ();
    model_trans *= ShearXY(0.75);
    model_trans *= Scale(2, 4, 0.2);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(173), getRgbFloat(178), getRgbFloat(189)); // aluminum fuselage color
    drawCube();
    
    // model MLG
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // save fuselage location
    
    mvstack.push(model_trans); // local copy of model_trans for MLG transformations
    drawMLG(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put fuselage center trans back on stack
    
    model_trans = mvstack.pop();
    model_trans *= ReflectXY(); // Reflect over XY plane for other MLG
    mvstack.push(model_trans);
    
    mvstack.push(model_trans);
    drawMLG(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put fuselage center trans back on stack
    
    // model FLG
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // save fuselage location
    model_trans *= Translate(5.5, 0, 0);
    mvstack.push(model_trans); // local copy of model_trans for FLG transformations
    drawFLG(view_trans);
    
    model_trans = mvstack.pop(); // intentional
    model_trans = mvstack.pop(); // avoid stack overflow
    
/**************************************************************
     Your drawing/modeling ends here
 ***************************************************************/
    
    glutSwapBuffers();
    if(Recording == 1)
        FrSaver.DumpPPM(Width, Height);
}


/*********************************************************
 **********************************************************
 **********************************************************
 
 PROC: idle()
 DOES: this gets called when nothing happens. 
       That's not true. 
       A lot of things happen can here.
       This is where you do your animation.
 
 MAKE YOUR CHANGES AND ADDITIONS HERE
 
 **********************************************************
 **********************************************************
 **********************************************************/
void idle(void)
{
    if( Animate == 1 )
    {
        // TM.Reset() ; // commenting out this will make the time run from 0
        // leaving 'Time' counts the time interval between successive calls to idleCB
        if( Recording == 0 )
            TIME = TM.GetElapsedTime() ;
        else
            TIME += 0.033 ; // save at 30 frames per second.
        
        //Your code starts here
        
        if (TIME < TwoPI) {
            
            // 360 degree camera fly-around using LookAt
            eye = vec4(30*cos(TIME), 10.0, 30*sin(TIME),1.0);
        } else {
            
            // Take-off
            xPosPlane += 1.0 / 30.0;
        }
        
        // calculate LG angles as a funcion of TIME
        mlgUpperLegAngle = (((sin(fmod(((TIME / 2.0) / TwoPI), 1.0) * TwoPI) + 1.0) / 2.0) * 90);
        mlgLowerLegAngle = (((sin(fmod(((TIME / 2.0) / TwoPI), 1.0) * TwoPI) + 1.0) / 2.0) * 180);
        flgUpperLegAngle = (((sin(fmod(((TIME / 2.0) / TwoPI), 1.0) * TwoPI) + 1.0) / 2.0) * 90);
        flgLowerLegAngle = (((sin(fmod(((TIME / 2.0) / TwoPI), 1.0) * TwoPI) + 1.0) / 2.0) * 180);

        //Your code ends here
        
        printf("TIME %f\n", TIME) ;
        glutPostRedisplay() ;
    }
}

/*********************************************************
     PROC: main()
     DOES: calls initialization, then hands over control
           to the event handler, which calls 
           display() whenever the screen needs to be redrawn
**********************************************************/

int main(int argc, char** argv) 
{
    glutInit(&argc, argv);
    // If your code fails to run, uncommenting these lines may help.
    //glutInitContextVersion(3, 2);
    //glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Width,Height);
    glutCreateWindow(argv[0]);
    printf("GL version %s\n", glGetString(GL_VERSION));
    glewExperimental = GL_TRUE;
    glewInit();
    
    instructions();
    myinit(); //performs most of the OpenGL intialization
    
    
    glutKeyboardFunc( myKey );   //keyboard event handler
    glutMouseFunc(myMouseCB) ;   //mouse button event handler
    glutMotionFunc(myMotionCB) ; //mouse motion event handler
    
    glutReshapeFunc (myReshape); //reshape event handler
    glutDisplayFunc(display);    //draw a new scene
    glutIdleFunc(idle) ;         //when nothing happens, do animaiton

    
    glutMainLoop();

    TM.Reset() ;
    return 0;         // never reached
}




