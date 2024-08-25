#include "ProgressBar.h"

ProgressBar::ProgressBar (uint32_t total, int8_t X, int8_t Y) {

    this->total = total;
    this->current = 0;
    this->progress = 0;
    this->X = X;
    this->Y = Y; 
}

void 
ProgressBar::update (uint32_t value) {

    this->current = value;
    this->progress = (double)((this->current * 100) / this->total);
}

void 
ProgressBar::draw() {

}
