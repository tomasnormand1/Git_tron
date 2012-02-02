#ifndef ACTOR_H
#define ACTOR_H

#include "body.h"

#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4


class actor : public body
{
    public:
        void change_dir(int dir);
        virtual ~actor();
    protected:
    private:
};

#endif // ACTOR_H
