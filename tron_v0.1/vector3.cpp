#include <math.h>

#include "vector3.h"

vector3::vector3(float x, float y, float z)
{
    //ctor
    xpos = x;
    ypos = y;
    zpos = z;
}

vector3& vector3::operator+ (const vector3 &v)
{
    float tmpx, tmpy, tmpz;
    tmpx = v.xpos + xpos;
    tmpy = v.ypos + ypos;
    tmpz = v.zpos + zpos;

    vector3 _v(tmpx, tmpy, tmpz);
    return _v;
}

float vector3::magnitude() const
{
    float mag = sqrtf((xpos * xpos) + (ypos * ypos) + (zpos *zpos));
    return mag;
}

const float vector3::getanglexz(const vector3 & v) const
{
    // get the heading angle given a vector on the x/z axis
    float heading_angle;

    heading_angle = (float)atan2f(v.xpos, v.zpos);
    return heading_angle;
}

const float vector3::getanglexy(const vector3 & v) const
{
    // get the heading angle given a vector on the x/y axis
    float heading_angle;

    heading_angle = (float)atan2f(v.xpos, v.ypos);
    return heading_angle;
}

float vector3::get_x() const
{
    return xpos;
}

float vector3::get_y() const
{
    return ypos;
}

float vector3::get_z() const
{
    return zpos;
}

vector3::~vector3()
{
    //dtor
}
