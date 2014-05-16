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

float beeXOffset;
float beeYOffset;
float beeZOffset;

float beeYRotationAngle;
float beeWingAngle;

float beeYWingOffset;
float beeZWingOffset;

float beeUpperLegAngle;
float beeLowerLegAngle;

float beeYUpperLegOffset;
float beeZUpperLegOffset;

float beeYLowerLegOffset;
float beeZLowerLegOffset;

float treeRotationAngle;

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

// net 1 pop on mvstack (pop, push, push, pop, pop)
void drawLeg(mat4 view_trans) {
    
    // upper part of leg
    mat4 model_trans = mvstack.pop();
    mvstack.push(model_trans); // intentional
    if (beeYUpperLegOffset == 0) {
        
        beeYUpperLegOffset = -0.5;
    }
    if (beeZUpperLegOffset == 0) {
        
        beeZUpperLegOffset = 0.3125;
    }
    model_trans *= Translate(0, beeYUpperLegOffset, beeZUpperLegOffset);
    model_trans *= RotateX(90 + beeUpperLegAngle);
    mvstack.push(model_trans);
    model_trans *= Scale(0.125, 0.125, 0.5);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(96), getRgbFloat(96), getRgbFloat(96));
    drawCube();
    
    // lower part of leg
    model_trans = mvstack.pop();
    if (beeYLowerLegOffset == 0) {
        
        beeYLowerLegOffset = -0.5;
    }
    if (beeZLowerLegOffset == 0) {
        
        beeZLowerLegOffset = 0.0;
    }
    model_trans *= Translate(0, beeZLowerLegOffset, -beeYLowerLegOffset);
    model_trans *= RotateX(beeLowerLegAngle);
    model_trans *= Scale(0.125, 0.125, 0.5);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(96), getRgbFloat(96), getRgbFloat(96));
    drawCube();
    
    mvstack.pop(); // get rid of the copy of model_trans meant for this specific leg
}

// net 1 pop on mvstack
void drawTreeConnector(mat4 view_trans, float yOffset, float radius) {
    
    mat4 model_trans = mvstack.pop();
    model_trans *= Translate(0, yOffset, 0);
    model_trans *= Scale(radius, radius, radius);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(160), getRgbFloat(82), getRgbFloat(45));
    drawSphere();
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
    
    view_trans *= Translate(0.0f, 0.0f, -15.0f); //the same effect as zoom out
    
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
    model_trans *= Scale(20, 0.25, 20);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(131), getRgbFloat(181), getRgbFloat(106)); // forest green color
    drawCube();
    
    // model tree trunk
    for (int i=0; i < 8; i++) {
        
        float treeRotationAngleInRads = treeRotationAngle * i * DegreesToRadians;
        
        model_trans = mvstack.pop();
        model_trans *= Translate(-(0.375 * sin(treeRotationAngleInRads)), (0.375 + (0.375 * cos(treeRotationAngleInRads))), 0);
        model_trans *= RotateZ(treeRotationAngle * i);
        mvstack.push(model_trans);
        model_trans *= Scale(0.25, 0.75, 0.25);
        model_view = view_trans * model_trans;
        set_colour(getRgbFloat(160), getRgbFloat(82), getRgbFloat(45));
        drawCube();
        
        model_trans = mvstack.pop();
        mvstack.push(model_trans);
        mvstack.push(model_trans); // intentional, copy for drawTreeConnector() to throw away
        drawTreeConnector(view_trans, 0.375, 0.125);
    }
    
    // model tree sphere
    model_trans = mvstack.pop();
    model_trans *= Translate(0, 1, 0);
    // mvstack.push(model_trans); // don't push here because we don't need to save tree transformation after this point
    model_trans *= Scale(1.0);
    model_view = view_trans * model_trans;
    set_colour(0.8f, 0.0f, 0.0f);
    drawSphere();
    
    // model bee body
    model_trans = mvstack.pop();
    if (beeZOffset == 0) {
        
        beeZOffset = 5.0;
    }
    model_trans *= Translate(beeXOffset, beeYOffset, beeZOffset);
    model_trans *= RotateY(beeYRotationAngle);
    mvstack.push(model_trans);
    model_trans *= Scale(1.0, 0.5, 0.5);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(96), getRgbFloat(96), getRgbFloat(96));
    drawCube(); 
    
    // model bee head
    model_trans = mvstack.pop();
    mvstack.push(model_trans);
    model_trans *= Translate(-0.75, 0, 0);
    model_trans *= Scale(0.25);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(101), getRgbFloat(70), getRgbFloat(128));
    drawSphere();
    
    // model bee tail
    model_trans = mvstack.pop();
    mvstack.push(model_trans);
    model_trans *= Translate(1.5, 0, 0);
    model_trans *= Scale(1, 0.55, 0.45);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(204), getRgbFloat(204), getRgbFloat(0));
    drawSphere();
    
    // 1 thing on stack at this point
    
    /* model bee legs */
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // save copy of model_trans at bee body center
    
    mvstack.push(model_trans); // local copy of model_trans for bee leg transformations
    drawLeg(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put bee body center trans back on stack
    
    model_trans *= Translate(-0.25, 0, 0);
    mvstack.push(model_trans);
    drawLeg(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put bee body center trans back on stack
    
    model_trans *= Translate(0.25, 0, 0);
    mvstack.push(model_trans);
    drawLeg(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put bee body center trans back on stack
    
    model_trans = mvstack.pop();
    model_trans *= ReflectXY(); // Reflect over XY plane for other three bee legs
    mvstack.push(model_trans);
    
    mvstack.push(model_trans);
    drawLeg(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put bee body center trans back on stack
    
    model_trans *= Translate(-0.25, 0, 0);
    mvstack.push(model_trans);
    drawLeg(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put bee body center trans back on stack
    
    model_trans *= Translate(0.25, 0, 0);
    mvstack.push(model_trans);
    drawLeg(view_trans);
    model_trans = mvstack.pop();
    mvstack.push(model_trans); // put bee body center trans back on stack
    
    // model right bee wing
    model_trans = mvstack.pop();
    mvstack.push(model_trans);
    if (beeYWingOffset == 0) {
        
        beeYWingOffset = 0.28125;
    }
    if (beeZWingOffset == 0) {
        
        beeZWingOffset = 1.0;
    }
    model_trans *= Translate(0, beeYWingOffset, beeZWingOffset);
    model_trans *= RotateX(beeWingAngle);
    model_trans *= Scale(0.5, 0.0625, 1.5);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(120), getRgbFloat(120), getRgbFloat(120));
    drawCube();
    
    // model left bee wing
    model_trans = mvstack.pop();
    mvstack.push(model_trans);
    model_trans *= Translate(0, beeYWingOffset, -beeZWingOffset);
    model_trans *= RotateX(-beeWingAngle);
    model_trans *= Scale(0.5, 0.0625, 1.5);
    model_view = view_trans * model_trans;
    set_colour(getRgbFloat(120), getRgbFloat(120), getRgbFloat(120));
    drawCube();
    
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
        
        float cosTime = cos(TIME / TwoPI);
        float sinTime = sin(TIME / TwoPI);
        
        // calculate bee offsets as a function of TIME
        beeXOffset = - sinTime * 5;
        beeYOffset = sin(fmod((TIME / TwoPI), 1.0) * TwoPI);
        beeZOffset = cosTime * 5;
        
        // calculate bee Y Rotation Angle as a function of TIME
        float d = TwoPI * TwoPI;
        beeYRotationAngle = fmod((360.0 - (fmod((TIME / d), TwoPI)) * 360.0), 360);
        
        // calculate bee wing angle as a function of TIME
        beeWingAngle = sin(fmod((TIME / TwoPI), 1.0) * TwoPI) * 45;
        
        // calculate bee wing offsets as a function of beeWingAngle
        beeYWingOffset = 0.25 - (0.75 * sin(beeWingAngle * DegreesToRadians)) + (0.03125 * cos(beeWingAngle * DegreesToRadians));
        beeZWingOffset = 0.25 + (0.75 * cos(beeWingAngle * DegreesToRadians)) + (0.03125 * sin(beeWingAngle * DegreesToRadians));
        
        // calculate bee leg angles as a funcion of TIME
        beeUpperLegAngle = (((sin(fmod((TIME / TwoPI), 1.0) * TwoPI) + 1.0) / 2.0) * 10);
        beeLowerLegAngle = (((sin(fmod((TIME / TwoPI), 1.0) * TwoPI) + 1.0) / 2.0) * 7.5);
        
        // calculate bee upper leg offsets as a function of beeUpperLegAngle
        beeYUpperLegOffset = -0.25 - (0.25 * cos(beeUpperLegAngle * DegreesToRadians)) - (0.0625 * sin(beeUpperLegAngle * DegreesToRadians));
        beeZUpperLegOffset = 0.25 - (0.25 * sin(beeUpperLegAngle * DegreesToRadians)) + (0.0625 * cos(beeUpperLegAngle * DegreesToRadians));
        
        // calculate bee lower leg offsets as a function of beeLowerLegAngle
        beeYLowerLegOffset = -0.25 - (0.25 * cos(beeLowerLegAngle * DegreesToRadians)) - (0.0625 * sin(beeLowerLegAngle * DegreesToRadians));
        beeZLowerLegOffset = -0.0625 - (0.25 * sin(beeLowerLegAngle * DegreesToRadians)) + (0.0625 * cos(beeLowerLegAngle * DegreesToRadians));
        
        // calculate tree rotation angle as a function of TIME
        treeRotationAngle = sin(fmod(((TIME / TwoPI) / 4), 1.0) * TwoPI) * 1.0;

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




