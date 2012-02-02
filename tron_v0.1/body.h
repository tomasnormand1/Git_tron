#ifndef BODY_H
#define BODY_H


class body
{
    public:
        virtual void draw() =0;
        virtual void move() =0;
        virtual ~body();
    protected:
        // position in 3d space
        float xpos;
        float ypos;
        float zpos;

        // direction
        float xdir;
        float ydir;
        float zdir;

        // verticies of body

    private:
};

#endif // BODY_H
