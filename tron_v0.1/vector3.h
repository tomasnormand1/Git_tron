#ifndef VECTOR3_H
#define VECTOR3_H


class vector3
{
    public:
        vector3(float x, float y, float z);
        vector3 & operator+ (const vector3 &v);
        float magnitude() const;
        const float getanglexz(const vector3 & v) const;
        const float getanglexy(const vector3 & v) const;
        float get_x() const;
        float get_y() const;
        float get_z() const;
        virtual ~vector3();
    protected:
    private:
        //current position
        float xpos;
        float ypos;
        float zpos;
};

#endif // VECTOR3_H
