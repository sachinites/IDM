#ifndef __PROGRESS_BAR__
#define __PROGRESS_BAR__

#include <stdint.h>

class ProgressBar {

    private:
        uint32_t total;
        uint32_t current;
        double progress;
        int8_t X, Y;

    protected:
    public:
        ProgressBar(uint32_t total, int8_t X, int8_t Y);
        void update(uint32_t value) ;
        void draw();
}; 

#endif 