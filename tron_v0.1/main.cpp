// system includes
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>

#include <GL/glut.h>    // Header File For The GLUT Library
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library
#include <unistd.h>     // Header file for sleeping.
#include "SOIL/SOIL.h"

//my includes
#include "timer.h"

/* ascii code for the escape key */
#define ESCAPE 27
#define PAGE_UP 73
#define PAGE_DOWN 81
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77
#define L 108
#define F 102

/* The number of our GLUT window */
int window;

//x, y and z rotation
float xrot;
float yrot;

//cube speed in units per second
float ups = 10.0f;
//grid size
const int gridsize = 20;
//trail count
int count = 0;

//depth
float start_x = 1.0f;
float start_y = -3.0f;
float start_z = -9.0f;
float _xtrans = start_x;
float _ytrans = start_y;
float _ztrans = start_z;

//directions
const bool dir_n =false;
const bool dir_e =false;
const bool dir_s =false;
const bool dir_w =false;
bool dir_array[4] = {dir_n, dir_e, dir_s, dir_w};

//array for verticies of the trail quads
std::vector<int[12]> trail_vector;

// storage for texture
GLuint texture[3];

//array used for keyboard
bool keys[256];

// light on/off
bool light;
// lighting arrays
float lightambient[] = {0.5f, 0.5f, 0.5f, 1.0f};
float lightdiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
float lightposition[] = {0.0f, 0.0f, 2.0f, 1.0f};

// smoothing filters
uint filter;

// keys l and f
bool lp;
bool fp;

//timer
timer delta;

typedef struct
{
    float x, y, z;
} CAMERA;

void const_move(float delta_time);
void drawtrail(int _count);

int LoadGLTextures()
{
    texture[0] = SOIL_load_OGL_texture
                 (
                     //set status to true

                     "redship2.bmp",
                     SOIL_LOAD_AUTO,
                     SOIL_CREATE_NEW_ID,
                     SOIL_FLAG_INVERT_Y
                 );

    if(texture[0] == 0)
    {
        return 0;
    }


    // make 3 textures and put them in tex array
    //glGenTextures(3, &texture[0]);

    // normal texture gen using the data from the bitmap
    // first texture - nearest
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
    // texture 2 to linear
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
    // third texture - mipmapped
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    //glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
    //gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image1->sizeX, image1->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
    //gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture[0])
    return 1;
}


/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
{
    LoadGLTextures();
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);		// This Will Clear The Background Color To Black
    glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);				// The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
    glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   //perspective calculations

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();				// Reset The Projection Matrix

    gluPerspective(60.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

    glMatrixMode(GL_MODELVIEW);

    // lighting
    glLightfv(GL_LIGHT1, GL_AMBIENT, lightambient);      // set up ambient light
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightdiffuse);      // diffuse lighting
    glLightfv(GL_LIGHT1, GL_POSITION, lightposition);    // light position
    glEnable(GL_LIGHT1);
}

/* The function called when our window is resized (which shouldn't happen, because we're fullscreen) */
void ReSizeGLScene(int Width, int Height)
{
    if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
        Height=1;

    glViewport(0, 0, Width, Height);		// Reset The Current Viewport And Perspective Transformation

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(40.0f,(GLfloat)Width/(GLfloat)Height,0.5f,100.0f);
    glMatrixMode(GL_MODELVIEW);
}

/* The main drawing function. */
void DrawGLScene(void)
{
    const_move(delta.get_ticks());
    delta.start();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    glLoadIdentity();				// Reset The View
    glTranslatef(-(gridsize/2) +1, 0, -30);



    //glRotatef(zrot, 0.0f, 0.0f, 1.0f);

    // choose the texture to use
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    //glLoadIdentity();
    for(float _xtrans_l =0; _xtrans_l < gridsize; _xtrans_l+=2)
    {
        for(float _ztrans_l =0; _ztrans_l < gridsize; _ztrans_l+=2)
        {
            //glPushMatrix();
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor3ub(144, 213, 225);
            glLineWidth(1);
            glPushMatrix();
            // rotate triangle on y axis
            /**glRotatef(xrot, 0.0f, 1.0f, 0.0f);*/
            //glRotatef(0, 0.0f, 0.0f, 0.0f);
            glTranslatef(_xtrans_l, _ytrans, _ztrans_l);
            if(_xtrans_l != gridsize)
            {
                glBegin(GL_LINES);
                    glVertex3f(-1.0f, 0.0f, -1.0f);
                    glVertex3f(1.0f, 0.0f, -1.0f);
                glEnd();
                glBegin(GL_LINES);
                    glVertex3f(-1.0f, 0.0f, 1.0f);
                    glVertex3f(1.0f, 0.0f, 1.0f);
                glEnd();
            }

//            {
//                glBegin(GL_LINES);
//                    glVertex3f(0.0f, 0.0f, 0.0f);
//                    glVertex3f(2.0f, 0.0f, 0.0f);
//                glEnd();
//            }
            if(_ztrans_l != gridsize)
            {
                glBegin(GL_LINES);
                    glVertex3f(-1.0f, 0.0f, -1.0f);
                    glVertex3f(-1.0f, 0.0f, 1.0f);
                glEnd();
                glBegin(GL_LINES);
                    glVertex3f(1.0f, 0.0f, -1.0f);
                    glVertex3f(1.0f, 0.0f, 1.0f);
                glEnd();
            }

//            {
//                glBegin(GL_LINES);
//                    glVertex3f(0.0f, 0.0f, 0.0f);
//                    glVertex3f(0.0f, 0.0f, 2.0f);
//                glEnd();
//            }
            glPopMatrix();


        }
    }

    glPushMatrix();
    glTranslatef(_xtrans, _ytrans, _ztrans);

    if(dir_array[0])
    {
        //up
        glRotatef(90, 0, 1, 0);
    }
    else if(dir_array[1])
    {
        //right
        glRotatef(0, 0, 1, 0);
    }
    else if(dir_array[2])
    {
        //down
        glRotatef(-90, 0, 1, 0);
    }
    else if(dir_array[3])
    {
        //left
        glRotatef(180, 0, 1, 0);
    }
    //draw cube for player palceholder
    glBegin(GL_QUADS);
        //bottom colour
        glColor3ub(144, 213, 225);
        //bottom
        glVertex3f(-1.0f, 0.0f, -1.0f);
        glVertex3f(1.0f, 0.0f, -1.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
        glVertex3f(-1.0f, 0.0f, 1.0f);
        //left colour
        glColor3ub(144, 213, 225);
        //left
        glVertex3f(-1.0f, 0.0f, -1.0f);
        glVertex3f(-1.0f, 0.0f,  1.0f);
        glVertex3f(-1.0f, 2.0f, 1.0f);
        glVertex3f(-1.0f, 2.0f, -1.0f);
        //top colour
        glColor3ub(144, 213, 225);
        //top
        glVertex3f(-1.0f, 2.0f, -1.0f);
        glVertex3f(-1.0f, 2.0f, 1.0f);
        glVertex3f(1.0f, 2.0f, 1.0f);
        glVertex3f(1.0f, 2.0f, -1.0f);
        //right colour
        glColor3ub(144, 213, 225);
        //right bottom
        glVertex3f(1.0f, 0.0f, -1.0f);
        glVertex3f(2.0f, 1.0f, -1.0f);
        glVertex3f(2.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
        //right colour
        glColor3ub(144, 213, 225);
        // right top
        glVertex3f(1.0f, 2.0f, -1.0f);
        glVertex3f(2.0f, 1.0f, -1.0f);
        glVertex3f(2.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, 2.0f, 1.0f);
        //front colour
        glColor3ub(144, 213, 225);
        //front
        glVertex3f(-1.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 2.0f, 1.0f);
        glVertex3f(-1.0f, 2.0f, 0.0f);
        //back colour
        glColor3ub(144, 213, 225);
        //back
        glVertex3f(-1.0f, 0.0f, -1.0f);
        glVertex3f(-1.0f, 2.0f, -1.0f);
        glVertex3f(1.0f, 2.0f, -1.0f);
        glVertex3f(1.0f, 0.0f, -1.0f);
    glEnd();

    glPopMatrix();
    //push mid points of top and bottom of cube into vector
    // top
//    int vertexarray[12] = {};
//    trail_vector.push_back();

    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    //count = 0;
    float _xtrans_int = _xtrans;

    if( _xtrans_int > 2)
    {
        count ++;
        _xtrans_int = 0;
    }
    std::cout << _xtrans_int << std::endl;



    for(int i = 0; i < count; i++)
    {
        glPushMatrix();
            drawtrail(count);
        glPopMatrix();
    }


    // since this is double buffered, swap the buffers to display what just got drawn.
    glutSwapBuffers();

}

//********************

// TODO fix teh trail

//********************
void drawtrail(int _count)
{
    //trails
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    glTranslatef(_xtrans, _ytrans, _ztrans);
    //horizontal trail

    int tmp_array[12] = {-1, 0, 0, -1, 2, 0, -2, 2, 0, -2, 0, 0};

    glBegin(GL_QUADS);
        glVertex3f(tmp_array[0], tmp_array[1], tmp_array[2]);
        glVertex3f(tmp_array[3], tmp_array[4], tmp_array[5]);
        glVertex3f(tmp_array[6], tmp_array[7], tmp_array[8]);
        glVertex3f(tmp_array[9], tmp_array[10], tmp_array[11]);
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
}

/* The function called whenever a key is pressed. */
void keyPressed(unsigned char key, int x, int y)
{
    /* avoid thrashing this procedure */
    usleep(100);

    /* If escape is pressed, kill everything. */
    if (key == ESCAPE)
    {
        /* shut down our window */
        glutDestroyWindow(window);

        /* exit the program...normal termination. */
        exit(0);
    }
    // enable and disable lighting with 'L' key
    if (key == L)
    {
        lp = true;
        light = !light;
        if (!light)
        {
            glDisable(GL_LIGHTING);
        }
        else
        {
            glEnable(GL_LIGHTING);
        }
    }
    if(key != L)
    {
        lp = false;
    }
    // change filters
    if (key == F)
    {
        fp = true;
        filter ++;
        if(filter > 2)
        {
            filter = 0;
        }
    }
    if(key != F)
    {
        fp = false;
    }

}

void action_keys(int key, int x, int y)
{
    usleep(100);


    if(key == GLUT_KEY_PAGE_UP)
    {
        xrot += 1.0f;
    }
    if(key == GLUT_KEY_PAGE_DOWN)
    {
        xrot -= 1.0f;
    }
    if(key == GLUT_KEY_UP && !dir_array[2])
    {
//        _ztrans -= 2.0f;
        dir_array[0] = true;
        // make all the other directions false
        dir_array[1] = false;
        dir_array[2] = false;
        dir_array[3] = false;
    }
    if(key == GLUT_KEY_DOWN && !dir_array[0])
    {
        dir_array[2] = true;
        // make all the other directions false
        dir_array[0] = false;
        dir_array[1] = false;
        dir_array[3] = false;
    }
    if(key == GLUT_KEY_LEFT && !dir_array[1])
    {
//        _xtrans -= 2.0f;
        dir_array[3] = true;
        // make all the other directions false
        dir_array[1] = false;
        dir_array[2] = false;
        dir_array[0] = false;
    }
    if(key == GLUT_KEY_RIGHT && !dir_array[3])
    {
//        _xtrans += 2.0f;
        dir_array[1] = true;
        // make all the other directions false
        dir_array[0] = false;
        dir_array[2] = false;
        dir_array[3] = false;
    }
}

void const_move(float delta_time)
{
    /*
    for snapping to grid - doesnt work yet

    float new_xtrans_a = _xtrans*10;
    float new_ztrans_a = _ztrans*10;
    int new_xtrans_b = (int)new_xtrans_a;
    int new_ztrans_b = (int)new_ztrans_a;
    float final_xtrans = ( (float)new_xtrans_b)/10;
    float final_ztrans = ( (float)new_ztrans_b)/10;

    float remain_x = final_xtrans - ((int)final_xtrans);
    float remain_z = final_ztrans - ((int)final_ztrans);
    */

        if(dir_array[0] )
        {
            _ztrans -= ups * (delta_time / 1000.0f);
        }
        else if(dir_array[1] )
        {
            _xtrans += ups * (delta_time / 1000.0f);
        }
        else if(dir_array[2] )
        {
            _ztrans += ups * (delta_time / 1000.0f);
        }
        else if(dir_array[3] )
        {
            _xtrans -= ups * (delta_time / 1000.0f);
        }

//        std::cout << "xtrans " << remain_x << std::endl;
//        std::cout << "ztrans " << remain_z << std::endl;
}

void mouserotate(int x, int y)
{
    xrot = x/4;
    yrot = y/4;

    //glutWarpPointer(640/2, 400/2);
}

int main(int argc, char **argv)
{
    /* Initialize GLUT state - glut will take any command line arguments that pertain to it or
       X Windows - look at its documentation at http://reality.sgi.com/mjk/spec3/spec3.html */
    glutInit(&argc, argv);

    /* Select type of Display mode:
       Double buffer
       RGBA color
       Alpha components supported
       Depth buffer */
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);

    /* get a 640 x 480 window */
    glutInitWindowSize(640, 480);

    /* the window starts at the upper left corner of the screen */
    glutInitWindowPosition(0, 0);

    /* Open a window */
    window = glutCreateWindow("tomas - opengl");

    /* Register the function to do all our OpenGL drawing. */
    glutDisplayFunc(&DrawGLScene);

    /* Go fullscreen.  This is as soon as possible. */
    glutFullScreen();

    /* Even if there are no events, redraw our gl scene. */
    glutIdleFunc(&DrawGLScene);

    /* Register the function called when our window is resized. */
    glutReshapeFunc(&ReSizeGLScene);

    /*passive mouse control*/
    glutPassiveMotionFunc(&mouserotate);

    /* Register the function called when the keyboard is pressed. */
    glutKeyboardFunc(&keyPressed);
    glutSpecialFunc(&action_keys);


    /* Initialize our window. */
    InitGL(640, 480);

    /* Start Event Processing Engine */
    glutMainLoop();

    return 1;
}
