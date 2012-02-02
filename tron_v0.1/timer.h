#ifndef TIMER_H
#define TIMER_H


class timer
{
    public:
        timer();
        void start();
        void stop();
        int get_ticks() const;
        bool is_started() const;
        bool is_paused() const;
        virtual ~timer();
    protected:
    private:
        int start_ticks;
        int paused_ticks;
        bool started;
        bool paused;
};

#endif // TIMER_H
