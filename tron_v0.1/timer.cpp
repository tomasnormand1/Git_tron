#include "timer.h"
#include <GL/glut.h>


timer::timer()
{
    //ctor
    start_ticks = 0;
    started = false;
    paused_ticks = 0;
    paused = false;

}

void timer::start()
{
    started = true;
    paused = false;
    start_ticks = glutGet(GLUT_ELAPSED_TIME);
}

void timer::stop()
{
    started = false;
    paused = false;
}

int timer::get_ticks() const
{
    if(started == true)
    {
        if(paused == true)
        {
            return paused_ticks;
        }
        else
        {
            return glutGet(GLUT_ELAPSED_TIME) - start_ticks;
        }
    }
    return 0;
}

bool timer::is_started() const
{
    return started;
}

bool timer::is_paused() const
{
    return paused;
}

timer::~timer()
{
    //dtor
}
